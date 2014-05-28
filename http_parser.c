// File: http_parser.c    
// Created May 27, 2014
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

#include "http_parser.h"

#define SUCCESS    0
#define FAILURE    1

// counts new lines in an http message
int count_newlines(char buffer[], int bufferlen) {
    int c = 0;
    for (int i = 0; i < bufferlen;  i++) {
        if (buffer[i] == '\n') c++;
    }
    return c;
}

