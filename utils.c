// File: utils.c    
// Created April 7, 2014
// Michael Baptist - mbaptist@ucsc.edu

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/stat.h>

// comment this out to turn on debug prints.
//#define NDEBUG NDEBUG

#include "utils.h"

#define SUCCESS    0
#define FAILURE    1


void check_socket(int fd) {
   if (fd < 0) {
      perror("Error: Socket Corrupt ");
      exit(errno);
   } else {
      DEBUGF("Client Socket: %d In Use\n", fd);
   }
}

void check_connection(int x) {
  if (x < 0) {
    perror("Error: Connection Failed ");
    exit(errno);
  } else {
    DEBUGF("Connection Established\n");
    DEBUGF("Server Socket %d Connected\n", x);
  }
}

FILE *retrieve_file(const char *restrict filename, const char* restrict mode) {
   DIR *d = NULL;
   struct dirent *dir = NULL;
   d = opendir(".");
   if (d) {
      while ((dir = readdir(d)) != NULL) {
          if (strcmp(dir->d_name, filename) == 0) {
             FILE *file = fopen(filename, mode);
             closedir(d);
             return file; 
          }
      }
      fprintf(stderr, "Error: no file matching %s exists in current directory.\n", filename);
      return NULL;
   }
   fprintf(stderr, "Error: no directory matching . exists.\n");
   return NULL;
}

int get_file_size(FILE *restrict file) {
   fseek(file, 0, SEEK_END); // seek to end of file
   int size = (int)ftell(file); // get current file pointer
   fseek(file, 0, SEEK_SET); // seek back to beginning of file
   return size;
}

unsigned char *serialize_int(unsigned char *buffer, unsigned int val) {
    unsigned int size = sizeof(unsigned int);
    for (unsigned int i = 0; i < size; ++i) {
        buffer[i] = val >> (8*(size-i-1));
    }
    return buffer + size;
}

unsigned char *serialize_data(unsigned char *buffer, char buf[], int len) {
   for (int i = 0; i < len; ++i) {
       buffer[i] = (unsigned char)buf[i];
   }   
   return buffer + len;
}

unsigned char *deserialize_int(unsigned char *buffer, unsigned int *val) {
    unsigned int size = sizeof(unsigned int);
    *val = 0;
    for (unsigned int i = 0; i < size; ++i) {
        unsigned int x = (unsigned int)buffer[i];
        *val +=(x << (8*(size-i-1)));
    }
    return buffer + size;
}

unsigned char *deserialize_data(unsigned char *buffer, char buf[], int len) {
   for (int i = 0; i < len; ++i) {
       buf[i] = (char)buffer[i];
   }   
   return buffer + len;
}

void debugprintf(char *format, ...) {
   va_list args;
   fflush (NULL);
   va_start (args, format);
   fprintf (stdout, "DEBUG: ");
   vfprintf (stdout, format, args);
   va_end (args);
   fflush (NULL);
}

