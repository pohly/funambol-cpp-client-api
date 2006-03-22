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
#include "base/util/WString.h"
#include "base/util/StringBuffer.h"

const size_t WString::npos = 0xFFFFFFFF;
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
size_t charlen = sizeof(wchar_t);
    if(charlen != 2) {
        abort("Panic: wide char size in not 2");
    }
    
#endif

WString::WString(const wchar_t* str, size_t len) {
    size = 0;
    s = 0;

    // if the given string is null, leave this null,
    // otherwise set it, even empty.
    if (str) {
        size_t slen = wcslen(str);
        size_t newlen = (len > slen) ? slen : len ; 

        if(newlen) {
            getmem(newlen);
            wcsncpy(s, str, newlen);
            s[newlen]=0;
        }
        else {  // empty string
            getmem(1);
            s[0] = 0;
        }
    }
}

WString::WString(const WString &sb) {
    size = 0;
    s = NULL;
    append(sb.c_str());
}

WString::~WString() {
    freemem();
}

WString& WString::append(const wchar_t* sNew) {
    if (sNew == NULL) {
        return *this;
    }

    unsigned long len = wcslen(sNew);

    if (len == 0) {
        return *this;
    }
    if (s) {
        getmem(wcslen(s) + len);
        wcscat(s, sNew);
    }
    else {
        getmem(len);
        wcscpy(s, sNew);
    }

    return *this;
}

WString& WString::append(unsigned long i, BOOL sign) {
    wchar_t v[12];

    if (sign) {
        // wcsprintf(v, "%ld", i);
        wsprintf(v, TEXT("%ld"), i);
    } else {
        // wcsprintf(v, "%lu", i);
        wsprintf(v, TEXT("%lu"), i);
    }

    append(v);

    return *this;
}

WString& WString::append(WString& str) {
    return append(str.getChars());
}

WString& WString::append(WString* str) {
    if (str)
        return append(str->getChars());
    else 
        return *this;
}

WString& WString::set(const wchar_t* sNew) {
    if (sNew) {
        size_t len = wcslen(sNew);
        if ( len ) {
            getmem( len );
            wcscpy(s, sNew);
        }
        else if (s) {
            s[0]=0; // just make the string empty
        }
    }
    else {
        freemem();  // release the string and set it to null
    }
    
    return *this;
}

const wchar_t* WString::getChars() const { return s; }

unsigned long WString::length() const {
    return (s) ? wcslen(s) : 0;
}

WString& WString::reset() {
    freemem();
    return *this;
}

size_t WString::find(const wchar_t *str, size_t pos) const 
{
    if (pos >= length())
        return npos;
    wchar_t *p = wcsstr(s+pos, str);
    if(!p)
        return npos;
    return (p-s);
}

/***** FIXME
size_t WString::ifind(const wchar_t *str, size_t pos) const
{
    if (pos >= length())
        return npos;
    wchar_t *ls = wcstolower(s+pos);
    wchar_t *lstr = wcstolower(str);

    wchar_t *p = wcsstr(ls, lstr);
    
    size_t ret = (p) ? p-ls : npos;

    delete [] ls;
    delete [] lstr;

    return ret;
}
*/
size_t WString::replace(const wchar_t *from, const wchar_t *to, size_t pos) 
{
	size_t ret = npos;

    if (!s)
        return npos;

    if(pos>=length())
        return npos;

    wchar_t *p = wcsstr(s+pos, from);
    if (p) {
        size_t flen = wcslen(from), tlen = wcslen(to);
        wchar_t *tail = 0;
        int ldiff = tlen - flen ;

        // reallocate if needed
        getmem(length() + ldiff);
        // check is there is a remainder after the replaced token
        if( p[flen] ) {
            tail = new wchar_t[length()];
            wcscpy(tail, p+flen);
        }
        // copy to in place of from
        wcscpy(p, to);
        // copy the remainder of old string, if there is one
        if( tail ) {
            wcscpy(p+tlen, tail);
            delete [] tail;
        }
		ret = p - s;
    }
    return ret;
}

