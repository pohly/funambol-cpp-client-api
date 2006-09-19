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
#include "spdm/spdmutils.h"

static const char* last(const char* s, char c) EXTRA_SECTION_02;



static const char* last(const char* s, char c) {
    if (s == NULL) {
        return NULL;
    }

    for (unsigned long i = strlen(s)-1; i>=0; --i) {
        if (s[i] == c) {
            return &s[i];
        }
    }

    return NULL;
}

int minimum (int i1, int i2) {
    return (i1<=i2) ? i1 : i2;
}

 /*
 * Extracts the node name from the node path
 *
 * @param node - the node path
 * @param name - the buffer that will contain the node name
 * @param size - buffer size
 */
void getNodeName(const char* node, char* name, int size) {
    const char* p;

    p = last(node, CHR('/'));

    if (p == NULL) {
        strncpy(name, node, size-1);
        return;
    }

    strncpy(name, p+1, size);
}

/*
 * Extracts the node context from the node path
 *
 * @param node - the node path
 * @param context - the buffer that will contain the node context
 * @param size - buffer size
 */
void getNodeConT(const char* node, char* context, int size) {
    const char* p;

    p = last(node, CHR('/'));

    if (p == NULL) {
        *context = 0;
        return;
    }

    strncpy(context, node, min((p-node), size));
}

