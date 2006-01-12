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
#ifndef INCL_HTTP_URL
    #define INCL_HTTP_URL

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "http/constants.h"

    class __declspec(dllexport) URL {

    public:
        wchar_t* fullURL ;
        wchar_t* protocol;
        wchar_t* host    ;
        wchar_t* resource;
        int      port    ;

        URL() EXTRA_SECTION_03;
        URL(const wchar_t* url) EXTRA_SECTION_03;
        ~URL() EXTRA_SECTION_03;

        void setURL(URL& url) EXTRA_SECTION_03;
        void setURL(const wchar_t* url) EXTRA_SECTION_03;

        BOOL isSecure() EXTRA_SECTION_03;

        URL& operator= (URL& url) EXTRA_SECTION_03;
        URL& operator= (const wchar_t* url) EXTRA_SECTION_03;

    protected:
        void setURL(wchar_t* u, wchar_t* p, wchar_t* h, wchar_t*r, unsigned int port) EXTRA_SECTION_03;
    };
#endif
