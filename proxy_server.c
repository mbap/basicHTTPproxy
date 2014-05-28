// File: server.c    
// Created April 7, 2014
// Michael Baptist - mbaptist@ucsc.edu

/*******
NAME
     server -- returns a formatted time to a requesting client

SYNOPSIS
     server [Port]

DESCRIPTION  
     This program accepts the client port number as it's arguments,
     and while running, if contacted by a client, returns a chunk 
     of a file to the user.

OPERANDS
     The only operand is a valid unused port number. If no port 
     number is input to the program, the program will exit with
     an error.

EXIT STATUS

     0    No errors were detected.
     1    One or more errors were detected and error messages were
          printed.
 errno    If a man page call detects an error, perror prints a sys
          message and exits with return value of errno.

******/


#include <stdio.h>       // standard I/O
#include <stdlib.h>      // standard library
#include <string.h>      // conatains memset
#include <sys/types.h>   // prerequisite typedefs 
#include <errno.h>       // error numbers for sys calls
#include <sys/socket.h>  // struct sockaddr; system prototypes and consts
#include <netdb.h>       // network info lookup prototypes and stuctures 
#include <netinet/in.h>  // struct sockaddr_in; byte ordering macros
#include <arpa/inet.h>   // utility function prototypes
#include <unistd.h>      // uni-standard lib.h
#include <sys/select.h>  // used for select io multiplexing.
#include <time.h>        // for the random numbers
#include <sys/ioctl.h>   // allows nonblocking sockets.
#include <pthread.h>     // allows for threaded server.

// comment this out to turn on debug print statements.
//#define NDEBUG NDEBUG

#include "utils.h"
#include "http_parser.h"

#define SUCCESS   0
#define FAILURE   1
#define TRUE      1
#define FALSE     0

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

static uint8_t exit_status = SUCCESS;

// handle a client request gets the data from the server and sends it.
void *handle_client_request(void *clisock);

// closes a client socket and removes it from an fd_set
void close_client(int clisock, fd_set *master);


int main(int argc, char **argv) {
  //initial error checking
  if (argc != 2) {
    fprintf(stderr, "Error: Include Listening Port Number.\n");
    fprintf(stderr, "Usage: %s [PORT]\n", argv[0]);
    exit_status = FAILURE;
    return FAILURE;
  }
  char *endptr = NULL;
  int portnum = (int)strtol(argv[1], &endptr, 10);
  if (0 == portnum || *endptr != '\0') {
     // error handling not valid port number
     fprintf(stderr, "Error: Invalid Port Number: %s\n", argv[1]);
     exit_status = FAILURE;
     return FAILURE;
  } else {
     DEBUGF("Server creates connections on port number: %d\n", portnum);
  }

  //create socket for the server 
  int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket < 0) {
     perror("ERROR: SOCKET CORRUPT ");
     return errno;
  } else {
     DEBUGF("Server Socket: %d\n", listen_socket);
  }

  // uncomment this if you want nonblocking sockets.
  // commented means that threads will block on accept and read and such.
  /*  
  int rc = 0;
  int on = 1;
  rc = ioctl(listen_socket, FIONBIO, (char *)&on);
  if (rc < 0) {
      perror("ioctl() failed");
      close(listen_socket);
      exit(FAILURE);
  }
  */

  // bind the new socket the port passed into the command line.
  sockaddr_in my_serv;
  my_serv.sin_family = AF_INET;
  my_serv.sin_port = htons(portnum);
  my_serv.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(my_serv.sin_zero, '\0', sizeof(my_serv.sin_zero));
  sockaddr_in *my_serv_ref = &my_serv;
  int er_chk = bind(listen_socket, (sockaddr*)my_serv_ref, sizeof(my_serv));
  if (er_chk < 0) {
     perror("Error: Socket to Address bind failure ");
     return errno;
  } else {
     DEBUGF("Bind Success.\n");
  }

  // make the new socket a listening socket.
  int lis_er_chk = listen(listen_socket, 10);
  if (lis_er_chk < 0) {
      perror("Error: connection failure. ");
      return errno;
  } 

  // loop forever to accept new connections.
  while (1) {
      DEBUGF("Main thread waiting for connections on listening port.\n");

      // data structure to hold client ip and port info.
      sockaddr_in client;
      uint clen = (uint)sizeof(client);

      // accept the new client
      int client_socket = accept(listen_socket, (sockaddr *)&client, &clen);
      if (client_socket == -1) {
          fprintf(stderr, "Error: accept(2) failed.\n");
      } else {
          DEBUGF("New connection. IP: %s. Port: %hu.\n", inet_ntoa(client.sin_addr), client.sin_port);
      }
      
      // set up thread for new connection
      pthread_t thread_ID; 
      int i = pthread_create(&thread_ID, NULL, 
                             handle_client_request,(void *)&client_socket);
      if (i == EAGAIN) {
          fprintf(stderr, "Error: Insufficient resources to create another \
                           thread,  or  a  system imposed  limit  on  \
                           the  number of threads was encountered.  \
                           The latter case  may  occur  in  two  ways:\
                           the  RLIMIT_NPROC  soft resource  limit  (set \
                           via setrlimit(2)), which limits the number of \
                           process for a real user ID, was reached; \
                           or the kernel's system-wide   limit   on  \
                           the  number  of  threads,  /proc/sys/kernel\
                           /threads-max, was reached.\n");
      } else if (i == EINVAL) {
          fprintf(stderr, "Error: Invalid settings in attr.\n");
      } else if (i == EPERM) {
          fprintf(stderr, "Error: No permission to set the scheduling\
                           policy and parameters specified in attr.\n");
      }
  }

  // program never gets here, but if there was an exit 
  // server this would close the server. 
  close(listen_socket);

  return exit_status;
}


