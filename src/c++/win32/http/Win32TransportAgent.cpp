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

/**
 How to test SSL connections on Win32
 -------------------------------------

 On the server:
 1) create the keystore:
    %JAVA_HOME%\bin\keytool -genkey -alias tomcat -keyalg RSA
 2) In $CATALINA_HOME/conf/server.xml uncomment the lines:
    <Connector className="org.apache.catalina.connector.http.HttpConnector"
               port="8443" minProcessors="5" maxProcessors="75"
               enableLookups="true"
               acceptCount="10" debug="0" scheme="https" secure="true">
      <Factory className="org.apache.catalina.net.SSLServerSocketFactory" clientAuth="false" protocol="TLS"/>
    </Connector>
 2) Export the certificate from the key store:
    %JAVA_HOME%\bin\keytool -export -alias tomcat -file myroot.cer  

 On the client:
 1) Connect (via https) to the server using a web-browser (type "https://<server_address>:8443)
 2) Accept and install the certificate sent from the server
**/

#include "base/Log.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/Win32TransportAgent.h"


#define ENTERING(func) //bsprintf(logmsg, "Entering %ls", func); LOG.debug(logmsg)
#define EXITING(func)  //bsprintf(logmsg, "Exiting %ls", func);  LOG.debug(logmsg)


/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
Win32TransportAgent::Win32TransportAgent(URL& newURL, Proxy& newProxy, 
										 unsigned int maxResponseTimeout, 
										 unsigned int maxmsgsize) : TransportAgent(){
	url = newURL;

    if (maxResponseTimeout == 0) {
        setTimeout(DEFAULT_MAX_TIMEOUT);
    } else {
        setTimeout(maxResponseTimeout);
    }
}

Win32TransportAgent::~Win32TransportAgent(){}



