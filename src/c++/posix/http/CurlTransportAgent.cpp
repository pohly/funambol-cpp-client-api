/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#include "base/Log.h"
#include "base/messages.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/CurlTransportAgent.h"

/*
 * This is the libcurl implementation of the TransportAgent object
 */

/*
 * Singleton: initialize libcurl once per program
 */
class CurlInit {
    static CURLcode initres;
    static CurlInit singleton;

    CurlInit() { initres = curl_global_init(CURL_GLOBAL_ALL); }
    ~CurlInit() { curl_global_cleanup(); initres = CURLE_FAILED_INIT; }

public:
    static CURL *easy_init() { return !initres ? curl_easy_init() : NULL; }
};

CURLcode CurlInit::initres;
CurlInit CurlInit::singleton;


/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
CurlTransportAgent::CurlTransportAgent(URL& newURL, Proxy& newProxy, unsigned int maxResponseTimeout) : TransportAgent(newURL, newProxy, maxResponseTimeout){
    easyhandle = CurlInit::easy_init();
    if (easyhandle) {
        curl_easy_setopt(easyhandle, CURLOPT_DEBUGFUNCTION, debugCallback);
        curl_easy_setopt(easyhandle, CURLOPT_VERBOSE, LOG.getLevel() ? TRUE : FALSE);
        curl_easy_setopt(easyhandle, CURLOPT_NOPROGRESS, TRUE);
        curl_easy_setopt(easyhandle, CURLOPT_WRITEFUNCTION, receiveData);
        curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(easyhandle, CURLOPT_READFUNCTION, sendData);
        curl_easy_setopt(easyhandle, CURLOPT_READDATA, this);
        curl_easy_setopt(easyhandle, CURLOPT_ERRORBUFFER, this->curlerrortxt );
        curl_easy_setopt(easyhandle, CURLOPT_AUTOREFERER, TRUE);
        curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION, TRUE);
        if (proxy.host[0]) {
            curl_easy_setopt(easyhandle, CURLOPT_PROXY, proxy.host);
            if (proxy.port) {
                curl_easy_setopt(easyhandle, CURLOPT_PROXYPORT, proxy.port);
            }
            if (proxy.user) {
                sprintf(proxyauth, "%s:%s", proxy.user, proxy.password);
                curl_easy_setopt(easyhandle, CURLOPT_PROXYUSERPWD, proxyauth);
            }
        }
    }
    setUserAgent("Funambol POSIX SyncML client");
}

void CurlTransportAgent::setUserAgent(const char*ua) {
    if (ua) {
        TransportAgent::setUserAgent(ua);
        if (easyhandle) {
            curl_easy_setopt(easyhandle, CURLOPT_USERAGENT, userAgent);
        }
    }
}


CurlTransportAgent::~CurlTransportAgent() {
    if (easyhandle) {
        curl_easy_cleanup(easyhandle);
    }
}

size_t CurlTransportAgent::receiveData(void *buffer, size_t size, size_t nmemb, void *stream) {
    CurlTransportAgent *agent = (CurlTransportAgent *)stream;
    size_t curr = size * nmemb;

    if (agent->received + curr + 1 > agent->responsebuffersize) {
        size_t newbuffersize = agent->responsebuffersize + max(10 * 1024, (curr + 1 + 1024) / 1024 * 1024);
        char *newbuffer = new char[newbuffersize];
        memcpy(newbuffer, agent->responsebuffer, agent->received);
        delete [] agent->responsebuffer;
        agent->responsebuffer = newbuffer;
        agent->responsebuffersize = newbuffersize;
    }
    memcpy(agent->responsebuffer + agent->received,
           buffer,
           curr);
    agent->received += curr;
    agent->responsebuffer[agent->received] = 0;
    return curr;
}

size_t CurlTransportAgent::sendData(void *buffer, size_t size, size_t nmemb, void *stream) {
    CurlTransportAgent *agent = (CurlTransportAgent *)stream;
    size_t curr = min(size * nmemb, agent->sendbuffersize - agent->sent);

    memcpy(buffer, agent->sendbuffer + agent->sent, curr);
    agent->sent += curr;
    return curr;
}

int CurlTransportAgent::debugCallback(CURL *easyhandle, curl_infotype type, char *data, size_t size, void *unused)
{
    BOOL isData = type == CURLINFO_DATA_IN || type == CURLINFO_DATA_OUT;

    if (LOG.getLevel() >= LOG_LEVEL_DEBUG && !isData ||
        LOG.getLevel() > LOG_LEVEL_DEBUG) {
        char *buffer = new char[30 + size];
        sprintf(buffer, "libcurl %s%.*s",
                type == CURLINFO_TEXT ? "info: " :
                type == CURLINFO_HEADER_IN ? "header in: " :
                type == CURLINFO_HEADER_OUT ? "header out:\n" :
                type == CURLINFO_DATA_IN ? "data in:\n" :
                type == CURLINFO_DATA_OUT ? "data out:\n" :
                "???",
                (size > 0 && data[size-1] == '\n') ? (int)size - 1 : (int)size,
                data);
        LOG.debug(buffer);
        delete [] buffer;
    }
    return 0;
}

/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */
char* CurlTransportAgent::sendMessage(const char* msg) {
    if (!easyhandle) {
        lastErrorCode = ERR_NETWORK_INIT;
        strcpy(lastErrorMsg, "libcurl error init error");
        return NULL;
    }

    LOG.debug("Requesting resource %s at %s:%d", url.resource, url.host, url.port);

    curl_slist *slist=NULL;
    char *response = NULL;
    CURLcode code;
    char contenttype[256];
    sprintf(contenttype, "Content-Type: %s", SYNCML_CONTENT_TYPE);
    slist = curl_slist_append(slist, contenttype);
    responsebuffersize = 64 * 1024;
    responsebuffer = new char[responsebuffersize];
    received = 0;
    responsebuffer[0] = 0;
    // todo? url.resource
    if ((code = curl_easy_setopt(easyhandle, CURLOPT_POST, TRUE)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_URL, url.fullURL)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDS, msg)) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_POSTFIELDSIZE, strlen(msg))) ||
        (code = curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, slist)) ||
        (code = curl_easy_perform(easyhandle))) {
        delete [] responsebuffer;
        lastErrorCode = ERR_HTTP;
        sprintf(lastErrorMsg, "libcurl error %d, %.250s", code, curlerrortxt);
    } else {
        response = responsebuffer;
        LOG.debug(response);
        LOG.debug("Response read");
    }
    responsebuffer = NULL;
    responsebuffersize = 0;

    if (slist) {
        curl_slist_free_all(slist);
    }

    return response;
}

