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



#ifndef INCL_PALM_ADAPTER
#define INCL_PALM_ADAPTER

#include <unix_stdarg.h>

#include <PalmOS.h>
#include <PalmTypes.h>
#include <StringMgr.h>
#include <DebugMgr.h>

#define EXTRA_SECTION_00  __attribute__ ((section ("c00")))
#define EXTRA_SECTION_01  __attribute__ ((section ("c01")))
#define EXTRA_SECTION_02  __attribute__ ((section ("c02")))
#define EXTRA_SECTION_03  __attribute__ ((section ("c03")))
#define EXTRA_SECTION_04  __attribute__ ((section ("c04")))
#define EXTRA_SECTION_05  __attribute__ ((section ("c05")))
#define EXTRA_SECTION_06  __attribute__ ((section ("c06")))

#define TEXT(t) t
#define __declspec(ignore)
#define __cdecl
#define FALSE false
#define TRUE true
#define NO_STD_IO

#define WCHAR char

#define wcscat(s1, s2) StrCat(s1, s2)
#define wcschr(s1, c1) StrChr(s1, c1)
#define wcscmp(s1, s2) StrCompare(s1, s2)
#define wcscpy(s1, s2) StrCopy(s1, s2)
#define wcsncat(s1, s2, i1) StrNCat(s1, s2, i1)
#define wcsncmp(s1, s2, i1) StrNCompare(s1, s2, i1)
#define wcsncpy(s1, s2, i1) StrNCopy(s1, s2, i1)
#define wcsstr(s1, s2) StrStr(s1, s2)

#define towlower(c1) tolower(c1)
#define towupper(c1) toupper(c1)

#define atoi(s1) StrAToI(s1);

inline long wcstol(WCHAR* s1, WCHAR **endptr = NULL, int base = 10);
inline void memcpy(WCHAR* p1, WCHAR* p2, unsigned long i1);
inline void wcsprintf(WCHAR* s, WCHAR* format, ...);
inline void wcstolwr(WCHAR* s);
inline char toupper(char c);
inline unsigned long time(void* ignored);
inline unsigned long wcslen(const WCHAR* s);

inline long wcstol(WCHAR* s1, WCHAR **endptr = NULL, int base = 10) {
    return StrAToI(s1);
}

inline void memcpy(WCHAR* p1, WCHAR* p2, unsigned long i1) {
    for (unsigned long i=0; i<i1; ++i) {
        *p1 = *p2;
        ++p1; ++p2;
    }
}

inline void wcsprintf(WCHAR* s, WCHAR* format, ...) {
    va_list args;

    va_start( args, format );

    StrVPrintF(s, format, args);

    va_end(args);
}

inline const WCHAR* wcsrchr(const WCHAR* s, WCHAR c) {
    FrmCustomAlert(200, "wcsrchr", s, "");
    if (s == NULL) {
        return NULL;
    }

    for (unsigned long i = wcslen(s)-1; i>=0; --i) {
        if (s[i] == c) {
            return &s[i];
        }
    }

    return NULL;
}

inline void wcstolwr(WCHAR* s) {
    WCHAR dest[StrLen(s)+1];

    StrToLower(dest, s);
    StrCopy(s, dest);
}

inline char tolower(char c) {
    return ((c >= 'A') && (c <= 'Z')) ? (c+32) : c;
}

inline char toupper(char c) {
    return ((c >= 'a') && (c <= 'z')) ? (c-32) : c;
}

inline unsigned long time(void* ignored) {
    return TimGetSeconds();
}


/**
 * NOTE: we cannot use StrLen, since it just returns a UInt16!!!
 */
inline unsigned long wcslen(const WCHAR* s) {
    for (unsigned long l = 0; l<0xFFFF; ++l) {
        if (s[l] == 0) {
            return l;
        }
    }

    return 0xFFFF;
}

#endif
