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

#ifndef INCL_AUTOTOOLS_ADAPTER
#define INCL_AUTOTOOLS_ADAPTER


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

//#ifdef ENABLE_NLS
//# include <libintl.h>
//# define TEXT(String) gettext (String)
//#else
//# define TEXT(String) (String)
//# endif
#define TEXT(_x) _x

#define EXTRA_SECTION_00
#define EXTRA_SECTION_01
#define EXTRA_SECTION_02
#define EXTRA_SECTION_03
#define EXTRA_SECTION_04
#define EXTRA_SECTION_05
#define EXTRA_SECTION_06

#define BOOL int
#define TRUE 1
#define FALSE 0

#define SYNC4J_LINEBREAK "\n"

/* map WCHAR and its functions back to standard functions */
#undef WCHAR
#define WCHAR char
#define WCHAR_PRINTF "s"

#define wsprintf sprintf
#define _wfopen fopen
#define wprintf printf
#define fwprintf fprintf
#define wsprintf sprintf
#define swprintf sprintf
#define wcscpy strcpy
#define wcsncpy strncpy
#define wcsncmp strncmp
#define wcslen strlen
#define wcstol strtol
#define wcstoul strtoul
#define wcsstr strstr
#define wcscmp strcmp
#define wcstok strtok
inline char towlower(char x) { return tolower(x); }
inline char towupper(char x) { return toupper(x); }
#define wmemmove memmove
#define wmemcpy memcpy
#define wmemcmp memcmp
#define wmemset memset
#define wcschr strchr
#define wcsrchr strrchr
#define wcscat strcat
#define wcsncat strncat
#define _wtoi atoi
#define wcstod strtod
#define wcsicmp strcasecmp
#define _wcsicmp strcasecmp
#define _stricmp strcasecmp

/* some of the code compares NULL against integers, which
   fails if NULL is defined as (void *)0 */
#undef NULL
#define NULL 0

#define min(x,y) ( (x) < (y) ? (x) : (y) )
#define max(x,y) ( (x) > (y) ? (x) : (y) )

#endif

