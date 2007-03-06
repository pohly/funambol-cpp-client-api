/*
 * Copyright (C) 2003-2007 Funambol
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
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "http/constants.h"

    class __declspec(dllexport) URL {

    public:
        char*  fullURL ;
        char*  protocol;
        char*  host    ;
        char*  resource;
        int      port    ;

        URL() EXTRA_SECTION_03;
        URL(const char*  url) EXTRA_SECTION_03;
        ~URL() EXTRA_SECTION_03;

        void setURL(URL& url) EXTRA_SECTION_03;
        void setURL(const char*  url) EXTRA_SECTION_03;

        BOOL isSecure() EXTRA_SECTION_03;

        URL& operator= (URL& url) EXTRA_SECTION_03;
        URL& operator= (const char*  url) EXTRA_SECTION_03;

    protected:
        void setURL(const char* u, const char* p, const char* h, const char* r, unsigned int port) EXTRA_SECTION_03;
    };
/** @endcond */
#endif
