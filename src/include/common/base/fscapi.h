/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
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
/** @cond DEV */

    #ifdef MAC
        #ifndef MSG_NOSIGNAL
            #define MSG_NOSIGNAL SO_NOSIGPIPE
        #endif
    #endif
    
    #ifdef POSIX
        #include "base/posixadapter.h"
    #endif
    #ifdef HAVE_STDARG_H
        #include <stdarg.h>
    #endif
    #ifdef SYMBIAN
        #include "base/symbianadapter.h"
    #endif

    #include "base/errors.h"

    #if defined(_WIN32_WCE) || defined(WIN32)
        // Windows common stuff
        #define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers

        #include <windows.h>
        #include "base/winadapter.h"
    #endif

    #if defined(WIN32)
        #include "wininet.h"
    #endif

    #if defined(WIN32) && !defined(_WIN32_WCE)
        #include <sys/stat.h>
        #include "shlobj.h"        
        #include <wchar.h>
        #include <time.h>
        #include <stdlib.h>
    #endif

    #ifdef _WIN32_WCE
        #include "base/time.h"
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

    #ifndef SYNC4J_LINEBREAK
    // default encoding of line break in native strings,
    // may be overridden by adapter header files above
    # define SYNC4J_LINEBREAK TEXT("\r\n")
    #endif

    #ifndef WCHAR_PRINTF
    /** use in format string like this: printf( "str '%" WCHAR_PRINTF "'", (WCHAR *)foo) */
    # define WCHAR_PRINTF "ls"
    #endif

    #if !defined(PLATFORM_VA_LIST)
    #define PLATFORM_VA_LIST va_list
    #endif

    #if !defined(PLATFORM_VA_START)
    #define PLATFORM_VA_START va_start
    #endif

    #if !defined(PLATFORM_VA_END)
    #define PLATFORM_VA_END  va_end
    #endif

    #if !defined(PLATFORM_VA_COPY)
    #define PLATFORM_VA_COPY va_copy
    #endif


    /**
     * All platforms are expected to have assert.h and provide
     * assert() in it. However, controlling whether assertions are
     * enabled or not depends on the specific platform.
     *
     * On Windows, the Visual Studio project file enables assertions
     * in debug builds and disables them in release builds.
     *
     * On systems using the autotools build, the --enable-assert
     * option must be used to enable assertions. To be compatible with
     * previous revisions they are disabled by default.
     *
     * @warning Source files should always include assert.h via
     * fscapi.h so that the platform specific code above has a chance
     * to control assertions.
     */
    #include <assert.h>

/** @endcond */
#endif


