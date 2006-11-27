/*
 * Copyright (C) 2005-2006 Funambol
 * Author Patrick Ohly
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

#ifndef INCL_CURL_TRANSPORT_AGENT
    #define INCL_CURL_TRANSPORT_AGENT

    #include "base/fscapi.h"

    #include "http/URL.h"
    #include "http/Proxy.h"
    #include "http/TransportAgent.h"

    #include "curl/curl.h"

    /*
     * This class is the transport agent responsible for messages exchange
     * over an HTTP connection. It uses libcurl.
     */

    class CurlTransportAgent : public TransportAgent {
        CURL *easyhandle;
        char proxyauth[DIM_USERNAME + 1 + DIM_PASSWORD];

        char *sendbuffer;
        size_t sent, sendbuffersize;
        static size_t sendData(void *ptr, size_t size, size_t nmemb, void *stream);

        char *responsebuffer;
        size_t received, responsebuffersize;
        static size_t receiveData(void *buffer, size_t size, size_t nmemb, void *stream);

        char curlerrortxt[CURL_ERROR_SIZE];
        
        static int debugCallback(CURL *easyhandle, curl_infotype type, char *data, size_t size, void *unused);

    public:
        CurlTransportAgent();
        CurlTransportAgent(URL& url, Proxy& proxy, unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT);
        ~CurlTransportAgent();

        char* sendMessage(const char* msg);
        void setUserAgent(const char* ua);

    };

#endif