/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 */
BCHAR* Win32TransportAgent::sendMessage(const BCHAR* msg) {

    ENTERING(L"TransportAgent::sendMessage");
    
    HINTERNET inet       = NULL,
              connection = NULL,
              request    = NULL;

    char bufferA[5000+1];
    int status = -1;
    int contentLength = 0;
	wchar_t* wurlHost;
    wchar_t* wurlResource;
    BCHAR* response = NULL;
    BCHAR* p        = NULL;

    DWORD size  = 0,
          read  = 0,
          flags = INTERNET_FLAG_RELOAD | 
                  INTERNET_FLAG_NO_CACHE_WRITE | 
                  INTERNET_FLAG_KEEP_CONNECTION;            // This is necessary if authentication is required.

	LPCWSTR acceptTypes[2] = {TEXT("*/*"), NULL};
    

    // Set flags for secure connection (https).
    if (url.isSecure()) {
        flags = flags
              | INTERNET_FLAG_SECURE
              | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
              | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
              ;
    }


    //
    // Open Internet connection.
    //
	wchar_t* ua = toWideChar(userAgent);
    inet = InternetOpen (ua, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
	if (ua) {delete [] ua; ua = NULL; }

    if (!inet) {
        lastErrorCode = ERR_NETWORK_INIT;
        bsprintf (lastErrorMsg, T("%s: %d"), T("InternetOpen Error"), GetLastError());
		LOG.error(lastErrorMsg);
        goto exit;
    }   
    bsprintf(logmsg, T("Connecting to %s:%d"), url.host, url.port);
    LOG.debug(logmsg);


    //
    // Open an HTTP session for a specified site by using lpszServer.
    //
	wurlHost = toWideChar(url.host);
    if (!(connection = InternetConnect (inet,
                                        wurlHost,
                                        url.port,
                                        NULL, // username
                                        NULL, // password
                                        INTERNET_SERVICE_HTTP,
                                        0,
                                        0))) {
      lastErrorCode = ERR_CONNECT;
      bsprintf (lastErrorMsg, T("%s: %d"), T("InternetConnect Error"), GetLastError());
	  LOG.error(lastErrorMsg);
      goto exit;
    }
    bsprintf(logmsg, T("Requesting resource %s"), url.resource);
    LOG.debug(logmsg);

    //
    // Open an HTTP request handle.
	//
    wurlResource = toWideChar(url.resource);
    if (!(request = HttpOpenRequest (connection,
                                     METHOD_POST,
                                     wurlResource,
                                     HTTP_VERSION,
                                     NULL,
                                     acceptTypes,
                                     flags, 0))) {
      lastErrorCode = ERR_CONNECT;
      bsprintf (lastErrorMsg, T("%s: %d"), T("HttpOpenRequest Error"), GetLastError());
	  LOG.error(lastErrorMsg);
      goto exit;
    }


    //
    // Prepares headers
    //
    wchar_t headers[512];

    contentLength = bstrlen(msg);
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);


    //
    // Try 5 times to send http request: used to retry sending request in case
    // of authentication (proxy/server).
    //
    for (int i=0; i<MAX_AUTH_ATTEMPT; i++) {

        // Send a request to the HTTP server.
        if (!HttpSendRequest (request, headers, wcslen(headers), (void*)msg, contentLength)) {
            lastErrorCode = ERR_CONNECT;
            bsprintf (lastErrorMsg, T("%s: %d"), T("HttpSendRequest Error"), GetLastError());
		    LOG.error(lastErrorMsg);
            goto exit;
        }
        LOG.debug(MESSAGE_SENT);


        // Check the status code.
        size = sizeof(status);
        HttpQueryInfo (request,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       (LPDWORD)&status,
                       (LPDWORD)&size,
                       NULL);

        //
        // OK (200)
        //
        if (status == HTTP_STATUS_OK) {
            break;
        }

        //
        // Proxy Authentication Required (407) / Server Authentication Required (401).
        // Prompt dialog box to set username/password.
        //
        else if(status == HTTP_STATUS_PROXY_AUTH_REQ ||
                status == HTTP_STATUS_DENIED) {
            
            LOG.debug("HTTP Authentication required.");
            DWORD dwError = InternetErrorDlg(GetDesktopWindow(), request, NULL, 
                                             FLAGS_ERROR_UI_FILTER_FOR_ERRORS | 
                                             FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
                                             FLAGS_ERROR_UI_FLAGS_GENERATE_DATA,
                                             NULL);
            
            if (dwError == ERROR_INTERNET_FORCE_RETRY) {
                continue;
            }
            else {
                LOG.error("HTTP Authentication failed.");
                break;
            }
        }
        
        //
        // Other HTTP errors
        //
        else {
            break;
        }
    }

    // If wrong status, exit immediately.
    if (status != HTTP_STATUS_OK) {
        lastErrorCode = ERR_HTTP;
        bsprintf(lastErrorMsg, T("HTTP request error: %d"), status);
        LOG.error("%s", lastErrorMsg);
        goto exit;
    }

    HttpQueryInfo (request,
                   HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                   (LPDWORD)&contentLength,
                   (LPDWORD)&size,
                   NULL);



    // ====================== Reading Response ==============================
    LOG.debug(READING_RESPONSE);
    bsprintf(logmsg, T("Content-length: %d"), contentLength);
    LOG.debug(logmsg);

    if (contentLength <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        bsprintf(lastErrorMsg, T("Invalid content-length: %d"), contentLength);
		LOG.error(lastErrorMsg);
        goto exit;
    }

    // Allocate a block of memory for lpHeadersW.
    response = new BCHAR[contentLength+1];
	p = response;
    (*p) = 0;
    do {
        if (!InternetReadFile (request, (LPVOID)bufferA, 5000, &read)) {
            lastErrorCode = ERR_READING_CONTENT;
            bsprintf(lastErrorMsg, T("%s: %d"), T("InternetReadFile Error"), GetLastError());
			LOG.error(lastErrorMsg);
            goto exit;
        }

        if (read != 0) {
            bufferA[read] = 0;

            bstrcpy(p, bufferA);
            p += strlen(bufferA);
        }

    } while (read);

    LOG.debug(T("Response read"));
	LOG.debug(response); 


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

    EXITING(L"TransportAgent::sendMessage");

    return response;
}
