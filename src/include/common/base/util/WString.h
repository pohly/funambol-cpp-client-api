/*
 * Copyright (C) 2003-2007 Funambol
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

#ifndef INCL_WSTRING
#define INCL_WSTRING

#include "base/util/ArrayElement.h"

class ArrayList;
class StringBuffer;

#define DEFAULT_DELIMITER (const WCHAR* )TEXT(" ")

/**
 * Awful implementation of a WString!
 */
class WString: public ArrayElement {
    public:
        // Constant value for an invalid pos (returned by find and rfind)
        static const size_t npos;

        WString(const WCHAR* str = NULL, size_t len = npos) EXTRA_SECTION_00;

        WString(const WString &sb) EXTRA_SECTION_00;

        ~WString() EXTRA_SECTION_00;

        WString& append(const WCHAR*) EXTRA_SECTION_00;

        WString& append(unsigned long u, BOOL sign = TRUE) EXTRA_SECTION_00;

        WString& append(WString& s) EXTRA_SECTION_00;

        WString& append(WString* str) EXTRA_SECTION_00;

        WString& set(const WCHAR*) EXTRA_SECTION_00;

        /**
         * Release the string buffer.
         */
        WString& reset() EXTRA_SECTION_00;

        /**
         * Get the WCHAR array, same as the cast operator
         */
        const WCHAR* getChars() const EXTRA_SECTION_00;
        inline const WCHAR* c_str() const { return s; } EXTRA_SECTION_00;

        /**
         * Find the first occurrence of substring str.
         *
         * @return - the position or WString::npos if not found.
         */
        size_t find(const WCHAR *str, size_t pos = 0) const EXTRA_SECTION_00;

        /**
         * Find the first occurrence of substring str, using case insensitive compare.
         *
         * @return - the position or WString::npos if not found.
         */
        size_t ifind(const WCHAR *str, size_t pos = 0) const EXTRA_SECTION_00;

        /**
         * Replace the first occurrence of substring 'from' with string 'to'.
         *
         * @return - the position of the first token replaced or WString::npos if
         *           not found.
         */
        size_t replace(const WCHAR *from, const WCHAR *to, size_t pos = 0) EXTRA_SECTION_00;
        /**
         * Replace all the occurrences of substring 'from' with string 'to'.
         *
         * @return - the number of tokens replaced 
         */
        int replaceAll(const WCHAR *from, const WCHAR *to, size_t pos = 0) EXTRA_SECTION_00;

        /**
         * Splits string on each occurrence of any of the characters in 
         * delimiters.
         *
         * @return - the position or WString::npos if not found.
         */
        ArrayList &split (ArrayList &tokens,
                          const WCHAR *delimiters = DEFAULT_DELIMITER ) const EXTRA_SECTION_00;

        /**
         * Joins all the tokens in the given ArrayList, using separator to
         * contatenate them, appending them to the WString
         *
         * @return - the WString
         */
        WString& join(ArrayList &tokens, const WCHAR *separator);

        /**
         * Return the substring between pos and pos+len.
         * If pos is greater then the string length, or len is 0, return an 
         * empty string
         * If len is greater then the string length, the last is used.
         */
        WString substr(size_t pos, size_t len = npos) const EXTRA_SECTION_00;

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
        WString& upperCase() EXTRA_SECTION_00 ;

        /**
         * Make the string lower case
         */
        WString& lowerCase() EXTRA_SECTION_00 ;

        /**
         * Perform case insensitive compare
         */
        bool icmp(const WCHAR *sc) const EXTRA_SECTION_00 ;

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
        WString& operator= (const WCHAR* sc) EXTRA_SECTION_00 ;
        WString& operator= (const WString& s) EXTRA_SECTION_00 ;
        WString& operator= (const StringBuffer& s) EXTRA_SECTION_00 ;
        WString& operator+= (const WCHAR* sc) EXTRA_SECTION_00 ;
        WString& operator+= (const WString& s) EXTRA_SECTION_00 ;
        WString& operator+= (const StringBuffer& s) EXTRA_SECTION_00 ;
        bool operator== (const WCHAR* sc) const EXTRA_SECTION_00 ;
        bool operator== (const WString& sb) const EXTRA_SECTION_00 ;
        bool operator== (const StringBuffer& sb) const EXTRA_SECTION_00 ;
        bool operator!= (const WCHAR* sc) const EXTRA_SECTION_00 ;
        bool operator!= (const WString& s) const EXTRA_SECTION_00 ;
        bool operator!= (const StringBuffer& sb) const EXTRA_SECTION_00 ;
        
        inline operator const WCHAR*() const { return s; } EXTRA_SECTION_00 ;
        
    private:
        WCHAR* s;
        size_t size;

        // Allocator
        void getmem(size_t len);
        // Deallocator
        void freemem();
};

WString operator+(const WString& x, const WCHAR *y);

#endif

