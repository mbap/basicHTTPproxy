// File: http_response.c    
// Created June 9, 2014
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

#include "http_response.h"
#include "utils.h"

#define SUCCESS    0
#define FAILURE    1

void forbidden_response(char *buf, size_t buflen) {
    char buffer[buflen];
    bzero(buffer, buflen);
    strncpy(buffer, "HTTP/1.1 403 Forbidden\r\n", 24);
    strncat(buffer, "Content-type: text/html\n\n", 25);
    strncat(buffer, "<html>\n", 7);
    strncat(buffer, " <body>\n", 8);
    strncat(buffer, "  <h1>403 Forbidden</h1>\n", 25);
    strncat(buffer, "  <p>Access to this site is denied.</p>\n", 36);
    strncat(buffer, " <body>\n", 8);
    strncat(buffer, "<html>\n", 7);
    strncat(buffer, "\r\n", 2);
    strncat(buffer, "\r\n", 2);
    memcpy(buf, buffer, buflen);
}

void unimplmented_response(char *buf, size_t buflen) {
    char buffer[buflen];
    bzero(buffer, buflen);
    strncpy(buffer, "HTTP/1.1 501 Not Implemented\r\n", 24);
    strncat(buffer, "Content-type: text/html\n\n", 25);
    strncat(buffer, "<html>\n", 7);
    strncat(buffer, " <body>\n", 8);
    strncat(buffer, "  <h1>501 Not Implemented</h1>\n", 31);
    strncat(buffer, "  <p>The HTTP request you made is not implemented.</p>\n", 55);
    strncat(buffer, " <body>\n", 8);
    strncat(buffer, "<html>\n", 7);
    strncat(buffer, "\r\n", 2);
    strncat(buffer, "\r\n", 2);
    memcpy(buf, buffer, buflen);
}


// returns a http response
char *http_response(const uint status) { 
    char *buffer = calloc(1, 1024);
    if (403 == status) {
        forbidden_response(buffer, 1024); 
        return buffer;
    } else if (501 == status) {
        unimplmented_response(buffer, 1024);
        return buffer;
    }
    return NULL;
}





