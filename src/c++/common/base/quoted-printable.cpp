/**
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "base/fscapi.h"

static int hex2int( wchar_t x )
{
    return (x >= '0' && x <= '9') ? x - '0' :
        (x >= 'A' && x <= 'F') ? x - 'A' + 10 :
        (x >= 'a' && x <= 'f') ? x - 'a' + 10 :
        0;
}

wchar_t *qp_decode(const wchar_t *qp)
{
    const wchar_t *in;
    wchar_t *ret = new wchar_t[wcslen(qp)+1];
    wchar_t *out = ret;

    for (in = qp; *in; in++ ) {
        // Handle encoded chars
        if ( *in == '=' ) {
            if (in[1] && in[2] ) {
                // The sequence is complete, check it
                in++;   // skip the '='
                if (in[0] == '\r' && in[1] == '\n') {
                    // soft line break, ignore it
                    in ++;
                    continue;
                }
                else if ( isxdigit(in[0]) && isxdigit(in[1]) ) {
                    // Ok, we have two hex digit: decode them
                    *out = (hex2int(in[0]) << 4) | hex2int(in[1]);
                    out++;
                    in ++;
                    continue;
                }
            }
            // In all wrong cases leave the original bytes
            // (see RFC 2045). They can be incomplete sequence,
            // or a '=' followed by non hex digit.
        }
        // TODO:
        // RFC 2045 says to exclude control characters mistakenly
        // present (unencoded) in the encoded stream.
        
        // Copy other characters
        *out = *in;
        out++;

        // FIXME: check if we have now a multi-byte sequence
        // to convert into multi-byte. Assume UTF-8 at first.
    }
    *out = 0;
    
    return ret;
}

// A simple version of qp_encoding not used yet
wchar_t *qp_encode(const wchar_t *qp) {
	wchar_t QP_DIGITS[] = TEXT("0123456789ABCDEF");
	wchar_t* ret = new wchar_t[wcslen(qp)*3+1];
	int i = 0;

	const wchar_t *in;
	for (in = qp; *in; in++ ) {
		if ( (0x21 <= in[0]) & (in[0] <= 0x7e) && in[0] != '=' ) {
            ret[i] = *in;
			i++;
        }
        else {
            ret[i] = '=';
			i++;
            ret[i] = QP_DIGITS[in[0] >> 4 & 0xf];
			i++;
            ret[i] = QP_DIGITS[in[0] & 0xf];
			i++;
        }
	}

	ret[i] = '\0';

	return ret;
}

bool qp_isNeed(const wchar_t *in) {
	for(int i = 0; i < int(wcslen(in)); i++) 
		if ( (0x21 > in[i]) | (in[i] > 0x7e) || in[i] == '=' )
			return true;
	
	return false;
}

