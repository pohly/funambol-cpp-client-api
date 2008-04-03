#ifndef SYMBIANADAPTER_H_
#define SYMBIANADAPTER_H_

#include <e32def.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <utf.h>
#include <ctype.h>


#define PLATFORM_VA_LIST    VA_LIST
#define PLATFORM_VA_START   VA_START
#define PLATFORM_VA_END     VA_END


#ifdef __WINSCW__
    #define PLATFORM_VA_COPY(a,b) (a[0] = b[0])
    #if !defined(va_copy)
    #define va_copy
    #endif
#endif

#define WCHAR       wchar_t
#define BOOL        int
#define TRUE        1
#define FALSE       0
#define __cdecl
#define USE_WCHAR   1
#define TEXT(_x)    L##_x


/* These functions miss in the Symbian libc and are implemented here */
wchar_t * wcsncpy (wchar_t *dst, const wchar_t *src, size_t count);
int towlower(int c);
int towupper(int c);
size_t vsnprintf(char* s, size_t size, const char* format, PLATFORM_VA_LIST aq);

#endif /*SYMBIANADAPTER_H_*/
