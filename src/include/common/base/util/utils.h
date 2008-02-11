/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#ifndef INCL_BASE_UTILS
#define INCL_BASE_UTILS
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/md5.h"
#include "base/base64.h"

#include <stdio.h>

// Default len for stringdup (means: use source string len)
#define STRINGDUP_NOLEN 0xFFFFFFFF

#define B64_ENCODING        "b64"
#define TEXT_PLAIN_ENCODING "text/plain"

/*
 * Deletes the given char* [] buffer if it is not NULL
 * and sets the pointer to NULL
 *
 */
void safeDelete(char*  p[]);

void safeDel(char** p);

/**
 * Convert an unsigned long to an anchor.
 *
 * @param timestamp the timestamp to convert into an anchor
 * @param anchor where the anchor will be written (has to be at least 21 characters long)
 */
void timestampToAnchor(unsigned long timestamp, char anchor[21]);

/**
 * inverse operation for timestampToAnchor(), returns 0 if not a valid anchor
 */
unsigned long anchorToTimestamp(const char* anchor);

char* stringdup(const char* s, size_t len = STRINGDUP_NOLEN) ;
WCHAR* wstrdup(const WCHAR* s, size_t len = STRINGDUP_NOLEN);

char*  strtolower(const char *s);

char*  wcstoupper(const char *s);

/**
 * find a substring from the end, with optional string lenght
 */
const char *brfind(const char *s1, const char *s2, size_t len=STRINGDUP_NOLEN) ;

/**
 * Returns true is the given string is NULL or zero-length
 */
inline bool isEmpty(const char*  s) {
    return ((s == NULL) || (strlen(s) == 0));
}

/**
 * Returns true is the given string is NULL or zero-length
 */
inline bool isNotEmpty(const char*  s) {
    return (s && (strlen(s) > 0));
}


/*
* compare two char array ignoring the case of the char
*/
bool wcscmpIgnoreCase(const char*  p, const char*  q);

/*
* Converts a integer into a char*
*/
char*  itow(int i);

/*
* Converts a integer into a char*
*/
char*  ltow(long i);


/*
* Method to create the cred data given the username, password and nonce
* It uses the calculateMD5 to calculate the MD5 using the alghoritm.
*/
char*  MD5CredentialData(const char* userName, const char* password, const char* nonce);

/*
* Calculates the digest given the token and its lenght
*/
char* calculateMD5(const void* token, int len, char* wdigest);

/*
 * Return a filename composed by the system temp dir and the name given
 * in input. If the file exists, try to add a digit 0-9.
 * If this fails too, return NULL (there's must be something wrong in
 * the calling program)
 *
 * @param name - a file name, without path
 * @return - a full pathname, allocated with new[], or NULL on error
 */
char *mkTempFileName(const char *name);

/*
 * Write len bytes from buffer to the file 'filename'.
 *
 * @param name - the file name
 * @param buffer - pointer to the buffer to write
 * @param len - the number of bytes to write
 * @param binary - if true the file will be opened in binary mode
 *
 * @return - true if file is successfully saved
 */
bool saveFile(const char *filename, const char *buffer, size_t len,
              bool binary = false );

/*
 * Get the size of the file, in bytes
 *
 * @param f - the file to evaluate
 * @return - the length of the file
 */
size_t fgetsize(FILE *f);

/*
 * Read the content
 *
 * @param name - the file name
 * @param message (out) - new allocated buffer with the file content
 * @param len - length of the read content
 * @param binary - if true the file has to be opened in binary mode
 *
 * @return - true if file is succesfully read
 */
bool readFile(const char* name, char **message, size_t *len, bool binary = false );

/*
 * Read the content of a directory
 *
 * @param name - the dir name
 * @param count (out) - number of files in dir
 * @param onlyCount - optional, if true only set the nuber of files (count)
 *
 * @return - new allocated array of fileNames (NULL if errors)
 */
char** readDir(char* name, int *count, bool onlyCount = false);

/**
 * Returns the most recent time stamp of file content or attribute
 * modifications, 0 in case of error. The time stamp must use the same
 * time base as time().
 */
unsigned long getFileModTime(const char* name);


long int getLenEncoding(const char*  s, const char* encoding);
char *toMultibyte(const WCHAR *wc, const char *encoding = 0 );
WCHAR *toWideChar(const char *mb, const char *encoding = 0 );


// Wide Char Convert: inline function used to convert a wchar
// in char, using a static buffer to store the converted string.
//
// BEWARE: the string is deleted and re-used at each call.
inline const char *_wcc(const WCHAR *wc, const char *enc=0) {
    static char* encodeBuf = 0;

    if (encodeBuf){
        delete [] encodeBuf;
        encodeBuf = 0;
    }
    if (wc) {
        encodeBuf = toMultibyte(wc, enc);
    }
    return encodeBuf;
}


/** @endcond */
#endif
