// File: http_response.h    
// Created June 9, 2014
// Michael Baptist - mbaptist@ucsc.edu

#ifndef __HTTP_RESPONSE_H__
#define __HTTP_RESPONSE_H__

//#define NDEBUG NoDebug

/**
 * @file http_response.h 
 * This file contains the http header and body response functions.
 */

/**
 * This function will return a formatted http response for the given status. Must be freed after use due to allocated on the heap.
 *
 * @param status The http status to respond to a request with.
 *
 * @return Returns a formatted http response or null if not implemented.
 *
 */
char *http_response(const uint status);




#endif






