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
int count_newlines(char buffer[], int bufferlen) {
    int c = 0;
    for (int i = 0; i < bufferlen;  i++) {
        if (buffer[i] == '\n') c++;
    }
    return c;
}

void parse_http_header(char *lines[], char *buf, int numlines) {
    // get server ip and port from server list.
    char *copy = malloc(strlen(buf) + 1);
    strcpy(copy, buf);
    char *tok = copy;
    int i = 0;
    while ((tok = strtok(tok, "\n")) != NULL) {
        int len = strlen(tok);
        lines[i] = malloc(len + 1);
        bcopy(tok, lines[i], len);
        tok = NULL;
        if (i < numlines) {
           ++i;
        } else {
           break;
        }
    }
    free(copy);
}

void parse_http_header_line(char *pieces[], char *line, int pieceslen) {
    // get server ip and port from server list.
    char *copy = malloc(strlen(line) + 1);
    strcpy(copy, line);
    char *tok = copy;
    int i = 0;
    while ((tok = strtok(tok, " ")) != NULL) {
        int len = strlen(tok);
        if (i == (pieceslen - 1)) {
            pieces[i] = malloc(len);
        } else {
            pieces[i] = malloc(len + 1);
        }
        bcopy(tok, pieces[i], len);
        pieces[i][len] = '\0';
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




