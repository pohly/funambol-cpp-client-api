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

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"

const size_t StringBuffer::npos = 0xFFFFFFFF;
static size_t growup = 5;

#if 0
/// FIXME: Debug code
#include <stdio.h>
void abort(const char *msg)
{
    FILE *f=fopen("\fatal.txt", "w");
    fputs(msg, f);
    fclose(f);
    exit(1);
}
size_t charlen = sizeof(BCHAR);
    if(charlen != 2) {
        abort("Panic: wide char size in not 2");
    }
    
#endif

StringBuffer::StringBuffer(const BCHAR* str, size_t len) {
    size = 0;
    s = 0;

    // if the given string is null, leave this null,
    // otherwise set it, even empty.
    if (str) {
        size_t slen = bstrlen(str);
        size_t newlen = (len > slen) ? slen : len ; 

        if(newlen) {
            getmem(newlen);
            bstrncpy(s, str, newlen);
            s[newlen]=0;
        }
        else {  // empty string
            getmem(1);
            s[0] = 0;
        }
    }
}

StringBuffer::StringBuffer(const StringBuffer &sb) {
    size = 0;
    s = NULL;
    append(sb.c_str());
}

StringBuffer::~StringBuffer() {
    freemem();
}

StringBuffer& StringBuffer::append(const BCHAR* sNew) {
    if (sNew == NULL) {
        return *this;
    }

    unsigned long len = bstrlen(sNew);

    if (len == 0) {
        return *this;
    }
    if (s) {
        getmem(bstrlen(s) + len);
        bstrcat(s, sNew);
    }
    else {
        getmem(len);
        bstrcpy(s, sNew);
    }

    return *this;
}

StringBuffer& StringBuffer::append(unsigned long i, BOOL sign) {
    BCHAR v[12];

    if (sign) {
        // bsprintf(v, "%ld", i);
        bsprintf(v, T("%ld"), i);
    } else {
        // bsprintf(v, "%lu", i);
        bsprintf(v, T("%lu"), i);
    }

    append(v);

    return *this;
}

StringBuffer& StringBuffer::append(StringBuffer& str) {
    return append(str.getChars());
}

StringBuffer& StringBuffer::append(StringBuffer* str) {
    if (str)
        return append(str->getChars());
    else 
        return *this;
}

StringBuffer& StringBuffer::set(const BCHAR* sNew) {
    if (sNew) {
        size_t len = bstrlen(sNew);
        if ( len ) {
            getmem( len );
            bstrcpy(s, sNew);
        }
        else if (s) {
            s[0]=0; // just make the string empty
        } else {
            getmem( 2 );
            bstrcpy(s, "");
        }
    }
    else {
        freemem();  // release the string and set it to null
    }
    
    return *this;
}

const BCHAR* StringBuffer::getChars() const { return s; }

unsigned long StringBuffer::length() const {
    return (s) ? bstrlen(s) : 0;
}

StringBuffer& StringBuffer::reset() {
    freemem();
    return *this;
}

size_t StringBuffer::find(const BCHAR *str, size_t pos) const 
{
    if (pos >= length())
        return npos;
    BCHAR *p = bstrstr(s+pos, str);
    if(!p)
        return npos;
    return (p-s);
}

size_t StringBuffer::ifind(const BCHAR *str, size_t pos) const
{
    if (pos >= length())
        return npos;
    BCHAR *ls = strtolower(s+pos);
    BCHAR *lstr = strtolower(str);

    BCHAR *p = bstrstr(ls, lstr);
    
    size_t ret = (p) ? p-ls : npos;

    delete [] ls;
    delete [] lstr;

    return ret;
}

size_t StringBuffer::rfind(const BCHAR *str, size_t pos) const 
{
    if (pos >= length())
        return npos;
    const BCHAR *p = brfind(s+pos, str);
    if(!p)
        return npos;
    return (p-s);
}


size_t StringBuffer::replace(const BCHAR *from, const BCHAR *to, size_t pos) 
{
	size_t ret = npos;

    if (!s)
        return npos;

    if(pos>=length())
        return npos;

    BCHAR *p = bstrstr(s+pos, from);
    if (p) {
        size_t flen = bstrlen(from), tlen = bstrlen(to);
        BCHAR *tail = 0;
        int ldiff = tlen - flen ;

        // reallocate if needed
        getmem(length() + ldiff);
        // check is there is a remainder after the replaced token
        if( p[flen] ) {
            tail = new BCHAR[length()];
            bstrcpy(tail, p+flen);
        }
        // copy to in place of from
        bstrcpy(p, to);
        // copy the remainder of old string, if there is one
        if( tail ) {
            bstrcpy(p+tlen, tail);
            delete [] tail;
        }
		ret = p - s;
    }
    return ret;
}

