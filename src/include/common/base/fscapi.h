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

/**
 * IMPORTANT: make sure your compiler includes both the include
 *            paths:
 *              <...>/src/include/<platform>
 *              <...>/src/include/common
 *            (in the given sequence)
 */

#ifndef INCL_FSCAPI
    #define INCL_FSCAPI

    #ifdef AUTOTOOLS
        #include "base/posixadapter.h"
    #endif
    #ifdef HAVE_STDARG_H
        #include <stdarg.h>
    #endif

    #include "base/errors.h"

    #if defined(_WIN32_WCE) || defined(WIN32)
        // Insert your headers here
        #define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

        #include <windows.h>
    #endif
    
    #if defined(WIN32) 
        #include "wininet.h"
    #endif

    #if defined(WIN32) && !defined(_WIN32_WCE)
        #include <wchar.h>
        #include <time.h>
        #include <stdlib.h>
        #include "base/win32adapter.h" 
    #endif

    #ifdef _WIN32_WCE
        #include "base/time.h"
        //#include "base/PPC2003adapter.h" 
        #include "base/winmobileadapter.h" 
    #endif

    #if defined(__PALMOS__)
      #include "base/palmadapter.h"
    #endif

    #ifdef MALLOC_DEBUG
      #pragma warning(disable:4291)
      extern size_t dbgcounter;
      void *operator new(size_t s);
      void *operator new[](size_t s);
      void *operator new(size_t s, char* file, int line);
      void *operator new[](size_t s, char* file, int line);
      void operator delete(void* p);
      void operator delete[] (void* p);

      #define new new(__FILE__, __LINE__)
    #endif
    
    extern char lastErrorMsg[];
    extern int  lastErrorCode;

    void resetError();

    #ifndef SYNC4J_LINEBREAK
    // default encoding of line break in native strings,
    // may be overridden by adapter header files above
    # define SYNC4J_LINEBREAK TEXT("\r\n")
    #endif
    
#endif


