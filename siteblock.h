// File: siteblock.h    
// Created May 30, 2014
// Michael Baptist - mbaptist@ucsc.edu

#ifndef __SITEBLOCK_H__
#define __SITEBLOCK_H__

//#define NDEBUG NoDebug


/**
 * @file siteblock.h
 * This file contains all server site blocking api calls.
 */

/**
 * Tells the user if the site is allowed to be visted. If no file named parental_controls.log, or no lines in that file all sites are allowed.
 *
 * @param site The site the user wishes to check permissons on.
 *
 * @return 0 if allowed or -1 if not allowed to be visited.
 *
 */
int allowed_site(const char *site);


#endif

