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

#include "http/Proxy.h"

Proxy::Proxy() {
    setProxy(NULL, 0, NULL, NULL);
}


Proxy::Proxy(BCHAR* proxyHost, int proxyPort) {
    setProxy(proxyHost, proxyPort, NULL, NULL);
}


Proxy::Proxy(BCHAR* proxyHost, int proxyPort, BCHAR* proxyUser, BCHAR* proxyPassword){
    setProxy(proxyHost, proxyPort, proxyUser, proxyPassword);
}

void Proxy::setProxy(Proxy& newProxy) {
    setProxy(newProxy.host, newProxy.port, newProxy.user, newProxy.password);
}


void Proxy::setProxy(BCHAR* proxyHost    ,
                     int      proxyPort    ,
                     BCHAR* proxyUser    ,
                     BCHAR* proxyPassword) {
    if (proxyHost != NULL) {
        bstrncpy((BCHAR*)host, (const BCHAR*)proxyHost, DIM_HOSTNAME);
        host[DIM_HOSTNAME-1] = 0;
    } else {
        memset(host, 0, DIM_HOSTNAME*sizeof(BCHAR));
        //bstrcpy((BCHAR*)host, (const BCHAR*)"");
    }

    if (proxyUser != NULL){
        bstrncpy((BCHAR*)user, (const BCHAR*)proxyUser, DIM_USERNAME);
        user[DIM_USERNAME-1] = 0;
    } else {
        memset(user, 0, DIM_USERNAME*sizeof(BCHAR));
        //bstrcpy((BCHAR*)user, (BCHAR*)"");
    }

    if (proxyPassword != NULL){
        bstrncpy((BCHAR*)password,(const BCHAR*) proxyPassword, DIM_PASSWORD);
        password[DIM_PASSWORD-1] = 0;
    } else {
        memset(password, 0, DIM_PASSWORD*sizeof(BCHAR));
        //bstrcpy((BCHAR*)password, (const BCHAR*)"");
    }
    port = proxyPort;
}
