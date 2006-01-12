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

#ifndef INCL_TRANSPORT_AGENT
    #define INCL_TRANSPORT_AGENT

    #include "base/fscapi.h"

    #include "http/URL.h"
    #include "http/Proxy.h"

    //
    // number of seconds of waiting response timeout.
    //
    #define DEFAULT_MAX_TIMEOUT 300

    //
    // the max_msg_size parameter. If 0 the max msg size is not specified by the client.
    // The value is expressed in byte
    //
    #define DEFAULT_MAX_MSG_SIZE 0    

    /*
     * This class is the transport agent responsible for messages exchange
     * over an HTTP connection.
     * This is a generic abtract class which is not bound to any paltform
     */

    class TransportAgent {

    protected:
        URL url;
        Proxy proxy;

        unsigned int timeout;
        unsigned int maxmsgsize;

    public:
        TransportAgent() EXTRA_SECTION_03;
        TransportAgent(URL& url, 
                       Proxy& proxy, 
                       unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT, 
                       unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE) EXTRA_SECTION_03;

        virtual ~TransportAgent() EXTRA_SECTION_03;

        /*
         * Change the URL the subsequent calls to setMessage() should
         * use as target url.
         *
         * @param url the new target url
         */
        void setURL(URL& newURL) EXTRA_SECTION_03;

        /*
         * Returns the url.
         */
        URL& getURL() EXTRA_SECTION_03;

        /**
         * Sets the connection timeout
         *
         * @param t the new timeout in seconds
         */
        void setTimeout(unsigned int t) EXTRA_SECTION_03;

        /**
         * Returns the connection timeout
         */
        unsigned int getTimeout() EXTRA_SECTION_03;
        
        /**
         * Sets the max msg size
         *
         * @param t the new msx msg size in bytes
         */
        void setMaxMsgSize(unsigned int t) EXTRA_SECTION_03;
         
        /**
         * Returns the max msg size
         */
        unsigned int getMaxMsgSize()EXTRA_SECTION_03;


        /*
         * Sends the given SyncML message to the server specified
         * by the install property 'url'. Returns the server's response.
         * The response string has to be freed with delete [].
         * In case of an error, NULL is returned and lastErrorCode/Msg
         * is set.
         */
        virtual wchar_t* sendMessage(wchar_t* msg) = 0;

    };

#endif
