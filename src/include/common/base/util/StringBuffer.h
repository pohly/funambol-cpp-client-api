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

#ifndef INCL_BASE_STRING_BUFFER
#define INCL_BASE_STRING_BUFFER

#include "base/util/ArrayElement.h"

class ArrayList;

/**
 * Awful implementation of a StringBuffer!
 */
class StringBuffer: public ArrayElement {
    public:
        // Constant value for an invalid pos (returned by find and rfind)
        static const size_t npos;

        StringBuffer(const char*  str = NULL, size_t len = npos) EXTRA_SECTION_00;

        StringBuffer(const StringBuffer &sb) EXTRA_SECTION_00;

        StringBuffer(const void* str, size_t len) EXTRA_SECTION_00;

        ~StringBuffer() EXTRA_SECTION_00;

        StringBuffer& append(const char* ) EXTRA_SECTION_00;

        StringBuffer& append(unsigned long u, BOOL sign = TRUE) EXTRA_SECTION_00;

        StringBuffer& append(StringBuffer& s) EXTRA_SECTION_00;

        StringBuffer& append(StringBuffer* str) EXTRA_SECTION_00;

        StringBuffer& set(const char* ) EXTRA_SECTION_00;
                
        /**
         * Release the string buffer.
         */
        StringBuffer& reset() EXTRA_SECTION_00;

        /**
         * Get the char array, same as the cast operator
         */
        const char*  getChars() const EXTRA_SECTION_00;
        inline const char*  c_str() const { return s; } EXTRA_SECTION_00;

        /**
         * Find the first occurrence of substring str, starting from pos.
         *
         * @param - start position
         * @return - the position or StringBuffer::npos if not found.
         */
        size_t find(const char *str, size_t pos = 0) const EXTRA_SECTION_00;

        /**
         * Find the first occurrence of substring str, using case insensitive compare.
         *
         * @return - the position or StringBuffer::npos if not found.
         */
        size_t ifind(const char *str, size_t pos = 0) const EXTRA_SECTION_00;

        /**
         * Find the last occurrence of substring str.
         *
         * @return - the position or StringBuffer::npos if not found.
         */
        size_t rfind(const char *str, size_t pos = 0) const EXTRA_SECTION_00;

        /**
         * Replace the first occurrence of substring 'from' with string 'to'.
         *
         * @return - the position of the first token replaced or StringBuffer::npos if
         *           not found.
         */
        size_t replace(const char *from, const char *to, size_t pos = 0) EXTRA_SECTION_00;
        /**
         * Replace all the occurrences of substring 'from' with string 'to'.
         *
         * @return - the number of tokens replaced 
         */
        int replaceAll(const char *from, const char *to, size_t pos = 0) EXTRA_SECTION_00;

        /**
         * Splits string on each occurrence of any of the characters in 
         * delimiters.
         *
         * @return - the position or StringBuffer::npos if not found.
         */
        ArrayList &split (ArrayList &tokens,
                          const char *delimiters = " ") const EXTRA_SECTION_00;

        /**
         * Joins all the tokens in the given ArrayList, using separator to
         * contatenate them, appending them to the StringBuffer
         *
         * @return - the StringBuffer
         */
        StringBuffer& join(ArrayList &tokens, const char *separator);

        /**
         * Return the substring between pos and pos+len.
         * If pos is greater then the string length, or len is 0, return an 
         * empty string
         * If len is greater then the string length, the last is used.
         */
        StringBuffer substr(size_t pos, size_t len = npos) const EXTRA_SECTION_00;

        /**
         * Return the length of the string, or zero if the internal pointer
         * is NULL.
         */
        unsigned long length() const EXTRA_SECTION_00;

        /**
         * Reserve len amount of space for the string.
         */
        void reserve(size_t len) EXTRA_SECTION_00;

        /**
         * Make the string upper case
         */
        StringBuffer& upperCase() EXTRA_SECTION_00 ;

        /**
         * Make the string lower case
         */
        StringBuffer& lowerCase() EXTRA_SECTION_00 ;

        /**
         * Perform case insensitive compare
         */
        bool icmp(const char *sc) const EXTRA_SECTION_00 ;

        /** 
         * True if the string is null or empty
         */
        bool empty() const EXTRA_SECTION_00;

        /**
         * True if the string is null
         */
        bool null() const EXTRA_SECTION_00;
        
        /**
         * Clone the string
         */
        ArrayElement* clone() EXTRA_SECTION_00 ;

        /**
         * Class operators
         */
        StringBuffer& operator= (const char*  sc) EXTRA_SECTION_00 ;
        StringBuffer& operator= (const StringBuffer& s) EXTRA_SECTION_00 ;
        StringBuffer& operator+= (const char*  sc) EXTRA_SECTION_00 ;
        StringBuffer& operator+= (const StringBuffer& s) EXTRA_SECTION_00 ;
        bool operator== (const char*  sc) const EXTRA_SECTION_00 ;
        bool operator== (const StringBuffer& sb) const EXTRA_SECTION_00 ;
        bool operator!= (const char*  sc) const EXTRA_SECTION_00 ;
        bool operator!= (const StringBuffer& s) const EXTRA_SECTION_00 ;
        
        operator const char* () const { return s; } EXTRA_SECTION_00 ;
        
    private:
        char*  s;
        size_t size;

        // Allocator
        void getmem(size_t len);
        // Deallocator
        void freemem();
};

StringBuffer operator+(const StringBuffer& x, const char *y);

#endif

