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
#include "base/Log.h"
#include "base/debug.h"
#include "base/messages.h"
#include "base/util/StringBuffer.h"
#include "base/util/utils.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/HTTPHeader.h"
#include "http/PalmTransportAgent.h"

PalmTransportAgent::PalmTransportAgent() : TransportAgent() {
    sk = NULL;
    init();
}

PalmTransportAgent::PalmTransportAgent(URL& newURL, Proxy& proxy, unsigned int timeout = DEFAULT_MAX_TIMEOUT) :
    TransportAgent(newURL, proxy, timeout) {
    sk = NULL;
    init();
}


PalmTransportAgent::~PalmTransportAgent() {
    if (sk) {
        sk->disconnect();
        sk->close();

        delete sk;
    }
}


void PalmTransportAgent::init() {
    if (sk == NULL) {
        sk = new Sock();

        Err err=sk->findLibrary();
            if (err==0) {
                err=sk->openNetLibrary();
                sk->setTimeout(timeout);
            } else {
                lastErrorCode = ERR_NETWORK_INIT;
        }
    }

}

WCHAR* PalmTransportAgent::sendMessage(WCHAR* msg) {

    int status = 0;

    char*    body    = NULL;
    char*    content = NULL;
    WCHAR* ret     = NULL;

    int contentLength = 0;

    HTTPHeader* hdr = NULL;
    int msgLen = wcslen(msg);

    //
    // Let's create the HTTP post request
    //
    StringBuffer sb;
    sb.append("POST ");
    sb.append(url.resource);
    sb.append(" HTTP/1.0\r\nContent-Type: ");
    sb.append(SYNCML_CONTENT_TYPE);
    sb.append("\r\nContent-Length: ");
    sb.append(msgLen);
    sb.append("\r\nUser-Agent: ");
    sb.append(USER_AGENT);
    sb.append("\r\n\r\n");

    int headerLen = sb.length();

    LOG.debug(DBG_HTTP_HEADERS);
    LOG.debug(sb.getChars());

    //
    // We are now ready to open the socket and writing content
    // in there.
    //
    Err err=sk->open();
    if (err!=0) {
        lastErrorCode=ERR_CONNECT;
        goto finally;
    }

    err=sk->connect(url.host, url.port);
    if (err!=0) {
        lastErrorCode = ERR_HOST_NOT_FOUND;
        goto finally;
    }

    err=sk->sendData(sb.getChars());
    if (err!=0) {
        lastErrorCode=ERR_WRITING_CONTENT;
        goto finally;
    }
    err=sk->sendData(msg);
    if (err!=0) {
        lastErrorCode=ERR_WRITING_CONTENT;
        goto finally;
    }

    err=sk->receiveData();
    if (err!=0) {
        lastErrorCode=ERR_READING_CONTENT;
        goto finally;
    }

    if (sk->getContentLength() == 0) {
        lastErrorCode=ERR_READING_CONTENT;
        goto finally;
    }

    content = sk->getContent();

    LOG.debug(DBG_RECEIVED_CONTENT);
    LOG.debug(content);

    hdr = new HTTPHeader(content);

    status = hdr->getStatus();
    switch (status) {
        case HTTP_OK:
            break;

        case HTTP_NOT_FOUND:
            lastErrorCode = ERR_HTTP;
            wcsprintf(lastErrorMsg, TEXT("%d - %s"), status, hdr->getStatusMessage());
            goto finally;

        default:
            lastErrorCode = ERR_SERVER_ERROR;
            wcsprintf(lastErrorMsg, TEXT("%d - %s"), status, hdr->getStatusMessage());
            goto finally;
    }

    contentLength = hdr->getContentLength();
    if (((int)contentLength) <= 0) {
        lastErrorCode=ERR_HTTP_MISSING_CONTENT_LENGTH;
        goto finally;
    }

    ret = stringdup(hdr->getContent());

finally:

    sk->disconnect();
    sk->close();

    delete hdr;

    if (body) {
        delete [] body; body = NULL;
    }

    return ret;
}