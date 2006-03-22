/*
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCL_BASE_UTILS
#define INCL_BASE_UTILS

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "base/md5.h"
#include "base/base64.h"

// Default len for stringdup (means: use source string len)
#define STRINGDUP_NOLEN 0xFFFFFFFF

#define B64_ENCODING        T("b64")
#define TEXT_PLAIN_ENCODING T("text/plain")

/*
 * Deletes the given BCHAR*[] buffer if it is not NULL
 * and sets the pointer to NULL
 *
 */
void safeDelete(BCHAR* p[]) EXTRA_SECTION_00;

void safeDel(BCHAR** p) EXTRA_SECTION_00;

void timestampToAnchor(unsigned long timestamp, BCHAR* anchor) EXTRA_SECTION_00;

char* stringdup(const char* s, size_t len = STRINGDUP_NOLEN) EXTRA_SECTION_00 ;
wchar_t* wstrdup(const wchar_t* s, size_t len = STRINGDUP_NOLEN) EXTRA_SECTION_00;

BCHAR* strtolower(const BCHAR *s) EXTRA_SECTION_00;

BCHAR* wcstoupper(const BCHAR *s) EXTRA_SECTION_00;

/**
 * Returns TRUE is the given string is NULL or zero-length
 */
inline BOOL isEmpty(const BCHAR* s) {
    return ((s == NULL) || (bstrlen(s) == 0));
}

/**
 * Returns TRUE is the given string is NULL or zero-length
 */
inline BOOL isNotEmpty(const BCHAR* s) {
    return (s && (bstrlen(s) > 0));
}

/**
 * Returns the number of bytes required to store the UTF-8 string
 * corresponding to the given BCHAR string. If the string could not be
 * converted, -1 is returned.
 *
 * @param s the original string
 */
long int utf8len(const BCHAR* s) EXTRA_SECTION_00;

/**
 * Converts the given wchar string in UTF-8. The real implementation
 * is platform specific and should go in <platform>adapter.cpp.
 * It is supposed that the destination buffer is either big enough to
 * contain the converted data or NULL; in the latter case the needed
 * space is allocated using the new C++ operator and must be discarded
 * by the caller with the delete operator.
 * This function return the pointer to the destination buffer (d or the new
 * allocated one) in case of success or NULL in case the conversion was not
 * possible.
 *
 * @param s source string
 * @param d destination buffer or NULL
 * @param dsize size of the destination buffer (ignored if d is null)
 *
 */
char* wc2utf8(const BCHAR* s, char* d = NULL, unsigned long dsize = 0) EXTRA_SECTION_00;

/**
 * Converts the given UTF-8 string in BCHAR. The real implementation
 * is platform specific and should go in <platform>adapter.cpp.
 * It is supposed that the destination buffer is either big enough to
 * contain the converted data or NULL; in the latter case the needed
 * space is allocated using the new C++ operator and must be discarded
 * by the caller with the delete operator.
 * This function return the pointer to the destination buffer (d or the new
 * allocated one) in case of success or NULL in case the conversion was not
 * possible.
 *
 * @param s source string
 * @param d destination buffer or NULL
 * @param dsize size of the destination buffer (ignored if d is null)
 *
 */
BCHAR* utf82wc(const char* s, BCHAR* d = NULL, unsigned long dsize = 0) EXTRA_SECTION_00;

/*
* compare two BCHAR array ignoring the case of the char
*/
bool wcscmpIgnoreCase(const BCHAR* p, const BCHAR* q);

/*
* Converts a integer into a BCHAR*
*/
BCHAR* itow(int i);

/*
* Converts a integer into a BCHAR*
*/
BCHAR* ltow(long i);

/*
* Method to create the cred data given the username, password and nonce
* It uses the calculateMD5 to calculate the MD5 using the alghoritm.
*/
BCHAR* MD5CredentialData(BCHAR* userName, BCHAR* password, BCHAR* nonce);

/*
* Calculates the digest given the token and its lenght
*/
char* calculateMD5(void* token, int len, char* wdigest);

/*
 * Return a filename composed by the system temp dir and the name given
 * in input. If the file exists, try to add a digit 0-9.
 * If this fails too, return NULL (there's must be something wrong in
 * the calling program)
 *
 * @param name - a file name, without path
 * @return - a full pathname, allocated with new[], or NULL on error
 */
BCHAR *mkTempFileName(const BCHAR *name);

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
 * @param buffer (out) - new allocated buffer with the file content
 * @param len - length of the read content
 * @param binary - if true the file has to be opened in binary mode
 *
 * @return - true if file is succesfully read
 */
bool readFile(const char* path, char **message, size_t *len, bool binary = false );

long int getLenEncoding(const BCHAR* s, BCHAR* encoding);
char *toMultibyte(const WCHAR *wc, const BCHAR *encoding = 0 );
WCHAR *toWideChar(const char *mb, const BCHAR *encoding = 0 );


// Wide Char Convert: inline function used to convert a wchar
// in char, using a static buffer to store the converted string.
//
// BEWARE: the string is deleted and re-used at each call.
inline const char *_wcc(const wchar_t *wc, const char *enc=0) {
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


#endif
