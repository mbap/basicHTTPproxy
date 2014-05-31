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

#endif

