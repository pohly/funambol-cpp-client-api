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

#include <sys/stat.h>
#include <unistd.h>

#include "base/Log.h"
#include "base/util/utils.h"


long utf8len(const char* s) {
	 return s ? wcslen(s) : 0;
}

char* wc2utf8(const char* s, char* d, unsigned long dsize) {

    //
    // First of all, if s is NULL, just return NULL.
    // Then, if d is NULL, let's allocate the required memory to contain the
    // utf8 string.
    //
    if (s == NULL) {
        return NULL;
    }

    if (d == NULL) {
        dsize = utf8len(s);
        d = new char[dsize+1];
    }

	 wcscpy( d, s );

    return d;
}

char* utf82wc(const char* s, char* d, unsigned long dsize) {

    //
    // First of all, if s is NULL, just return NULL.
    // Then, if d is NULL, let's allocate the required memory to contain the
    // WCHAR string.
    //
    if (s == NULL) {
        return NULL;
    }

    if (d == NULL) {
        // get the right lenght with a NULL dest
        dsize = strlen (s);
        d = new WCHAR[dsize+1];
    }



    wcscpy(d, s);

    return d;

}

bool saveFile(const char *filename, const char *buffer, size_t len, bool binary)
{
	const char *mode = binary ? "wb" : "w" ;

    FILE *f = fopen(filename, "w");

    if(!f)
        return false;

    if (fwrite(buffer, len, sizeof(char), f) != len) {
        fclose(f);
        return false;
    }
    fclose(f);

    return true;
}

bool readFile(const char* path, char **message, size_t *len, bool binary)
{
    FILE *f = NULL;
    size_t msglen=0;
    char *msg=0;
    const char *mode = binary ? "rb" : "r" ;
    struct stat buf;
    bool res = false;

    f = fopen(path, mode);
    if ( !f ) {
        goto done;
    }
    if ( fstat(fileno(f), &buf) ) {
        goto done;
    }

    msglen = buf.st_size;
    msg = new char[msglen+1];
    msg[msglen] = 0;

    *len=fread(msg, sizeof(char), msglen, f);
    if ( ferror(f) || *len < msglen ){
        goto done;
    }

    // Set return parameters
    *message=msg;
    msg = 0;
    res = true;

  done:
    if (f) {
        fclose(f);
    }
    if (msg) {
        delete [] msg;
    }

    return res;
}


// TBD: dummy implementation!
char** readDir(char* name, int *count, bool onlyCount) {
    return NULL;
}

// TODO: convert to the specified encoding, assuming wc is UTF-8
char* toMultibyte(const WCHAR *wc, const char *encoding)
{
    return stringdup(wc);
}

// TODO: convert to UTF-8 from the specified encoding
WCHAR* toWideChar(const char *mb, const char *encoding)
{
    return stringdup(mb);
}

// Implemented using mkstemp() with a template hard-coded
// to /tmp. Because the API of mkTempFileName() cannot return
// an open file, the file has to be closed. This exposes
// the possibility for an attacker to replace the file
// before it is opened again if /tmp has incorrect permissions.
char *mkTempFileName(const char *name)
{
    char *filename = new char[strlen("/tmp/") + strlen(name) + strlen(".XXXXXX") + 1];
    int fd;

    sprintf(filename, "/tmp/%s.XXXXXX", name );
    fd = mkstemp(filename);
    if (fd == -1) {
        delete [] filename;
        return NULL;
    } else {
        close(fd);
        return filename;
    }
}
