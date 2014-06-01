// File: siteblock.c    
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

#include "siteblock.h"
#include "utils.h"

#define SUCCESS    0
#define FAILURE    1


int allowed_site(const char *site) {
    FILE *file = fopen("parental_controls.log", "r");
    int result = 0;
    if (file) {
        // check this list for allowed servers.
        char buf[64];
        bzero(buf, sizeof(buf));
        char *line = fgets(buf, sizeof(buf), file);
        while (line != NULL) {
            int len1 = strlen(buf);
            int len2 = strlen(site);
            int len = len1 > len2 ? len2 : len1;
            if (strncmp(site, buf, len) == 0) {
               result = 0;
               break;
            } else {
               result = -1;
            }
            line = fgets(buf, sizeof(buf), file);
        }
        fclose(file);
    }
    return result; // if no lines in parental controls file returns 0;
}




