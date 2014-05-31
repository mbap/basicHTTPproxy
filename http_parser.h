// File: http_parser.h    
// Created May 27, 2014
// Michael Baptist - mbaptist@ucsc.edu

#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

//#define NDEBUG NoDebug

/**
 * @file http_parser.h 
 * This file contains the http header and body parsing functions.
 */

/**
 * This will count the number of lines in an http message.
 *
 * @param buffer The buffer that is filled with the http message.
 * @param bufferlen The length of the buffer passed in.
 *
 * @return The numnber of newline characters.
 */
int count_newlines(char buffer[], int bufferlen);

/**
 * Returns a an array with the http header broken into lines for further parsing. Uses malloc so the array must free each element.
 *
 * @param buf The http header.
 * @param numlines The number of lines in the header.
 * @param lines An array to fill with lines;
 *
 */
void parse_http_header(char *lines[], char *buf, int numlines); 


/**
 * Parses one line from an http header. Mallocs a slot in an array for each token. Must be freed after use.
 *
 * @param pieces Array of char pointers that will get filled in with tokens.
 * @param line The line to tokenize.
 * @param pieceslen The length of the pieces array.
 *
 */
void parse_http_header_line(char *pieces[], char *line, int pieceslen);

/**
 * Frees memory allocations from parsing functions.
 *
 * @param allocations An array filled with memory from the heap.
 * @param len The length of the array.
 *
 */
void free_parse_allocs(char *allocations[], int len);

#endif

