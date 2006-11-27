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
#ifndef INCL_HTTP_PROXY
#define INCL_HTTP_PROXY

#include "base/fscapi.h"
#include "base/constants.h"
#include "http/constants.h"


class Proxy {

    public:
        char host    [DIM_HOSTNAME];
        char user    [DIM_USERNAME];
        char password[DIM_PASSWORD];
        int     port;

        Proxy() EXTRA_SECTION_03;
        Proxy(char*  host, int port) EXTRA_SECTION_03;

        Proxy(char*  host, int port, char*  user, char*  password) EXTRA_SECTION_03;

        void setProxy(Proxy& proxy) EXTRA_SECTION_03;
        void setProxy(const char* proxyHost, int proxyPort, const char* proxyUser, const char* proxyPassword) EXTRA_SECTION_03;

        Proxy& operator= (Proxy& proxy) { setProxy(proxy); return *this;}
    };
#endif
