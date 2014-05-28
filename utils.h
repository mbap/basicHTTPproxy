// File: utils.h    
// Created April 7, 2014
// Michael Baptist - mbaptist@ucsc.edu

#ifndef __UTILS_H__
#define __UTILS_H__

//#define NDEBUG NoDebug


/**
 * @file client_utils.h
 * My utitlies header file for checking errors for system and api function calls.
 */

/**
 * A boolean data type created by an enum.
 */
typedef enum {FALSE = 0, TRUE = 1} bool;

/**
 * Checks the return value of the socket(3) networking api call for any errors and prints messages and sets the exit status accordingly.
 *
 * @param fd The file descriptor returned by the socket(3) call.
 */
void check_socket(int fd);

/**
 * Checks the return value of the connect(3) networking api call for any errors and prints messages and sets the exit status accordingly.
 * 
 * @param val The return value from the connect(3) call.
 */
void check_connection(int val);

/**
 * Checks the current directory for the file filename. If file name is found retrieve_file will attepmt to open the file using fopen. If not an error message will be returned. fclose(3) must be called or memory leak will occur. 
 *
 * @param filename The file to be searched for and opened.
 * @param mode The mode in which the file will be opened.
 *
 * @return The file descriptor for the file if fopen succeeds. Otherwise NULL is returned if filename is not found, or if fopen fails.
 */
FILE *retrieve_file(const char *restrict filename, const char *restrict mode);

/**
 * Takes a file and gives back the size of the file. 
 *
 * @param filename The file in which  you want the size of.
 *
 * @return The size of the file filename, or -1 if an error occurs. Errno will be set to the proper error.
 */
int get_file_size(FILE *restrict filename);

/**
 * Serializes an int into a unsigned char
 * 
 * @param buffer The array to insert the data.
 * @param val The value to serialize.
 *  
 * @return A pointer to the next free space in the buffer. 
 */
unsigned char *serialize_int(unsigned char *buffer, unsigned int val);

/**
 * Serializes an char array into a unsigned char array
 * 
 * @param buffer The array to insert the data.
 * @param buf The value to serialize.
 * @param len Then length of buf.
 *  
 * @return A pointer to the next free space in the buffer. 
 */
unsigned char *serialize_data(unsigned char *buffer, char buf[], int len);


/**
 * Deserializes an int into a unsigned char
 * 
 * @param buffer The array to get the data out of.
 * @param val The value to save the data.
 *  
 * @return A pointer to the next free space in the buffer. 
 */
unsigned char *deserialize_int(unsigned char *buffer, unsigned int *val);


/**
 * Deserializes an char array into a unsigned char array
 * 
 * @param buffer The array to get the data from.
 * @param buf The buffer to save it.
 * @param len Then length of buf.
 *  
 * @return A pointer to the next free space in the buffer. 
 */
unsigned char *deserialize_data(unsigned char *buffer, char buf[], int len);

/**
 * Allows for debugging print statements to be made and easily turned off for release build
 *
 * @param format The format string to format the print statement.
 */
#ifdef NDEBUG
#define DEBUGF(...) // DEBUG (__VA_ARGS__)
#else
#define DEBUGF(...) debugprintf (__VA_ARGS__)
void debugprintf (char *format, ...);
#endif

#endif

