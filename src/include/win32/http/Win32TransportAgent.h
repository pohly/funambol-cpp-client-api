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

#ifndef INCL_WIN32_TRANSPORT_AGENT
    #define INCL_WIN32_TRANSPORT_AGENT

    #include "base/fscapi.h"

    #include "http/URL.h"
    #include "http/Proxy.h"
    #include "http/TransportAgent.h"


    /*
     * This class is the transport agent responsible for messages exchange
     * over an HTTP connection.
     * This is a generic abtract class which is not bound to any paltform
     */

    class Win32TransportAgent : public TransportAgent {


    public:
        Win32TransportAgent();
        Win32TransportAgent(URL& url, Proxy& proxy, 
                            unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT, 
                            unsigned int msxmsgsize = DEFAULT_MAX_MSG_SIZE);
        ~Win32TransportAgent();

        /*
         * Sends the given SyncML message to the server specified
         * by the instal property 'url'. Returns the response status code.
         * Use getResponse() to get the server response.
         */
        wchar_t* sendMessage(wchar_t* msg);
    };

#endif