#ifndef SYMBIANADAPTER_H_
#define SYMBIANADAPTER_H_

// We have a name clash as we define Mem and Symbian defines a Mem class too.
// We do not use Symbian Mem class directly, therefore we play dirty and
// rename symbian class into Symbian Mem. Clean solution would be to use
// namespaces.
#define Mem SymbianMem

#include <e32def.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <utf.h>
#include <ctype.h>

#undef Mem


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
size_t snwprintf(WCHAR *v, size_t size, const WCHAR* format, unsigned long value);
WCHAR *wcschr(const WCHAR *ws, WCHAR wc);
WCHAR *wcsstr(WCHAR *ws1, WCHAR *ws2);
WCHAR *wcstok(WCHAR *ws1, const WCHAR *ws2);
WCHAR *wcsncat(WCHAR *ws1, const WCHAR *ws2, size_t n);
double wcstod(const WCHAR *nptr, WCHAR ** endptr);
int _wtoi(const WCHAR *str);

/* Symbian does not ship a stdint.h interface (unless you use PIPS or OpenC)
 * We define the types used in the library, but at the moment we are not
 * standard complaint as we do not define all the necessary macros. This needs
 * to be done
 **/
typedef TInt8   int8_t;
typedef TUint8  uint8_t;
typedef TInt16  int16_t;
typedef TUint16 uint16_t;
typedef TInt32  int32_t;
typedef TUint32 uint32_t;

#endif /*SYMBIANADAPTER_H_*/
