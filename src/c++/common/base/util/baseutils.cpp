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
#include "base/util/utils.h"

/*
 * Deletes the given wchar_t[] buffer if it is not NULL
 * and sets the pointer to NULL
 *
 */
void safeDelete(wchar_t* p[]) {
    if (*p) {
        delete [] *p; *p = NULL;
    }
}

void safeDel(wchar_t** p) {
    if (*p) {
        delete [] *p; *p = NULL;
    }
}

wchar_t* stringdup(const wchar_t* s, size_t len)
{
    if ( !s )
        return NULL;

    int l = (len==STRINGDUP_NOLEN)?wcslen(s):len;

    wchar_t* news = new wchar_t[l+1];

    wcsncpy(news, s, l);
    news[l]=0;

    return news;
}

wchar_t* wcstolower(const wchar_t *s)
{
    wchar_t* l = NULL;
    wchar_t* p = NULL;

    for(l = p = stringdup(s); *p; ++p) {
        *p=towlower(*p);
    }

    return l;
}

wchar_t* wcstoupper(const wchar_t *s)
{
    wchar_t* u = NULL;
    wchar_t* p = NULL;

    for(u = p = stringdup(s); *p; ++p) {
        *p=towupper(*p);
    }

    return u;
}

/**
 * @brief       Convert a UTF-8 char buffer with specified
 *              to a wchar string. Source buffer MAY not be
 *              NULL-termimated.
 *
 * @param s     source buffer
 * @param ssize source buffer lenght
 * @param d     destination buffer
 * @param dsize destination buffer lenght
 *
 * @return
 */
wchar_t* charBuf2wc(
        const char* s, unsigned long ssize,
        wchar_t* d, unsigned long dsize)
{
    wchar_t *ret=NULL;

    if (s) {
        // allocate char string and make sure it is null-terminated
        char *str = new char[ssize+1];
        strncpy(str, s, ssize);
        str[ssize]='\0';

        ret=utf82wc(str,d,dsize);
        delete [] str;
    }
    return ret;
}

/**
 * Convert an unsigned long to an anchor.
 *
 * @param timestamp the timestamo to convert into an anchor
 * @param anchor where the anchor will be written
 */
void timestampToAnchor(unsigned long timestamp, wchar_t* anchor) {
    wcsprintf(anchor, TEXT("%lu"), timestamp);
}

bool wcscmpIgnoreCase(const wchar_t* p, const wchar_t* q) {

    bool ret = false;
    if (p == NULL || q == NULL)
        return ret;

    unsigned int lenp = 0, lenq = 0;
    lenp = wcslen(p);
    lenq = wcslen(q);

    if (lenp != lenq) {
        return ret;
    }

    for (unsigned int i = 0; i < lenp; i++) {
        if ( towlower(p[i]) != towlower(q[i]))
            return ret;
    }
    ret = true;
    return ret;
}


wchar_t* itow(int i) {
    wchar_t* ret = new wchar_t[10];
    wmemset(ret, 0, 10);
    wsprintf(ret, TEXT("%i"), i);
    return ret;
}

wchar_t* ltow(long i) {
    wchar_t* ret = new wchar_t[20];
    wmemset(ret, 0, 20);
    wsprintf(ret, TEXT("%i"), i);
    return ret;
}

/*
* It implements algo for authentication with MD5 method.
* It computes digest token according with follow:
* Let H   : MD5 Function represents by calculateMD5 method
* Let B64 : Base64 encoding Function represents by encodeBase64 method
* Data: H (B64(H(username:password)):nonce)
*/

wchar_t* MD5CredentialData(wchar_t* userName, wchar_t* password, wchar_t* nonce) {

    int len = 0, lenNonce = 0, totLen = 0;

    char token       [64];
    char cnonce      [64];
    char digest      [16];
    char base64      [64];
    char base64Nonce [64];
    wchar_t wnonce   [64];
    wchar_t wtoken   [64];
    wchar_t* md5Digest = NULL;
    char ch          [3];
    char* dig = NULL;

    memset(digest,      0, 16);
    memset(base64,      0, 64);
    memset(base64Nonce, 0, 64);
    memset(cnonce,      0, 64);
    memset(token,       0, 64);
    memset(wtoken,     0,  64);
    sprintf(ch, ":");

    wsprintf(wtoken, TEXT("%s:%s"), userName
                                  , password
                                  );

    len = utf8len(wtoken);
    wc2utf8(wtoken, token, len);

    // H(username:password)
    calculateMD5((void*)token, len, digest);

    // B64(H(username:password))
    len = b64_encode((char*)base64, digest, 16);

    memset(digest, 0, 16);

    wsprintf(wnonce, TEXT("%s"), nonce);
    lenNonce = utf8len(wnonce);
    wc2utf8(wnonce, cnonce, lenNonce);

    // decode wnonce from stored base64 to bin
    lenNonce = b64_decode(cnonce, cnonce);

    memcpy(base64Nonce, base64, len);
    memcpy(&base64Nonce[len], ch, 1);
    memcpy(&base64Nonce[len+1], cnonce, lenNonce);

    totLen = len + 1 + lenNonce;

    calculateMD5(base64Nonce, totLen, digest);

    b64_encode(base64, digest, 16);

    // return value in wchar...
    md5Digest = new wchar_t[33];

    utf82wc(base64, md5Digest, 33);

    return md5Digest;

}



char* calculateMD5(void* token, int len, char* wdigest) {

    //algo for md5 digest
    char dig [18];
    md5_state_t state;
    md5_byte_t digest[16];
    int di;
    char* ret = NULL;

    md5_init  (&state);
    md5_append(&state, (const md5_byte_t *)token, len);
    md5_finish(&state, digest);
    for (di = 0; di < 16; ++di) {
        sprintf(dig + di, "%c", digest[di]);
    }
    if (wdigest == NULL) {
        ret = new char[16];
        memcpy(ret, dig, 16);
        return ret;
    } else {
        memcpy(wdigest, dig, 16);
        return NULL;
    }
}
