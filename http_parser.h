// File: http_parser.h    
// Created May 27, 2014
// Michael Baptist - mbaptist@ucsc.edu

#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

//#define NDEBUG NoDebug

/**
 * This will count the number of lines in an http message.
 *
 * @param buffer The buffer that is filled with the http message.
 * @param bufferlen The length of the buffer passed in.
 *
 * @return The numnber of newline characters.
 */
int count_newlines(char buffer[], int bufferlen);

#endif

