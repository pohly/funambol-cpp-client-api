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
#ifndef INCL_BASE_UTF8_STRING_BUFFER
#define INCL_BASE_UTF8_STRING_BUFFER

#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"

/**
 * This class converts all incoming strings to utf8.
 */
class UTF8StringBuffer: public ArrayElement {
    public:
        UTF8StringBuffer(const wchar_t* str = NULL) EXTRA_SECTION_00;
                
        ~UTF8StringBuffer() EXTRA_SECTION_00;

        UTF8StringBuffer& append(const wchar_t*) EXTRA_SECTION_00;

        UTF8StringBuffer& append(StringBuffer& s) EXTRA_SECTION_00;

        UTF8StringBuffer& append(UTF8StringBuffer& s) EXTRA_SECTION_00;

        UTF8StringBuffer& set(const wchar_t*) EXTRA_SECTION_00;

        UTF8StringBuffer& reset() EXTRA_SECTION_00;

        const char* getChars() EXTRA_SECTION_00;

        unsigned long length() EXTRA_SECTION_00;

        ArrayElement* clone();

    private:
        char* s;
};

#endif
