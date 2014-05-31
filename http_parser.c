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
#include "utils.h"

#define SUCCESS    0
#define FAILURE    1

// counts new lines in an http message
size_t count_newlines(char buffer[], size_t bufferlen) {
    size_t c = 0;
    for (size_t i = 0; i < bufferlen;  i++) {
        if (buffer[i] == '\n') c++;
    }
    return c;
}

void parse_http_header(char *lines[], char *buf, size_t numlines) {
    // get server ip and port from server list.
    size_t length = strlen(buf);
    char *copy = malloc(length + 1);

    // remove all '\r' chars
    size_t x = 0;
    for (x = 0; x < (length + 1)  && buf[x] != '\0'; x++) {
        if (buf[x] == '\r') {
            buf[x] = '\n';
        }
    }

    // string copy.
    size_t i = 0;
    for (i = 0; i < (length + 1)  && buf[i] != '\0'; i++) {
        copy[i] = buf[i];
    }
    for ( ; i < (length + 1); i++) {
        copy[i] = '\0';
    }

    char *tok = copy;
    i = 0;
    while ((tok = strtok(tok, "\n")) != NULL) {
        size_t len = strlen(tok);
        lines[i] = calloc(1, len + 1);
        bcopy(tok, lines[i], len + 1);
        lines[i][len] = '\0';
        tok = NULL;
        if (i < numlines) {
           ++i;
        } else {
           break;
        }
    }
    free(copy);
}

void parse_http_header_line(char *pieces[], char *line, size_t pieceslen) {
    // get server ip and port from server list.
    size_t length = strlen(line);
    char *copy = malloc(length + 1);

    // string copy.
    size_t i = 0;
    for (i = 0; i < (length + 1)  && line[i] != '\0'; i++) {
        copy[i] = line[i];
    }
    for ( ; i < (length + 1); i++) {
        copy[i] = '\0';
    }

    char *tok = copy;
    i = 0;
    while ((tok = strtok(tok, " ")) != NULL) {
        size_t len = strlen(tok);
        pieces[i] = calloc(1, len + 1);
        bcopy(tok, pieces[i], len + 1);
        tok = NULL;
        if (i < pieceslen) {
           ++i;
        } else {
           break;
        }
    }
    free(copy);
}


void free_parse_allocs(char *allocations[], int len) {
    for (int i = 0; i < len; ++i) {
        if (allocations[i] != NULL) {
            free(allocations[i]);
        }
    }
}




