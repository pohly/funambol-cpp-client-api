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

#ifndef INCL_SOCK
#define INCL_SOCK

#include "base/fscapi.h"
#include "base/constants.h"


class Sock {
    public:
        // costruttore e distruttore
        Sock(unsigned long maxMsgSize = MAX_MSG_SIZE) EXTRA_SECTION_03;
        ~Sock() EXTRA_SECTION_03;

        // gestione tempi
        void setTimeout(UInt16) EXTRA_SECTION_03;
        UInt16 getTimeout() EXTRA_SECTION_03;
        UInt16 getTicks() EXTRA_SECTION_03;

        // gestione NetLibrary
        Err findLibrary() EXTRA_SECTION_03;
        Err openNetLibrary() EXTRA_SECTION_03;
        Err closeNetLibrary() EXTRA_SECTION_03;

        // gestione apertura e chiusura del socket
        Err open() EXTRA_SECTION_03;
        Err close() EXTRA_SECTION_03;

        // gestione connessione e disconnessione del socket
        Err connect(char* ipAddr,UInt16 port) EXTRA_SECTION_03;
        Err disconnect() EXTRA_SECTION_03;

        // gestione invio e ricezione dati
        Err sendData(char* String) EXTRA_SECTION_03;
        Err receiveData() EXTRA_SECTION_03;

        // gestione dati ricevuti
        const char* getContent() EXTRA_SECTION_03;
        UInt16 getContentLength() EXTRA_SECTION_03;

    private:

        NetSocketAddrINType* addr;
        UInt16 netRefnum;
        NetSocketRef socketRef;
        Int32 AppNetTimeout;
        Int32 timeOut;

        char* content;
        unsigned long contentLength;
        unsigned long maxMsgSize;

        Boolean isDotted(char* hostName) EXTRA_SECTION_03;
};


NetIPAddr GetIPAddress(Char* hostName,UInt16 netRefNum,Int32 timeOut);


#endif