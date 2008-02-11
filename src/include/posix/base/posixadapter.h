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


#ifndef INCL_AUTOTOOLS_ADAPTER
#define INCL_AUTOTOOLS_ADAPTER
/** @cond DEV */


/*
 * POSIX environment, configured and compiled with automake/autoconf
 */

#include <config.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>

// For ntoh functions
#include <netinet/in.h>

// Workaround for wchar defines below: unit testing may depend
// on standard header files. Include its header file first.
#include <base/test.h>

// Cygwin version of gcc does have these builtin
#ifndef __CYGWIN__
# define __declspec(x)
# define __cdecl
#endif

// Enable the fix for encoding when building for posix
#define VOCL_ENCODING_FIX


#ifdef USE_WCHAR

#undef WCHAR
#include <wchar.h>
#include <wctype.h>

#define WCHAR wchar_t
#define WCHAR_PRINTF "s"
#define TEXT(_x) L##_x

// FIXME: remove this and adapt VOCL.
WCHAR *wcstok(WCHAR *s, const WCHAR *delim);

inline int _wtoi(const WCHAR *s) { return (int)wcstol(s, NULL, 10); }

#define _wcsicmp wcscasecmp
#define wcsicmp wcscasecmp
#define snwprintf swprintf

#else

/* map WCHAR and its functions back to standard functions */
#       undef WCHAR
#       define WCHAR char
#       define WCHAR_PRINTF "s"
#       define TEXT(_x) _x

#       define SYNC4J_LINEBREAK "\n"

#       define wsprintf sprintf
#       define _wfopen fopen
#       define wprintf printf
#       define fwprintf fprintf
#       define wsprintf sprintf
#       define swprintf snprintf
#       define snwprintf snprintf
#       define wcscpy strcpy
#       define wcsncpy strncpy
#       define wcsncmp strncmp
#       define wcslen strlen
#       define wcstol strtol
#       define wcstoul strtoul
#       define wcsstr strstr
#       define wcscmp strcmp
#       define wcstok strtok
        inline char towlower(char x) { return tolower(x); }
        inline char towupper(char x) { return toupper(x); }
#       define wmemmove memmove
#       define wmemcpy memcpy
#       define wmemcmp memcmp
#       define wmemset memset
#       define wcschr strchr
#       define wcsrchr strrchr
#       define wcscat strcat
#       define wcsncat strncat
#       define _wtoi atoi
#       define wcstod strtod
#       define wcsicmp strcasecmp
#       define _wcsicmp strcasecmp
#       define _stricmp strcasecmp

#endif

/* some of the code compares NULL against integers, which
   fails if NULL is defined as (void *)0 */
#undef NULL
#define NULL 0

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

/** @endcond */
#endif

