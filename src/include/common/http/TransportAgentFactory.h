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

#ifndef INCL_TRANSPORT_AGENT_FACTORY
    #define INCL_TRANSPORT_AGENT_FACTORY

    #include "base/fscapi.h"

    #include "http/URL.h"
    #include "http/Proxy.h"
    #include "http/TransportAgent.h"


    /*
     * This is a factory class for the platform specific TransportAgent.
     * It must be specifically implemented for any classes
     */

    class TransportAgentFactory {

        public:
            static TransportAgent* getTransportAgent(URL& url, Proxy& proxy, 
                                                     unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT, 
                                                     unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE) EXTRA_SECTION_03;
    };

#endif
