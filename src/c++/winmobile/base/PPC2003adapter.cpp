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

#include <stdio.h>
#include "base/Log.h"
#include "base/util/utils.h"

long utf8len(const wchar_t* s) {
    if ((s == NULL) || (wcslen(s) == 0)) {
        return 0;
    }
    
    long k = 0;
    
    // If the function succeeds (WideCharToMultiByte) 
    // the return value is the required size, in bytes, 
    // for a buffer that can receive the translated string.

    k = WideCharToMultiByte (CP_UTF8, 0, s, wcslen(s), NULL, 0, NULL, NULL);

    return (k != 0) ? (long)k : -1;
}

char* wc2utf8(const wchar_t* s, char* d, unsigned long dsize) {
    
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
    
    unsigned long k = 0;
    
    k = WideCharToMultiByte (CP_UTF8, 0, s, wcslen(s), d, dsize, 0, 0);	
	
	d[dsize] = 0;
    
                                    
    return (k != 0) ? d : NULL;
}

wchar_t* utf82wc(const char* s, wchar_t* d, unsigned long dsize) {
              
    //
    // First of all, if s is NULL, just return NULL.
    // Then, if d is NULL, let's allocate the required memory to contain the
    // wchar_t string.
    //
    if (s == NULL) {
        return NULL;
    }

    if (d == NULL) {
        dsize = strlen(s);
        d = new wchar_t[dsize+1];
    }
   
    wmemset(d, 0, dsize + 1);

    int k = MultiByteToWideChar(CP_UTF8, 0, s, strlen(s), d, dsize + 1);
    
    return (k != 0) ? d : NULL;
    
}


/*
 * Return a filename composed by the system temp dir and the name given 
 * in input. If the file exists, try to add a digit 0-9.
 * If this fails too, return NULL (there's must be something wrong in
 * the calling program)
 *
 * @param name - a file name, without path
 * @return - a full pathname, allocated with new[], or NULL on error
 */
wchar_t *mkTempFileName(const wchar_t *name)
{
    wchar_t tmpPath[64];
    wchar_t* tmpFileName = new wchar_t[MAX_PATH];  // System constants for the path

    GetTempPath(64, tmpPath);
    int ret = GetTempFileName(tmpPath, TEXT("fun"), 0, tmpFileName);
    
    if (ret == 0) { // function GetTempFileName fails 
        delete [] tmpFileName; 
        tmpFileName = NULL;
    }

    return tmpFileName;

    /*
	wchar_t *ret = new wchar_t[wcslen(TEMPROOT)+wcslen(name)+3] ;
	wsprintf(ret, TEXT("%s\\%s"), TEMPROOT, name);
	FILE *f;
    int i;

	for (i=0; i<10 && (f=_wfopen(ret, TEXT("r")))!=NULL; i++ ) {
		fclose(f);
		wsprintf(ret, TEXT("%s\\%d%s"), TEMPROOT, i, name);
	}

	if(i==10) {
		// Can't find a free temp file name !
		delete [] ret;
		return NULL;
	}
    
	return ret;*/
}


bool saveFile(const char *filename, const char *buffer, size_t len, bool binary)
{
	const char *mode = binary ? "wb" : "w" ;

    FILE *f = fopen(filename, mode);

    if(!f)
        return false;

    if (fwrite(buffer, sizeof(char), len, f) != len) {
        fclose(f);
        return false;
    }
    fclose(f);

    return true;
}

size_t fgetsize(FILE *f)
{
    size_t size;

    fseek(f, 0, SEEK_END);
    size=ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

bool readFile(const char* path, char **message, size_t *len, bool binary)
{
    FILE *f;
    size_t msglen=0;
    char *msg=0;
    const char *mode = binary ? "rb" : "r" ;

    f = fopen(path, mode);
    if ( !f ) {
        return false;
    }
    msglen = fgetsize(f);
    msg = new char[msglen+1];

    fread(msg, msglen, sizeof(char), f);

    if(ferror(f)){
        delete [] msg;
        return false;
    }
    fclose(f);
    // Terminate the string, so that if it's a text file, the result is
    // a NULL terminated string
    msg[msglen]=0;
    *message= msg ;
    *len=msglen;

    return true;
}


#ifdef __DEBUG__

    //
    // This is required since in debug mode, new is rewritten
    // as new(__FILE__, __LINE__). See utils.h for details
    //
    #undef new
    #undef delete

    #include <stddef.h>

    void *operator new(size_t s, char* file, int line) {
        wchar_t m[256];

        void* p = malloc(s);

        // /** WARNING: this sloooooowwwwssss doooowwwwnnnn things
        wcsprintf(m, L"%S:%d new - s:%ld, p:%lx\n", file, line, s, p);
        LOG.error(m);
        // **/

        return p;
    }

    void *operator new(size_t s) {
        return ::operator new(s, "", 0);
    }

    void *operator new[](size_t s) {
        return ::operator new(s, "", 0);
    }

    void *operator new[](size_t s, char* file, int line) {
        return ::operator new(s, file, line);
    }

    void operator delete(void* p) {
        wchar_t m[216];

        // /** WARNING: this sloooooowwwwssss doooowwwwnnnn things
        wcsprintf(m, L"delete - p:%lx\n", (long)p);
        LOG.error(m);
        // **/

        if (p) {
            free(p);
        }
    }

    void operator delete[] (void* p) {
        ::operator delete(p);
    }

#endif

