// File: server.c    
// Created April 7, 2014
// Michael Baptist - mbaptist@ucsc.edu

/*******
NAME
     proxy_server -- creates a communication path between a client
                     and a server using TCP.

SYNOPSIS
     proxy_server [Port]

DESCRIPTION  
     This program accepts the port number as it's arguments,
     and while running, if contacted by a client, it contacts 
     the server by forwarding the clients http request. It then
     attempts to recieve the request data from the server and 
     relay it back to the clients browser.

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
#include "http_response.h"
#include "log.h"
#include "siteblock.h"

#define SUCCESS   0
#define FAILURE   1
#define TRUE      1
#define FALSE     0
#define BUFLEN    8*1024

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

   // recv http message from browser.
   char buf[BUFLEN];
   bzero(buf, sizeof(buf));
   read(client_socket, buf, sizeof(buf));
   DEBUGF("HTTP MESSAGE\n%s", buf);

   // log time of received message
   char timestamp[64];
   get_current_time_formatted(timestamp, sizeof(timestamp));
   DEBUGF("%s\n", timestamp);
    
   // parse http message.  
   size_t numlines = count_newlines(buf, sizeof(buf));
   DEBUGF("newlines:%d\n", numlines);
   char *httpstrs[numlines];
   null_array(httpstrs, numlines);
   parse_http_header(httpstrs, buf, numlines);

   // parse http line.
   // look for http command and version number 
   char *httptokens[100];
   null_array(httptokens, 100);
   parse_http_header_line(httptokens, httpstrs[0], 100);
   
   // check if allowed server and supported msg (GET HEAD POST only) 
   // respond with 501 if bad msg
   char command[16];
   char uri[1024];
   char http_version[16];
   if (strncmp("GET", httptokens[0], sizeof(httptokens[0])) == 0  || 
       strncmp("POST", httptokens[0], sizeof(httptokens[0])) == 0 ||
       strncmp("HEAD", httptokens[0], sizeof(httptokens[0])) == 0) {
       DEBUGF("%s command.\n", httptokens[0]);
       bcopy(httptokens[0], command, strlen(httptokens[0]));
       bcopy(httptokens[1], uri, strlen(httptokens[1]));
       bcopy(httptokens[2], http_version, strlen(httptokens[2]));
       free_parse_allocs(httptokens, 100);
   } else {
       //log_request(timestamp, httptokens[0], httptokens[2], inet_ntoa(client_info.sin_addr), httptokens[1], NULL, "Filtered", "HTTP command not supported");
       fprintf(stderr, "%s command not supported.\n", httptokens[0]);
       char *response = http_response(501);
       send(client_socket, response, strlen(response), MSG_NOSIGNAL);
       free(response);
       free_parse_allocs(httptokens, 100);
       free_parse_allocs(httpstrs, numlines);
       close_client(client_socket, &master);  
       pthread_exit((void *)FAILURE);
   }


   // check for missing fields in the http msg
   // respond with 400 if bad
   // UNIMPLMENTED
   
   // get the host and get the server and ip of host.
   null_array(httptokens, 100);
   parse_http_header_line(httptokens, httpstrs[1], 100);
   DEBUGF("HOST: %s\n", httptokens[1]);

   // check the parental_controls log if the site is allowed.
   if (allowed_site(httptokens[1]) == -1) {
       //log_request(timestamp, command, http_version, inet_ntoa(client_info.sin_addr), uri, NULL, "Filtered", "Site Blocked");
       fprintf(stderr, "Error: site not allowed by proxy.\n"); 
       char *response = http_response(403);
       send(client_socket, response, strlen(response), MSG_NOSIGNAL);
       free(response);
       free_parse_allocs(httpstrs, numlines);
       free_parse_allocs(httptokens, 100);
       close_client(client_socket, &master);  
       pthread_exit((void *)FAILURE);
   }

   // do the dns lookup.
   struct hostent *host = gethostbyname(httptokens[1]);
   if (host == NULL) {      
       //log_request(timestamp, command, http_version, inet_ntoa(client_info.sin_addr), uri, NULL, "Filtered", "DNS Failed");
       fprintf(stderr, "Error: gethostbyname(3) failed\n"); 
       free_parse_allocs(httpstrs, numlines);
       free_parse_allocs(httptokens, 100);
       close_client(client_socket, &master);  
       pthread_exit((void *)FAILURE);
   }
   free_parse_allocs(httptokens, 100);

   // get info into sockaddr_in struct.
   sockaddr_in host_addr;
   bzero((char*)&host_addr,sizeof(host_addr));
   host_addr.sin_port = htons(80);
   host_addr.sin_family = AF_INET;
   bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);

   // save server address for logging.
   char *serv_addr_str = inet_ntoa(host_addr.sin_addr);
   DEBUGF("IP: %s, Port: %hu.\n", serv_addr_str, host_addr.sin_port);
      
   // create socket to forward the request.
   DEBUGF("Thread creating socket to connect to server.\n");
   int forward_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (forward_socket < 0) {
       //log_request(timestamp, command, http_version, inet_ntoa(client_info.sin_addr), uri, serv_addr_str, "Filtered", "Socket(2) Failed");
       fprintf(stderr, "Error: creating socket to forward the request"
                       "has failed. Aborting request.\n");
       free_parse_allocs(httpstrs, numlines);
       close_client(client_socket, &master);  
       pthread_exit((void *)FAILURE);
   }

   // connect the to the server using the new socket.
   DEBUGF("Thread attempting to connect to the server, on socket %d.\n", forward_socket);
   int servcon = connect(forward_socket, (sockaddr *)&host_addr, sizeof(sockaddr));
   if (servcon < 0) {
       //log_request(timestamp, command, http_version, inet_ntoa(client_info.sin_addr), uri, serv_addr_str, "Filtered", "Connection to server failed");
       fprintf(stderr, "Error: Connection Failed. Aborting request.\n");
       close(forward_socket);
       close_client(client_socket, &master);  
       pthread_exit((void *)FAILURE);
   } 

   // write the request to the server. 
   // NOTE: failed to check if all bytes were written.
   DEBUGF("Forwarding client http request to the server.\n");
   int wchk = send(forward_socket, buf, sizeof(buf), MSG_NOSIGNAL);
   free_parse_allocs(httpstrs, numlines);
   if (wchk < 0) {
       //log_request(timestamp, command, http_version, inet_ntoa(client_info.sin_addr), uri, serv_addr_str, "Forwarding", "Client Forwarding Failed");
       fprintf(stderr, "Error: request forward to real server failed.\n");
       fprintf(stderr, "Error: Aborting request.\n");
       close(forward_socket);
       close_client(client_socket, &master);  
       pthread_exit((void *)FAILURE);
   } 


   // get the response from the server.
   char forwarder[500];
   int n = 1;
   DEBUGF("Replying to client response by forwarding server response to client.\n");
   while (n > 0) {
      bzero(forwarder, sizeof(forwarder));
      n = recv(forward_socket, forwarder, sizeof(forwarder), MSG_NOSIGNAL);
      if(!(n <= 0)) {
         int wc = send(client_socket, forwarder, sizeof(forwarder), MSG_NOSIGNAL);
         if (wc < 0) {
             //log_request(timestamp, command, http_version, inet_ntoa(client_info.sin_addr), uri, serv_addr_str, "Forwarding", "Server Forwarding Failed");
             fprintf(stderr, "Error: request forward from real server failed.\n");
             fprintf(stderr, "Error: Aborting request.\n");
             close(forward_socket);
             close_client(client_socket, &master);  
             pthread_exit((void *)FAILURE);
         }
      }
   }

   // client served succesful close socket and exit thread.
   //log_request(timestamp, command, http_version, inet_ntoa(client_info.sin_addr), uri, serv_addr_str, "Forwarded", NULL);
   close(forward_socket);
   close_client(client_socket, &master);  
   pthread_exit((void *)SUCCESS);
}

void close_client(int clisock, fd_set *master) {
   DEBUGF("closing client socket: %d.\n", clisock);
   close(clisock);
   FD_CLR(clisock, master);
}














