/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

#ifndef INCL_PALM_TRANSPORT_AGENT
    #define INCL_PALM_TRANSPORT_AGENT
/** @cond DEV */

    #include "base/fscapi.h"

    #include "http/URL.h"
    #include "http/Proxy.h"
    #include "http/TransportAgent.h"

    #include "http/Sock.h"


    /*
     * This class is the transport agent responsible for messages exchange
     * over an HTTP connection.
     * This is a generic abtract class which is not bound to any paltform
     */

    class PalmTransportAgent : public TransportAgent {

    private:
        Sock* sk;

        void init() EXTRA_SECTION_03;

    public:
        PalmTransportAgent() EXTRA_SECTION_03;
        PalmTransportAgent(URL& url, Proxy& proxy, unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT) EXTRA_SECTION_03;
        ~PalmTransportAgent() EXTRA_SECTION_03;

        /*
         * Sends the given SyncML message to the server specified
         * by the instal property 'url'. Returns the response status code.
         * Use getResponse() to get the server response.
         */
        WCHAR* sendMessage(WCHAR* msg);
    };

/** @endcond */
#endif