// TODO: implement some smarter argorithm to avoid multiple reallocations
int WString::replaceAll(const wchar_t *from, const wchar_t *to, size_t pos) {
    int i=0;
    size_t next;
    for(next=replace(from, to, pos); next != npos; next=replace(from,to,next) ) {
        i++;
    }
    return i;
}

ArrayList& WString::split(ArrayList &tokens, const wchar_t *separator) const {
    tokens.clear();
    size_t seplen = wcslen(separator);
    wchar_t *base = s;
    wchar_t *p = wcsstr( base, separator );

    while( p )
    {
        WString token(base, p-base);
        tokens.add( token );
        base = p + seplen;
        p = wcsstr( base, separator );
    }
    WString token(base);
    tokens.add( token );

    return tokens;
}

WString& WString::join(ArrayList &tokens, const wchar_t *separator) {
    WString *line;
    size_t totlen = 0, seplen = wcslen(separator);
    // Calc total size
    for (line=(WString *)tokens.front();
		 line;
         line=(WString *)tokens.next() ) {
        totlen += line->length() + seplen;
    }
    // Reserve mem
    reserve(totlen);
    // Join strings
    for (line=(WString *)tokens.front();
		 line;
         line=(WString *)tokens.next() ) {
        this->append(line->c_str());
        this->append(separator);
    }
    return *this;
}

WString WString::substr(size_t pos, size_t len) const {
    if(pos > wcslen(s))
        return WString(TEXT(""));

	return (WString(s+pos, len));
}

void WString::reserve(size_t len) {
    getmem(len);
}

WString& WString::upperCase() {
    wchar_t* p = NULL;

    for(p = s; *p; p++) {
        *p=towupper(*p);
    }

    return *this;
}

WString& WString::lowerCase() {
    wchar_t* p = NULL;

    for(p = s; *p; p++) {
        *p=towlower(*p);
    }

    return *this;
}

/**
 * Perform case insensitive compare
 */
bool WString::icmp(const wchar_t *sc) const {
    return (wcsicmp(s, sc)==0);
}

ArrayElement* WString::clone() {
    return new WString(s);
}

bool WString::empty() const {
    if(!s)
        return true;
    if(!*s)
        return true;
    return false;
}

bool WString::null() const { return (s==0); }


// Member Operators
WString& WString::operator= (const wchar_t* sc)
    { return set(sc); }
WString& WString::operator= (const WString& sb)
    { return set(sb); }
WString& WString::operator= (const StringBuffer& sb) {
    wchar_t* t = NULL;
    t = toWideChar(sb.c_str());        
    WString& w = set(t); 
    delete [] t;
    return w;    
}
WString& WString::operator+= (const wchar_t* sc)
    { append(sc); return *this; }
WString& WString::operator+= (const WString& s)
    { append(s); return *this; }
bool  WString::operator== (const wchar_t* sc) const
    { return wcscmp(s, sc) == 0; }
bool  WString::operator== (const WString& sb) const
    { return wcscmp(s, sb.c_str()) == 0; }
bool  WString::operator!= (const wchar_t* sc) const
    { return !(*this == sc); }
bool  WString::operator!= (const WString& s) const
    { return !(*this == s); }


// Function operators
WString operator+(const WString& x, const wchar_t *y)
{
  WString result(x);
  result.append(y);
  return result;
}


//size_t WString_memcount = 0;

// Private allocator
void WString::getmem(size_t len)
{
    //fprintf(stderr, "GETMEM len=%ld s=%lx size=%ld\n", len, s, size);
    if(len > size) {
        // Remember the old length (0 for the null string)
        size_t oldlen = length();
        // Realloc the string (like malloc when s is null)
        s = (wchar_t *)realloc(s, (len+1) * sizeof(wchar_t) );
        //WString_memcount += (len-size);
        size = len;
        // Make sure s is terminated at the old position 
        // (malloc may fill the buffer with rubbish) 
        s[oldlen] = 0;
        //fprintf(stderr, "\t\ts=%lx size=%ld alloc=%ld\n", s, size, alloclen);
    }
}

// Private deallocator
void WString::freemem()
{
    if(s) {
        free(s); s = 0;
        //fprintf(stderr, "FREEMEM s=%lx size=%ld\n", s, size);
    }
    //WString_memcount -= size;
    size = 0;
}