// TODO: implement some smarter argorithm to avoid multiple reallocations
int StringBuffer::replaceAll(const BCHAR *from, const BCHAR *to, size_t pos) {
    int i=0;
    size_t next;
    for(next=replace(from, to, pos); next != npos; next=replace(from,to,next) ) {
        i++;
    }
    return i;
}

ArrayList& StringBuffer::split(ArrayList &tokens, const BCHAR *separator) const {
    tokens.clear();
    size_t seplen = bstrlen(separator);
    BCHAR *base = s;
    BCHAR *p = bstrstr( base, separator );

    while( p )
    {
        StringBuffer token(base, p-base);
        tokens.add( token );
        base = p + seplen;
        p = bstrstr( base, separator );
    }
    StringBuffer token(base);
    tokens.add( token );

    return tokens;
}

StringBuffer& StringBuffer::join(ArrayList &tokens, const BCHAR *separator) {
    StringBuffer *line;
    size_t totlen = 0, seplen = bstrlen(separator);
    // Calc total size
    for (line=(StringBuffer *)tokens.front();
		 line;
         line=(StringBuffer *)tokens.next() ) {
        totlen += line->length() + seplen;
    }
    // Reserve mem
    reserve(totlen);
    // Join strings
    for (line=(StringBuffer *)tokens.front();
		 line;
         line=(StringBuffer *)tokens.next() ) {
        this->append(line->c_str());
        this->append(separator);
    }
    return *this;
}

StringBuffer StringBuffer::substr(size_t pos, size_t len) const {
    if(pos > bstrlen(s))
        return StringBuffer(T(""));

	return (StringBuffer(s+pos, len));
}

void StringBuffer::reserve(size_t len) {
    getmem(len);
}

StringBuffer& StringBuffer::upperCase() {
    BCHAR* p = NULL;

    for(p = s; *p; p++) {
        *p=toupper(*p);
    }

    return *this;
}

StringBuffer& StringBuffer::lowerCase() {
    BCHAR* p = NULL;

    for(p = s; *p; p++) {
        *p=tolower(*p);
    }

    return *this;
}

/**
 * Perform case insensitive compare
 */
bool StringBuffer::icmp(const BCHAR *sc) const {
    return wcscmpIgnoreCase(s, sc);
}

ArrayElement* StringBuffer::clone() {
    return new StringBuffer(s);
}

bool StringBuffer::empty() const {
    if(!s)
        return true;
    if(!*s)
        return true;
    return false;
}

bool StringBuffer::null() const { return (s==0); }


// Member Operators
StringBuffer& StringBuffer::operator= (const BCHAR* sc)
    { return set(sc); }
StringBuffer& StringBuffer::operator= (const StringBuffer& sb)
    { return set(sb); }
StringBuffer& StringBuffer::operator+= (const BCHAR* sc)
    { append(sc); return *this; }
StringBuffer& StringBuffer::operator+= (const StringBuffer& s)
    { append(s); return *this; }
bool  StringBuffer::operator== (const BCHAR* sc) const
    { return bstrcmp(s, sc) == 0; }
bool  StringBuffer::operator== (const StringBuffer& sb) const
    { return bstrcmp(s, sb.c_str()) == 0; }
bool  StringBuffer::operator!= (const BCHAR* sc) const
    { return !(*this == sc); }
bool  StringBuffer::operator!= (const StringBuffer& s) const
    { return !(*this == s); }


// Function operators
StringBuffer operator+(const StringBuffer& x, const BCHAR *y)
{
  StringBuffer result(x);
  result.append(y);
  return result;
}


//size_t StringBuffer_memcount = 0;

// Private allocator
void StringBuffer::getmem(size_t len)
{
    //fprintf(stderr, "GETMEM len=%ld s=%lx size=%ld\n", len, s, size);
    if(len > size) {
        // Remember the old length (0 for the null string)
        size_t oldlen = length();
        // Realloc the string (like malloc when s is null)
        s = (BCHAR *)realloc(s, (len+1) * sizeof(BCHAR) );
        //StringBuffer_memcount += (len-size);
        size = len;
        // Make sure s is terminated at the old position 
        // (malloc may fill the buffer with rubbish) 
        s[oldlen] = 0;
        //fprintf(stderr, "\t\ts=%lx size=%ld alloc=%ld\n", s, size, alloclen);
    }
}

// Private deallocator
void StringBuffer::freemem()
{
    if(s) {
        free(s); s = 0;
        //fprintf(stderr, "FREEMEM s=%lx size=%ld\n", s, size);
    }
    //StringBuffer_memcount -= size;
    size = 0;
}

