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


#include "base/Log.h"
#include "base/util/utils.h"

#ifdef __DEBUG__

    //
    // This is required since in debug mode, new is rewritten
    // as new(__FILE__, __LINE__). See utils.h for details
    //
    #undef new

    #include <stddef.h>

    void *operator new(size_t s, char* file, int line) {
        WCHAR m[256];

        MemHandle h = MemHandleNew(s);

        void* p = MemHandleLock(h);

        // /** WARNING: this sloooooowwwwssss doooowwwwnnnn things
        wcsprintf(m, "%s:%d new - s:%ld, p:%lx\n", file, line, s, p);
        DbgMessage(m);
        // **/

        return p;
    }

    void *operator new(size_t s) {
        return ::operator new(s, "", 0);
    }

    void *operator new[](size_t s) {
        return ::operator new(s, "", 0);
    }

    void *operator new[](size_t s, char* file, int line) {
        return ::operator new(s, file, line);
    }

    void operator delete(void* p) {
        WCHAR m[216];

        // /** WARNING: this sloooooowwwwssss doooowwwwnnnn things
        wcsprintf(m, "delete - p:%lx\n", (long)p);
        DbgMessage(m);
        // **/

        MemHandle h = MemPtrRecoverHandle(p);

        if (h) {
            MemHandleUnlock(h);
            MemHandleFree(h);
        }
    }

    void operator delete[] (void* p) {
        ::operator delete(p);
    }

#endif

long utf8len(const WCHAR* s) {
    if ((s == NULL) || (wcslen(s) == 0)) {
        return 0;
    }

    Err err = errNone;
    UInt16 lenSrc = wcslen(s);
    UInt16 lenDst = 0;
    UInt32 encoding;

    FtrGet(sysFtrCreator, sysFtrNumEncoding, &encoding);

    err = TxtConvertEncoding(
              true,
              NULL,
              (Char*)s,
              &lenSrc,
              CHAR_ENCODING_VALUE(encoding),

              NULL,  // just return the size
              &lenDst,
              charEncodingUTF8,
              "?",
              1
          );

    return (err == errNone) ? (long)lenDst : -1;
}

char* wc2utf8(const WCHAR* s, char* d = NULL, unsigned long dsize = 0) {
    //
    // First of all, if s is NULL, just return NULL.
    // Then, if d is NULL, let's allocate the required memory to contain the
    // utf8 string.
    //
    if (s == NULL) {
        return NULL;
    }

    if (d == NULL) {
        dsize = utf8len(s);
        d = new char[dsize+1];
    }

    //
    // Let's convert!
    //
    Err err = errNone;
    UInt16 lenSrc = wcslen(s);
    UInt16 lenDst = (UInt16)dsize+1; // lets' include \0
    UInt32 encoding;

    FtrGet(sysFtrCreator, sysFtrNumEncoding, &encoding);

    err = TxtConvertEncoding(
              true,
              NULL,
              (Char*)s,
              &lenSrc,
              CHAR_ENCODING_VALUE(encoding),
              d,
              &lenDst,
              charEncodingUTF8,
              "?",
              1
          );



    d[dsize] = 0;

    return (err == errNone) ? d : NULL;
}

WCHAR* utf82wc(const char* s, WCHAR* d = NULL, unsigned long dsize = 0) {
    //
    // First of all, if s is NULL, just return NULL.
    // Then, if d is NULL, let's allocate the required memory to contain the
    // WCHAR string.
    //
    if (s == NULL) {
        return NULL;
    }

    if (d == NULL) {
        dsize = strlen(s);
        d = new WCHAR[dsize+1];
    }

    //
    // Let's convert!
    //
    Err err = errNone;
    UInt16 lenSrc = dsize;
    UInt16 lenDst = (dsize+1)*sizeof(WCHAR); // lets' include \0
    UInt32 encoding;

    FtrGet(sysFtrCreator, sysFtrNumEncoding, &encoding);

    err = TxtConvertEncoding(
              true,
              NULL,
              (Char*)s,
              &lenSrc,
              charEncodingUTF8,
              d,
              &lenDst,
              CHAR_ENCODING_VALUE(encoding),
              "?",
              1
          );

    d[dsize] = 0;

    return (err == errNone) ? d : NULL;
}
