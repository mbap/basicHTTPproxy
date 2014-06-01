// File: log.c    
// Created May 30, 2014
// Michael Baptist - mbaptist@ucsc.edu

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

// comment this out to turn on debug prints.
//#define NDEBUG NDEBUG

#include "log.h"
#include "utils.h"

#define SUCCESS    0
#define FAILURE    1



void get_current_time_formatted(char *formatted_time, int len) {
   bzero(formatted_time, len);
   time_t currtime = time(NULL);
   if (currtime == ((time_t) - 1)) {
       fprintf(stderr, "Error: time(3) had an error."
             "Log file will have invalid date time string.\n");

   }
   struct tm *timeinfo = localtime(&currtime);
   int x = strftime(formatted_time, len, "%a %b %d %T %Z %Y", timeinfo);
   if (x == 0) {
      fprintf(stderr, "Error: strftime failed.\n");
   }   
}


void log_request(const char* time, const char *req, const char *version, const char *host, const char *uri, const char *ip, const char *action, const char *errors) {
    FILE *logfile = fopen("proxy.log", "a");
    if (time != NULL) {
        write(fileno(logfile), time, strlen(time));
    }
    if (req != NULL) {
        write(fileno(logfile), req, strlen(req));
    }
    if (version != NULL) {
        write(fileno(logfile), version, strlen(version));
    }
    if (host != NULL) {
        write(fileno(logfile), host, strlen(host));
    }
    if (uri != NULL) {
        write(fileno(logfile), uri, strlen(uri));
    }
    if (ip != NULL) {
        write(fileno(logfile), ip, strlen(ip));
    }
    if (action != NULL) {
        write(fileno(logfile), action, strlen(action));
    }
    if (errors != NULL) {
        write(fileno(logfile), errors, strlen(errors));
    }
    fclose(logfile);
}



