#include <e32cmn.h>
#include <e32des8.h>
#include <e32def.h>
#include <e32std.h>

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/util/symbianUtils.h"

wchar_t * wcsncpy (wchar_t *dst, const wchar_t *src, size_t count) {
    memcpy(dst, src, count * sizeof(wchar_t));
    return dst;
}

int towlower(int c) {
    wchar_t tmpStr[1];
    tmpStr[0] = (wchar_t)c;
    TBuf16<1> buf = (const TUint16*)tmpStr;
    buf.LowerCase();
    const TUint16* resStr = buf.Ptr();
    return (int)*resStr;
}

int towupper(int c)  {
    wchar_t tmpStr[1];
    tmpStr[0] = (wchar_t)c;
    TBuf16<1> buf = (const TUint16*)tmpStr;
    buf.UpperCase();
    const TUint16* resStr = buf.Ptr();
    return (int)*resStr;
}

size_t vsnprintf(char* s, size_t size, const char* format, PLATFORM_VA_LIST aq) {

    TPtrC8 formatBuf((const unsigned char*)format);
    TInt error;
    RBuf8 formattedBuf;

    TRAP(error, formattedBuf.CreateL(size));
    if (error == KErrNone) {
        TRAP(error, formattedBuf.FormatList(formatBuf, aq));
        if (error == KErrNone) {
            char* ptr = (char *) formattedBuf.Ptr();
            size_t finalSize = formattedBuf.Length();
            if (finalSize < size) {
                memcpy(s, ptr, finalSize);
                s[finalSize] = 0;           // Symbian descriptors don't have the trailing null char
                return finalSize;
            } else {
                // In this case we truncate. We signal this by returning -1
                memcpy(s, ptr, size);
                return (size_t)-1;
            }
        }
    }
    // We cannot format the string. Return -1.
    return (size_t)-1;
}


// TODO: convert to the specified encoding, assuming wc is UTF-8
char* toMultibyte(const WCHAR *wc, const char *encoding)
{
#ifdef USE_WCHAR
    size_t length = wcstombs(NULL, wc, 0) + 1;
    if(length == (size_t)-1) {
        //LOG.error("toMultibyte: invalid string.");
        return strdup("");
    }
    char* ret = new char[length];
    wcstombs(ret, wc, length);

    return ret;
#else
    return stringdup(wc);
#endif
}

// TODO: convert to UTF-8 from the specified encoding
WCHAR* toWideChar(const char *mb, const char *encoding)
{
#ifdef USE_WCHAR
    size_t length = mbstowcs(NULL, mb, 0) + 1;
    if(length == (size_t)-1) {
        //LOG.error("toWideChar: invalid string.");
        return wstrdup(TEXT(""));
    }
    WCHAR* ret = new WCHAR[length];
    mbstowcs(ret, mb, length);

    return ret;
#else
    return stringdup(mb);
#endif
}


size_t snwprintf(WCHAR *v, size_t size, const WCHAR* format, unsigned long value) {

    TPtrC16 formatBuf((const TUint16*)format);
    TInt error;
    RBuf16 formattedBuf;

    TRAP(error, formattedBuf.CreateL(size));
    if (error == KErrNone) {
        TRAP(error, formattedBuf.Format(formatBuf, value));
        if (error == KErrNone) {
            WCHAR* ptr = (WCHAR *) formattedBuf.Ptr();
            size_t finalSize = formattedBuf.Length() * sizeof(WCHAR);
            if (finalSize < size) {
                memcpy(v, ptr, finalSize);
                v[finalSize] = 0;   // Symbian descriptors don't have the trailing null char
                return finalSize;
            } else {
                // In this case we truncate. We signal this by returning -1
                memcpy(v, ptr, size);
                v[size] = 0;   // Symbian descriptors don't have the trailing null char
                return (size_t)-1;
            }
        }
    }
    // We cannot format the string. Return -1.
    return (size_t)-1;
}


WCHAR *wcschr(const WCHAR *ws, WCHAR wc) {
    return NULL;
}

WCHAR *wcsstr(WCHAR *ws1, WCHAR *ws2) {
    return NULL;
}

WCHAR *wcstok(WCHAR *ws1, const WCHAR *ws2) {
    return NULL;
}

WCHAR *wcsncat(WCHAR *ws1, const WCHAR *ws2, size_t n) {
    return NULL;
}

double wcstod(const WCHAR *nptr, WCHAR **endptr) {
    return 0.0;
}

int _wtoi(const WCHAR *str) {
    return 0;
}

