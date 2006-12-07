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
#include "event/FireEvent.h"

#define ENTERING(func) // LOG.debug("Entering %ls", func);
#define EXITING(func)  // LOG.debug("Exiting %ls", func);


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
    proxy.setProxy(newProxy);

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
char* Win32TransportAgent::sendMessage(const char* msg) {

    ENTERING(L"TransportAgent::sendMessage");
    char bufferA[5000+1];
    int status = -1;
    unsigned int contentLength = 0;
	WCHAR* wurlHost;
    WCHAR* wurlResource;
    char* response = NULL;
    char* p        = NULL;
    HINTERNET inet       = NULL,
              connection = NULL,
              request    = NULL;


    // Check sending msg and host.
    if (!msg) {
        lastErrorCode = ERR_NETWORK_INIT;
        sprintf(lastErrorMsg, "TransportAgent::sendMessage error: NULL message.");
        goto exit;
    }
    if (!(url.host) || strlen(url.host) == 0) {
        lastErrorCode = ERR_HOST_NOT_FOUND;
        sprintf(lastErrorMsg, "TransportAgent::sendMessage error: %s.", ERRMSG_HOST_NOT_FOUND);
        goto exit;
    }


    DWORD size  = 0,
          read  = 0,
          flags = INTERNET_FLAG_RELOAD | 
                  INTERNET_FLAG_NO_CACHE_WRITE | 
                  INTERNET_FLAG_KEEP_CONNECTION |           // This is necessary if authentication is required.
                  INTERNET_FLAG_NO_COOKIES;                 // This is used to avoid possible server errors on successive sessions.

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
	WCHAR* ua = toWideChar(userAgent);
    inet = InternetOpen (ua, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
	if (ua) {delete [] ua; ua = NULL; }

    if (!inet) {
        lastErrorCode = ERR_NETWORK_INIT;
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        sprintf (lastErrorMsg, "InternetOpen Error: %d - %s", code, tmp);
		delete [] tmp;
        goto exit;
    }   
    LOG.debug("Connecting to %s:%d", url.host, url.port);


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
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        sprintf (lastErrorMsg, "InternetConnect Error: %d - %s", code, tmp);
        delete [] tmp;
        goto exit;
    }
    LOG.debug("Requesting resource %s", url.resource);

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
        DWORD code = GetLastError();
        char* tmp = createHttpErrorMessage(code);
        sprintf (lastErrorMsg, "HttpOpenRequest Error: %d - %s", code, tmp);
        delete [] tmp;
        goto exit;
    }


    //
    // Prepares headers
    //
    WCHAR headers[512];

    contentLength = strlen(msg);
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);


    //
    // Try 5 times to send http request: used to retry sending request in case
    // of authentication (proxy/server).
    //
    for (int i=0;; i++) {

        // Fire Send Data Begin Transport Event
        fireTransportEvent(contentLength, SEND_DATA_BEGIN);

        // Send a request to the HTTP server.
        if (!HttpSendRequest (request, headers, wcslen(headers), (void*)msg, contentLength)) {
            DWORD code = GetLastError();

            if (code == ERROR_INTERNET_CONNECTION_RESET) {
                // Fire Send Data Begin Transport Event
                fireTransportEvent(contentLength, SEND_DATA_BEGIN);

                // Retry: some proxy need to resend the http request.
                if (!HttpSendRequest (request, headers, wcslen(headers), (void*)msg, contentLength)) {
                    lastErrorCode = ERR_CONNECT;
                    char* tmp = createHttpErrorMessage(code);
                    sprintf (lastErrorMsg, "HttpSendRequest Error: %d - %s", code, tmp);
                    delete [] tmp;
                    goto exit;
                }
            }
            // This is another type of error (e.g. cannot find server) -> exit
            else {
                lastErrorCode = ERR_CONNECT;
                char* tmp = createHttpErrorMessage(code);
                sprintf (lastErrorMsg, "HttpSendRequest Error: %d - %s", code, tmp);
                delete [] tmp;
                goto exit;
            }
        }

        LOG.debug(MESSAGE_SENT);
        // Fire Send Data End Transport Event
        fireTransportEvent(contentLength, SEND_DATA_END);

        // Check the status code.
        size = sizeof(status);
        HttpQueryInfo (request,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       (LPDWORD)&status,
                       (LPDWORD)&size,
                       NULL);

        //
        // OK (200) -> OUT
        //
        if (status == HTTP_STATUS_OK) {
            break;
        }

        //
        // 5th error -> OUT
        //
        if (i == MAX_AUTH_ATTEMPT) {
            LOG.error("HTTP Authentication failed: bad username or password.");
            break;
        }


        //
        // Proxy Authentication Required (407) / Server Authentication Required (401).
        // Need to set username/password.
        //
        else if(status == HTTP_STATUS_PROXY_AUTH_REQ ||
                status == HTTP_STATUS_DENIED) {
            LOG.debug("HTTP Authentication required.");
            DWORD dwError;
            
            // Automatic authentication (user/pass stored in win reg key).
            if (strcmp(proxy.user, "") && strcmp(proxy.password, "")) {
                WCHAR* wUser = toWideChar(proxy.user);
                WCHAR* wPwd  = toWideChar(proxy.password);

                InternetSetOption(request, INTERNET_OPTION_PROXY_USERNAME, wUser, wcslen(wUser)+1);
                InternetSetOption(request, INTERNET_OPTION_PROXY_PASSWORD, wPwd,  wcslen(wPwd)+1);

                delete [] wUser;
                delete [] wPwd;
                dwError = ERROR_INTERNET_FORCE_RETRY;
            }

            // Prompt dialog box.
            else {
                dwError = InternetErrorDlg(GetDesktopWindow(), request, NULL, 
                                           FLAGS_ERROR_UI_FILTER_FOR_ERRORS | 
                                           FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS |
                                           FLAGS_ERROR_UI_FLAGS_GENERATE_DATA,
                                           NULL);
            }
            
            if (dwError == ERROR_INTERNET_FORCE_RETRY) {
                continue;
            }
            else {
                LOG.error("HTTP Authentication failed.");
                break;
            }
        }
        
        //
        // Other HTTP errors -> OUT
        //
        else {
            break;
        }
    }

    // If wrong status, exit immediately.
    if (status != HTTP_STATUS_OK) {
        lastErrorCode = ERR_HTTP;
        sprintf(lastErrorMsg, "HTTP request error: %d", status);
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
    LOG.debug("Content-length: %d", contentLength);

    if (contentLength <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        sprintf(lastErrorMsg, "Invalid content-length: %d", contentLength);
		LOG.error(lastErrorMsg);
        goto exit;
    }

    // Fire Data Received Transport Event
    fireTransportEvent(contentLength, RECEIVE_DATA_BEGIN);

    // Allocate a block of memory for lpHeadersW.
    response = new char[contentLength+1];
	p = response;
    (*p) = 0;
    do {
        if (!InternetReadFile (request, (LPVOID)bufferA, 5000, &read)) {
            DWORD code = GetLastError();
            lastErrorCode = ERR_READING_CONTENT;
            char* tmp = createHttpErrorMessage(code);
            sprintf(lastErrorMsg, "InternetReadFile Error: %d - %s", code, tmp);
			delete [] tmp;
            goto exit;
        }

        // Sanity check: some proxy could send additional bytes...
        if ( (strlen(response) + read) > contentLength) {
            // Correct 'read' value to be sure we won't overflow the 'rensponse' buffer.
            read = contentLength - strlen(response);
        }

        if (read > 0) {
            bufferA[read] = 0;
            strcpy(p, bufferA);         // here copy also the last '0' byte -> strlen(rensponse) make sense.
            p += strlen(bufferA);

            // Fire Data Received Transport Event
            fireTransportEvent(read, DATA_RECEIVED);
        }

    } while (read);

    // Fire Receive Data End Transport Event
    fireTransportEvent(contentLength, RECEIVE_DATA_END);
    LOG.debug("Response read");
	LOG.debug("%s", response); 


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


// Utility function to retrieve the correspondant message for the Wininet error code passed.
// Pointer returned is allocated new, must be freed by caller.
char* Win32TransportAgent::createHttpErrorMessage(DWORD errorCode) {
    
    char* errorMessage = new char[512];
    memset(errorMessage, 0, 512);

    FormatMessageA(
                FORMAT_MESSAGE_FROM_HMODULE,
                GetModuleHandleA("wininet.dll"),
                errorCode,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
                errorMessage, 
                512, 
                NULL);

    if (!errorMessage || strlen(errorMessage) == 0) {
        sprintf(errorMessage, "Unknown error.");
    }
    return errorMessage;
}


