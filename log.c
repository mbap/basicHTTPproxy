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
