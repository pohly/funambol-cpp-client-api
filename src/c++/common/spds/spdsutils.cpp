
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
#include "spds/spdsutils.h"

// Base64 encoding for files (with newline)
wchar_t *uuencode(const char *msg, int len);

SyncMode syncModeCode(const wchar_t* syncMode) {

    if (wcscmp(syncMode,TEXT("slow")) == 0)
        return SYNC_SLOW;
    else if (wcscmp(syncMode,TEXT("two-way")) == 0)
        return SYNC_TWO_WAY;
    else if (wcscmp(syncMode,TEXT("one-way")) == 0)
        return SYNC_ONE_WAY_FROM_SERVER;
    else if (wcscmp(syncMode,TEXT("refresh")) == 0)
        return SYNC_REFRESH_FROM_SERVER;
    //--------- Funambol extension --------------------
    else if (wcscmp(syncMode, TEXT("addrchange")) == 0)
        return SYNC_ADDR_CHANGE_NOTIFICATION;
    return SYNC_NONE;
}


SyncItemStatus** toSyncItemStatusArray(ArrayList& items) {

    int l = items.size();

    if (l < 1) {
        return NULL;
    }

    SyncItemStatus** itemArrayStatus = new SyncItemStatus*[l];

    for (int i=0; i<l; ++i) {
        itemArrayStatus[i] = (SyncItemStatus*)((ArrayElement*)items[i])->clone();
    }

    return itemArrayStatus;
}


SyncItem** toSyncItemArray(ArrayList& items) {
    int l = items.size();

    if (l < 1) {
        return NULL;
    }

    SyncItem** itemArray = new SyncItem*[l];

    for (int i=0; i<l; ++i) {
        itemArray[i] = (SyncItem*)((ArrayElement*)items[i])->clone();
    }

    return itemArray;
}

wchar_t *uuencode(const char *msg, int len)
{
    wchar_t *ret = 0;
    int i, step=54, dlen=0;

    char *buf = new char[ len * 2 ]; // b64 is 4/3, but we have also the newlines....
    for(i=0; i<len; i+=step) {
        if(len-i < step)
            step = len-i;
        dlen += b64_encode(buf+dlen, (void *)(msg+i), step);
        buf[dlen++]='\n';
    }
    
    // Terminate the string
    buf[dlen]=0;
    // Convert it
    ret = utf82wc(buf);
    delete [] buf;
    return ret;
}

// Get a line from the char buffer msg
// line endings are discarded
static const char *getLine(const char *msg, char **line) {
    // Null message?
    if (!msg)
        return 0;
    // End of string
    if ( *msg == 0)
        return 0;

    const char *next = strpbrk(msg, "\r\n\0");
    int linelen;

    if(!next) {
        linelen = strlen(msg);
        next = msg+linelen;
    }
    else
        linelen = next-msg;

    *line= new char[linelen+1];
    strncpy(*line, msg, linelen );
    (*line)[linelen]=0;
    
    while (*next == '\r' || *next == '\n') {
        next++;
    }
    return next;
}

// This functions works for standard encoded files with new line every
// 72 characters. It does not work if the line length is not multiple of 4.
int uudecode(const wchar_t *msg, char **binmsg, size_t *binlen)
{
    // Convert the string
    char *buf = wc2utf8(msg);
    if (!buf)
        return -1;

    const char *cursor = buf;
    char *line;
    // Make room for the destination (3/4 of the original)
    int outlen = strlen(buf)/3 * 4 + 1;
    char *out = new char[outlen+1];
    memset(out, 0, outlen);
    int len = 0, nl=0;

    while( (cursor=getLine(cursor, &line)) != 0 ) {
        nl++;
        len += b64_decode(out+len, line);
        if(nl==200)
            puts("Near the end");
        delete [] line;
    }
    delete [] buf; 
    // Terminate the string
    out[len]=0;
    // Set return parameters
    *binmsg = out;
    *binlen = len;
    return 0;
}

wchar_t *loadAndConvert(const wchar_t *filename, const wchar_t *encoding)
{
    char *name = wc2utf8(filename);
    char *msg = 0;
    bool binary = true;
    size_t msglen=0;
    wchar_t *ret = 0;

    
    if(!name)
        return 0;

    if( wcscmp(encoding, TEXT("base64")) == 0 ) {
        binary = true;
    }

    // Read file
    if(!readFile(name, &msg, &msglen, binary))
        return 0;
    // Encode the file
    if( wcscmp(encoding, TEXT("base64")) == 0 ) {
        ret = uuencode(msg, msglen);
    }
    else if( wcscmp(encoding, TEXT("ISO-8859-1")) == 0 ) {
        // TODO
        ret = utf82wc(msg);
    }
    else {      // Default UTF-8
        ret = utf82wc(msg);    
    }
    // Free memory
    delete [] msg;
    delete [] name;
    return ret;
}

int convertAndSave(const wchar_t *filename,
                   const wchar_t *s,
                   const wchar_t *encoding)
{
    char *buf, *name = wc2utf8(filename);
    bool binary = true;
    size_t len;

    if(!name)
        return -1;

    // Decode the file
    if( wcscmp(encoding, TEXT("base64")) == 0 ) {
        if( uudecode(s, &buf, &len) ) {
            return -1;
        }
        binary = true;
    }
    else if( wcscmp(encoding, TEXT("ISO-8859-1")) == 0 ) {
        // TODO
        buf = wc2utf8(s);
        len = strlen(buf);        
    }
    else {      // Default UTF-8
        buf = wc2utf8(s);
        len = strlen(buf);
    }
#ifdef DEBUG
    saveFile("/allegato.txt", buf, len, binary);
#endif
    saveFile(name, buf, len, binary);
    delete [] buf;
    delete [] name;
    return 0;
}


