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
    
    /*
     * Deletes the given wchar_t*[] buffer if it is not NULL
     * and sets the pointer to NULL
     *
     */
    void safeDelete(wchar_t* p[]) EXTRA_SECTION_00;

    void safeDel(wchar_t** p) EXTRA_SECTION_00;

    void timestampToAnchor(unsigned long timestamp, wchar_t* anchor) EXTRA_SECTION_00;

    wchar_t* stringdup(const wchar_t* s, size_t len = STRINGDUP_NOLEN) EXTRA_SECTION_00 ;
    
    wchar_t* wcstolower(const wchar_t *s) EXTRA_SECTION_00;

    wchar_t* wcstoupper(const wchar_t *s) EXTRA_SECTION_00;

    /**
     * Returns TRUE is the given string is NULL or zero-length
     */
    inline BOOL isEmpty(const wchar_t* s) {
        return ((s == NULL) || (wcslen(s) == 0));
    }

    /**
     * Returns TRUE is the given string is NULL or zero-length
     */
    inline BOOL isNotEmpty(const wchar_t* s) {
        return (s && (wcslen(s) > 0));
    }

    /**
     * Returns the number of bytes required to store the UTF-8 string
     * corresponding to the given wchar_t string. If the string could not be
     * converted, -1 is returned.
     *
     * @param s the original string
     */
    long int utf8len(const wchar_t* s) EXTRA_SECTION_00;

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
    char* wc2utf8(const wchar_t* s, char* d = NULL, unsigned long dsize = 0) EXTRA_SECTION_00;

    /**
     * Converts the given UTF-8 string in WCHAR_T. The real implementation
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
    wchar_t* utf82wc(const char* s, wchar_t* d = NULL, unsigned long dsize = 0) EXTRA_SECTION_00;

    /**
     * Converts the given UTF-8 string, with the given lenght, in WCHAR_T.
     * This function just make the source string NULL-terminated, then calls
     * the system-specific utf82wc.
     *
     * @param s source buffer
     * @param s source buffer lenght
     * @param d destination buffer or NULL
     * @param dsize size of the destination buffer (ignored if d is null)
     *
     */
    wchar_t* charBuf2wc(const char* s, unsigned long ssize,
                     wchar_t* d = 0, unsigned long dsize = 0) EXTRA_SECTION_00;

    /*
    * compare two wchar_t array ignoring the case of the char
    */
    bool wcscmpIgnoreCase(const wchar_t* p, const wchar_t* q);

    /*
    * Converts a integer into a wchar_t*
    */
    wchar_t* itow(int i);

    /*
    * Converts a integer into a wchar_t*
    */
    wchar_t* ltow(long i);

    /*
    * Method to create the cred data given the username, password and nonce
    * It uses the calculateMD5 to calculate the MD5 using the alghoritm.
    */
    wchar_t* MD5CredentialData(wchar_t* userName, wchar_t* password, wchar_t* nonce);

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
    wchar_t *mkTempFileName(const wchar_t *name);

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

#endif
