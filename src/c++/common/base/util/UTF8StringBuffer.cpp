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

#include <string.h>

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "base/util/UTF8StringBuffer.h"


UTF8StringBuffer::UTF8StringBuffer(const wchar_t* str) {
    s = NULL;
    if (str) {
        append(str);
    }
}

UTF8StringBuffer::~UTF8StringBuffer() {
    reset();
}

UTF8StringBuffer& UTF8StringBuffer::append(const wchar_t* sNew) {
    if (sNew == NULL) {
        return *this;
    }

    unsigned long len = utf8len(sNew);

    if (len == 0) {
        return *this;
    }

    unsigned long l = length();
    char* tmp = new char[l + len + 1];

    if (tmp) {
        if (s != NULL) {
            strcpy(tmp, s);
            delete s; s = NULL;
        } else {
            strcpy(tmp, "");
        }
        wc2utf8(sNew, &tmp[l], len);
        s = tmp;
    }

    return *this;
}


UTF8StringBuffer& UTF8StringBuffer::append(StringBuffer& str) {
    return append(str.getChars());
}

UTF8StringBuffer& UTF8StringBuffer::append(UTF8StringBuffer& str) {
    unsigned long l = str.length();

    if (l == 0) {
        return *this;
    }

    unsigned long len = length();

    char* tmp = new char[l + len + 1];
    if (tmp) {
        if (s != NULL) {
            strcpy(tmp, s);
            delete s; s = NULL;
        } else {
            strcpy(tmp, "");
        }
        strcat(tmp, str.getChars());
        s = tmp;
    }

    return *this;
}

UTF8StringBuffer& UTF8StringBuffer::set(const wchar_t* sNew) {
    reset();
    if (sNew) {
        long l = utf8len(sNew);
        s = (char*)new char[l+1];
        wc2utf8(sNew, s, l);
    }

    return *this;
}

const char* UTF8StringBuffer::getChars() {
    return s;
}

unsigned long UTF8StringBuffer::length() {
    return (s) ? strlen(s) : 0;
}

UTF8StringBuffer& UTF8StringBuffer::reset() {
    if (s) {
        delete [] s; s = NULL;
    }

    return *this;
}

ArrayElement* UTF8StringBuffer::clone() {
    return new UTF8StringBuffer(utf82wc(s));
}