// THREAD FUNCTION
void *handle_client_request(void *c) {
    DEBUGF("New pthread created to handle client.\n");

    // get client socket from the parameter c
    int *clisock = (int *)c;
    int client_socket = *clisock;

    // get peer name
    sockaddr_in client_info;
    uint clen = (uint)sizeof(client_info);
    int gpn_check = getpeername(client_socket, (sockaddr *)&client_info, &clen); 
    if (gpn_check < 0) {
        fprintf(stderr, "getpeername(2) failed. Client info couldn't be \
                         gathered aborting connection.");
        return (void *)FAILURE;
    } else {
        DEBUGF("client address: %s, port: %hu\n", 
                inet_ntoa(client_info.sin_addr), client_info.sin_port);
    }
 
   // create MAIN timeout for if the connection goes dead for a while then
   // exit the thread. here i can use alarm() and signal(SIGARLRM, handler)
   // or the timer method from the forums.

   // create fd_set to use for the timeout. 
   fd_set master;            // master list of file descriptors.
   FD_ZERO(&master);
   FD_SET(client_socket, &master); // add listening socket to master list.

   /*
   // loop forever until the exit or done message is given.
   // if the timeval tv expires in select a retranmission should occur.
   int breakloop = FALSE;  // var to break out of while loop below
   int connection_timeouts = 0; // sets breakloop. counts timeouts
   while (1) {

       DEBUGF("Posix thread waiting on select().\n");
       struct timeval tv = {0,0};
       tv.tv_sec = 5;
       fd_set read_fds = master;

       if (select(client_socket + 1, &read_fds, NULL, NULL, &tv) < 0) {
          fprintf(stderr, "Error: select() failed.\n");
          if (errno == EBADF) {
             fprintf(stderr, "Error: select() failed due to bad descriptor.\n");
             breakloop = TRUE;
          }
       }
       DEBUGF("select() has returned.\n");

       if (FD_ISSET(client_socket, &read_fds)) {
          // process client response.
          unsigned char buffer[1100];
          bzero(buffer, sizeof(buffer));
       } else {
          // handle timeout
          // retransmit last packet.
          connection_timeouts++;
          if (connection_timeouts > 5) {
              breakloop = TRUE;
          }
       }

       // if breakloop flag is set we must have a bad connection.
       if (breakloop) {
           break;
       }
   }
   */

   // recv http message from browser.
   char buf[8*1024];
   bzero(buf, sizeof(buf));
   read(client_socket, buf, sizeof(buf));
   DEBUGF("HTTP MESSAGE\n%s", buf);

   // log time of received message

   // parse http message.  
   int newlines = count_newlines(buf, sizeof(buf));
   DEBUGF("newlines:%d\n", newlines);

   // check if allowed server and supported msg (GET HEAD POST only) 
   // respond with 501 if bad msg

   // check for missing fields in the http msg
   // respond with 400 if bad

   // get server ip and port from the hostname.


   // client served succesful close socket and exit thread.
   close_client(client_socket, &master);  
   pthread_exit((void *)SUCCESS);
}

void close_client(int clisock, fd_set *master) {
   DEBUGF("closing client socket: %d.\n", clisock);
   close(clisock);
   FD_CLR(clisock, master);
}




