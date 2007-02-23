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
#include "http/constants.h"
#include "http/HTTPHeader.h"
#include "http/TransportAgent.h"
#include "base/util/utils.h"

TransportAgent::TransportAgent() {
    timeout = DEFAULT_MAX_TIMEOUT;
    maxmsgsize = DEFAULT_MAX_MSG_SIZE;
    userAgent[0] = 0;
}

TransportAgent::TransportAgent(URL& newURL, Proxy& proxy, unsigned int timeout, unsigned int maxmsgsize) {
    url     = newURL ;
    timeout = timeout;
    maxmsgsize  = maxmsgsize;
    userAgent[0] = 0;
}

TransportAgent::~TransportAgent() {
}

void TransportAgent::setURL(URL& newURL) {
    url = newURL;
}

URL& TransportAgent::getURL() {
    return url;
}

void TransportAgent::setTimeout(unsigned int t) {
    timeout = t;
}

unsigned int TransportAgent::getTimeout() {
    return timeout;
}

void TransportAgent::setMaxMsgSize(unsigned int t) {
    maxmsgsize = t;
}

unsigned int TransportAgent::getMaxMsgSize() {
    return maxmsgsize;
}

void TransportAgent::setReadBufferSize(unsigned int t) {
    readBufferSize = t;
}

unsigned int TransportAgent::getReadBufferSize() {
    return readBufferSize;
}

void TransportAgent::setUserAgent(const char* ua) {
    if (ua) 
        strcpy(userAgent, ua);
}

const char* TransportAgent::getUserAgent() {
    return stringdup(userAgent);
}
