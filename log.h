// File: log.h    
// Created May 30, 2014
// Michael Baptist - mbaptist@ucsc.edu

#ifndef __LOG_H__
#define __LOG_H__

//#define NDEBUG NoDebug


/**
 * @file log.h
 * This file contains all server logging api calls.
 */

/**
 * This functions fills the buffer passed in with formatted time string.
 *
 * @param buffer A char buffer of size 64 bytes or greater.
 * @param len The length of the buffer
 *
 */
void get_current_time_formatted(char *buffer, int len);

/**
 * Writes the following fields of the client request to the log file named, proxy.log. 
 *
 * @param time The date and time of the request.
 * @param req The type of request.
 * @param version The version of http.
 * @param host The requesting hostname.
 * @param uri The uri of the request.
 * @param ip The server ip address requested. 
 * @param action The action that the proxy server took.
 * @param errors Any errors that occured.
 */
void log_request(const char* time, const char *req, const char *version, const char *host, const char *uri, const char *ip, const char *action, const char *errors); 

#endif

