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
#include "base/messages.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/Win32TransportAgent.h"

/*
 * This is the Win32 implementation of the TransportAgent object
 */

/*
 * This function translate a UNICODE string into a UTF string without
 * allocating additional memory. The translation is performed removing
 * the second byte of the UNICODE coding.
 *
 * @param s the string to translate
 */
void toUTF(wchar_t* s) {
    int l = wcslen(s);
    wchar_t* w = s;
    char*    c = (char*)s;

    while (l--) {
        *c = (char)*w;
        ++c; ++w;
    }

    *c = 0;
}



/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
Win32TransportAgent::Win32TransportAgent(URL& newURL, Proxy& newProxy, unsigned int responseTimeout, unsigned int maxmsgsize) : TransportAgent(){
    url   = newURL  ;        
}

Win32TransportAgent::~Win32TransportAgent(){}


/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */
wchar_t* Win32TransportAgent::sendMessage(wchar_t* msg) {
    HINTERNET inet       = NULL,
              connection = NULL,
              request    = NULL;
    char bufferA[5000+1];
    int status = -1;
    int contentLength = 0;

    DWORD size = 0,
          read = 0,
          flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;

    wchar_t* response = NULL;
    wchar_t* p        = NULL;

    LPTSTR acceptTypes[2] = {TEXT("*/*"), NULL};

    inet = InternetOpen (USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);

    if (!inet) {
        lastErrorCode = ERR_NETWORK_INIT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("InternetOpen Error"), GetLastError());
        goto exit;
    }

    wsprintf(logmsg, TEXT("Connecting to %s:%d"), url.host, url.port);
    LOG.debug(logmsg);

    // Open an HTTP session for a specified site by using lpszServer.
    if (!(connection = InternetConnect (inet,
                                        url.host,
                                        url.port,
                                        NULL, // username
                                        NULL, // password
                                        INTERNET_SERVICE_HTTP,
                                        0,
                                        0))) {
      lastErrorCode = ERR_CONNECT;
      wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("InternetConnect Error"), GetLastError());
      goto exit;
    }

    wsprintf(logmsg, TEXT("Requesting resource %s"), url.resource);
    LOG.debug(logmsg);

    // Open an HTTP request handle.
    if (!(request = HttpOpenRequest (connection,
                                     METHOD_POST,
                                     url.resource,
                                     HTTP_VERSION,
                                     NULL,
                                     (LPCTSTR*)acceptTypes,
                                     flags, 0))) {
      lastErrorCode = ERR_CONNECT;
      wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpOpenRequest Error"), GetLastError());
      goto exit;
    }

    //
    // Prepares headers
    //
    wchar_t headers[512];

    contentLength = wcslen(msg);
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);


    // Send a request to the HTTP server.
    toUTF(msg);
    if (!HttpSendRequest (request, headers, wcslen(headers), (void*)msg, contentLength)) {
        lastErrorCode = ERR_CONNECT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpSendRequest Error"),GetLastError());
        goto exit;
    }
    
    LOG.debug(MESSAGE_SENT);
    LOG.debug(READING_RESPONSE);
    //
    // First of all, check the status code
    //

    size = sizeof(status);
    HttpQueryInfo (request,
                   HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                   (LPDWORD)&status,
                   (LPDWORD)&size,
                   NULL);

    //
    // If status code is not OK, returns immediately, otherwise reads the response
    //
    if (status != STATUS_OK) {
        lastErrorCode = ERR_HTTP;
        wsprintf(lastErrorMsg, TEXT("HTTP request error: %d"), status);
        goto exit;
    }

    HttpQueryInfo (request,
                   HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                   (LPDWORD)&contentLength,
                   (LPDWORD)&size,
                   NULL);


    wsprintf(logmsg, TEXT("Content-length: %d"), contentLength);
    LOG.debug(logmsg);

    if (contentLength <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        wsprintf(lastErrorMsg, TEXT("Invalid content-length: %d"), contentLength);
        goto exit;
    }

    // Allocate a block of memory for lpHeadersW.
    response = p = new wchar_t[contentLength+1];
    (*p) = 0;
    do {
        if (!InternetReadFile (request, (LPVOID)bufferA, 5000, &read)) {
            lastErrorCode = ERR_READING_CONTENT;
            wsprintf(lastErrorMsg, TEXT("%s: %d"), TEXT("InternetReadFile Error"), GetLastError());
            goto exit;
        }

        if (read != 0) {
            bufferA[read] = 0;

            // Get the required size of the buffer which receives the Unicode
            // string.
            size = MultiByteToWideChar (CP_ACP, 0, bufferA, -1, NULL, 0);          

            // Convert the buffer from ASCII to Unicode.
            MultiByteToWideChar (CP_ACP, 0, bufferA, read, p, read);
            p[read] = 0;
            p += size -1;
    
        }
    } while (read);
    
    LOG.debug(response);
    LOG.debug(TEXT("Response read"));   

    exit:

    // Close the Internet handles.
    if (inet) {
        InternetCloseHandle (inet);
    }

    if (connection) {
        InternetCloseHandle (connection);
    }

    if (request) {
        InternetCloseHandle (request);
    }

    if ((status != STATUS_OK) && (response !=NULL)) {
        delete [] response; response = NULL;
    }

    return response;
}
