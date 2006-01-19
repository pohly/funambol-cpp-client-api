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


#include "base/util/utils.h"
#include "http/URL.h"


/*
 * Creates a new URL object from a string representation of the URL. The URL
 * must be in the form:
 *
 * <protocol>://<hostname>[:<port>]/<resource>
 *
 * If a parsing error occurs, fullURL is set to an empty string ("").
 *
 * @param url the URL
 */
URL::URL(const wchar_t* url) : fullURL(NULL), protocol(NULL), host(NULL), resource(NULL){
    setURL(url);
}

/*
 * Default constructs
 */
URL::URL() : fullURL(NULL), protocol(NULL), host(NULL), resource(NULL) {
}

URL::~URL() {
    if (fullURL) {
        delete [] fullURL; fullURL = NULL;
    }

    if (protocol) {
        delete [] protocol; protocol = NULL;
    }

    if (host) {
        delete [] host; host = NULL;
    }

    if (resource) {
        delete [] resource; resource = NULL;
    }

}

void URL::setURL(URL& url) {
    setURL(url.fullURL, url.protocol, url.host, url.resource, url.port);
}

void URL::setURL(wchar_t* u, wchar_t* p, wchar_t* h, wchar_t*r, unsigned int port) {
    if (fullURL) {
        delete [] fullURL; fullURL = NULL;
    }

    if (u) {
        fullURL = stringdup(u);
    }

    if (protocol) {
        delete [] protocol; protocol = NULL;
    }

    if (p) {
        protocol = stringdup(p);
    }

    if (host) {
        delete [] host; host = NULL;
    }

    if (h) {
        host = stringdup(h);
    }

    if (resource) {
        delete [] resource; resource = NULL;
    }

    if (r) {
        resource = stringdup(r);
    }

    if (port == -1) {
        //
        // use default ports
        //
        this->port = isSecure() ? 443 : 80;
    } else {
        this->port = port;
    }
}

void URL::setURL(const wchar_t* url) {
    if ((url == NULL) || (wcslen(url) == 0)) {
        return;
    }

    int size;

    wchar_t* s = NULL;
    wchar_t* q = NULL;

    //
    // protocol (mandatory)
    //
    s = wcsstr((wchar_t*)url, TEXT("://"));
    if ((s == NULL) || (s == url)) {
        return;
    }

    size = s-url;
    wchar_t* p = new wchar_t[size+1];
    wcsncpy(p, url, size);  p[size] = 0;

    //
    // server (mandatory)
    // and
    // port (optional)
    //
    s += 3;
    q = wcsstr(s, TEXT("/"));
    if (q == NULL) {
        size = wcslen(s);
    } else {
        size = q-s;
    }
    wchar_t* h = new wchar_t[size+1];    
    wcsncpy(h, s, size); h[size] = 0;

    unsigned int port = (unsigned int)-1;
    s = wcsstr(h, TEXT(":"));
    if (s) {
        port = wcstol(s+1, NULL, 10);
        *s = 0;
    }

    //
    // resource
    //
    size = q ? wcslen(q) : 0;
    wchar_t* r = new wchar_t[size+1];
    if (size) wcsncpy(r, q, size); r[size] = 0;

    //
    // fullURL
    //
    size = wcslen(url);
    wchar_t* u = new wchar_t[size+1];
    wcscpy(u, url);

    setURL(u, p, h, r, port);

    //
    // frees all pointer
    //
    if (p != NULL)
        delete [] p;
    if (u != NULL)
        delete [] u;   
    if (h != NULL)
        delete [] h;
    if (r != NULL)
        delete [] r;


}

URL& URL::operator= (URL& url) {
    setURL(url); return *this;
}

URL& URL::operator= (const wchar_t* url) {
    setURL(url); return *this;
}

BOOL URL::isSecure() {
   wchar_t* t = wcstolower(protocol);

   BOOL ret = (wcscmp(t, TEXT("https")) == 0);

   delete [] t; t = NULL;

   return ret;
}
