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
 * Deletes the given BCHAR[] buffer if it is not NULL
 * and sets the pointer to NULL
 *
 */
void safeDelete(BCHAR* p[]) {
    if (*p) {
        delete [] *p; *p = NULL;
    }
}

void safeDel(BCHAR** p) {
    if (*p) {
        delete [] *p; *p = NULL;
    }
}

BCHAR* stringdup(const BCHAR* s, size_t len)
{
    if ( !s )
        return NULL;

    int l = (len==STRINGDUP_NOLEN)?bstrlen(s):len;

    BCHAR* news = new BCHAR[l+1];

    strncpy(news, s, l);
    news[l]=0;

    return news;
}

wchar_t* wstrdup(const wchar_t* s, size_t len)
{
    if ( !s )
        return NULL;

    int l = (len==STRINGDUP_NOLEN)?wcslen(s):len;

    wchar_t* news = new wchar_t[l+1];

    wcsncpy(news, s, l);
    news[l]=0;

    return news;
}

BCHAR* strtolower(const BCHAR *s)
{
    BCHAR* l = NULL;
    BCHAR* p = NULL;

    for(l = p = stringdup(s); *p; ++p) {
        *p=tolower(*p);
    }
    return l;
}

BCHAR* strtoupper(const BCHAR *s)
{
    BCHAR* u = NULL;
    BCHAR* p = NULL;

    for(u = p = stringdup(s); *p; ++p) {
        *p=toupper(*p);
    }
    return u;
}


wchar_t* wcstolower(const wchar_t *s)
{
    wchar_t* l = NULL;
    wchar_t* p = NULL;

    for(l = p = wstrdup(s); *p; ++p) {
        *p=towlower(*p);
    }

    return l;
}

wchar_t* wcstoupper(const wchar_t *s)
{
    wchar_t* u = NULL;
    wchar_t* p = NULL;

    for(u = p = wstrdup(s); *p; ++p) {
        *p=towupper(*p);
    }

    return u;
}

/**
 * find a substring from the end, with optional string lenght
 */
const BCHAR *brfind(const BCHAR *s1, const BCHAR *s2, size_t len)
{
	const BCHAR *sc1, *sc2, *ps1;

    if (!s1)
        return NULL;

	if (*s2 == '\0')
		return s1;

    if(len < strlen(s1)){
        ps1 = s1 + len;
    }
    else {
	    ps1 = s1 + strlen(s1);
    }
	
	while(ps1 > s1) {
		--ps1;
        for (sc1 = ps1, sc2 = s2; *sc1 != *sc2; sc1++, sc2++) {
			if (*sc2 == '\0')
				return (ps1);
        }
	}
	return NULL;
}

#if 0
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
BCHAR* charBuf2wc(
        const char* s, unsigned long ssize,
        BCHAR* d, unsigned long dsize)
{
    BCHAR *ret=NULL;

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
#endif

/**
 * Convert an unsigned long to an anchor.
 *
 * @param timestamp the timestamo to convert into an anchor
 * @param anchor where the anchor will be written
 */
void timestampToAnchor(unsigned long timestamp, BCHAR* anchor) {
    bsprintf(anchor, T("%lu"), timestamp);
}

bool wcscmpIgnoreCase(const BCHAR* p, const BCHAR* q) {

    bool ret = false;
    if (p == NULL || q == NULL)
        return ret;

    unsigned int lenp = 0, lenq = 0;
    lenp = bstrlen(p);
    lenq = bstrlen(q);

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


BCHAR* itow(int i) {
    BCHAR* ret = new BCHAR[10];
    memset(ret, 0, 10*sizeof(BCHAR) );
    bsprintf(ret, T("%i"), i);
    return ret;
}

BCHAR* ltow(long i) {
    BCHAR* ret = new BCHAR[20];
    memset(ret, 0, 20*sizeof(BCHAR));
    bsprintf(ret, T("%i"), i);
    return ret;
}

/*
* It implements algo for authentication with MD5 method.
* It computes digest token according with follow:
* Let H   : MD5 Function represents by calculateMD5 method
* Let B64 : Base64 encoding Function represents by encodeBase64 method
* Data: H (B64(H(username:password)):nonce)
*/

BCHAR* MD5CredentialData(BCHAR* userName, BCHAR* password, BCHAR* nonce) {

    int len = 0, lenNonce = 0, totLen = 0;

    char token       [64];
    char cnonce      [64];
    char digest      [16];
    char base64      [64];
    char base64Nonce [64];
    BCHAR wnonce   [64];
    BCHAR wtoken   [64];
    BCHAR* md5Digest = NULL;
    char ch          [3];
    char* dig = NULL;

    memset(digest,      0, 16);
    memset(base64,      0, 64);
    memset(base64Nonce, 0, 64);
    memset(cnonce,      0, 64);
    memset(token,       0, 64);
    memset(wtoken,     0,  64);
    sprintf(ch, ":");

    bsprintf(wtoken, T("%s:%s"), userName
                                  , password
                                  );

    len = utf8len(wtoken);
    wc2utf8(wtoken, token, len);

    // H(username:password)
    calculateMD5((void*)token, len, digest);

    // B64(H(username:password))
    len = b64_encode((char*)base64, digest, 16);

    memset(digest, 0, 16);

    bsprintf(wnonce, T("%s"), nonce);
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
    md5Digest = new BCHAR[33];

    utf82wc(base64, md5Digest, 33);

    return md5Digest;

}



char* calculateMD5(const void* token, int len, char* wdigest) {

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
