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


#ifndef INCL_KEY_VALUE_PAIR
#define INCL_KEY_VALUE_PAIR

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"

/**
 * This class is an ArrayElement that keeps a key-value pair.
 */
class WKeyValuePair : public ArrayElement {
    public:

        WKeyValuePair(const wchar_t* key = NULL, const wchar_t* value = NULL) EXTRA_SECTION_00;
        ~WKeyValuePair() EXTRA_SECTION_00;

        ArrayElement* clone() EXTRA_SECTION_00;

        /**
         * Sets the key. The string is duplicated so that the caller can
         * independently release its copy. If key is NULL, the older value
         * is released.
         *
         * @param key the new key
         */
        void setKey(const wchar_t* key) EXTRA_SECTION_00;

        /**
         * Sets the value. The string is duplicated so that the caller can
         * independently release its copy. If value is NULL, the older value
         * is released.
         *
         * @param value the new value
         */

        void setValue(const wchar_t* value) EXTRA_SECTION_00;

        /**
         * Returns the key (the internal buffer address is returned).
         */
        wchar_t* getKey() EXTRA_SECTION_00;

        /**
         * Returns the value (the internal buffer address is returned).
         */
        wchar_t* getValue() EXTRA_SECTION_00;

    private:
        wchar_t* k;
        wchar_t* v;
};

#endif
