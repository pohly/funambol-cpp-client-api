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
#include "base/util/StringBuffer.h"

#define ENTERING(func) // LOG.debug("Entering %ls", func);
#define EXITING(func)  // LOG.debug("Exiting %ls", func);

#ifdef USE_ZLIB
    #include "zlib.h"
#endif

/**
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

     readBufferSize = DEFAULT_INTERNET_READ_BUFFER_SIZE;

     isToDeflate    = FALSE;
     isFirstMessage = TRUE;
     isToInflate    = FALSE;
}

Win32TransportAgent::~Win32TransportAgent(){}



/**
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 */
char* Win32TransportAgent::sendMessage(const char* msg) {

    ENTERING(L"TransportAgent::sendMessage");
    char* bufferA = new char[readBufferSize+1];
    int status = -1;
    unsigned int contentLength = 0;
    WCHAR* wurlHost;
    WCHAR* wurlResource;
    char* p        = NULL;
    char* response = NULL;
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

    // Msg to send is the original msg by default.
    void* msgToSend = (void*)msg;


#ifdef USE_ZLIB
    Bytef* compr = NULL;

    //
    // Say the client can accept the zipped content but the first message is clear
    //
    if (isFirstMessage || !isToDeflate) { 
        wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d\r\nAccept-Encoding: deflate"),
                          SYNCML_CONTENT_TYPE, contentLength);    
        isFirstMessage = false;
    } 
    else if (isToDeflate) {
        //
        // DEFLATE (compress data)
        //
        uLong comprLen = contentLength;
        compr = new Bytef[contentLength];

        // Compresses the source buffer into the destination buffer.
        int err = compress(compr, &comprLen, (Bytef*)msg, contentLength);
        if (err != Z_OK) {
            lastErrorCode = ERR_HTTP_DEFLATE;
            sprintf(lastErrorMsg, "ZLIB: error occurred compressing data.");
            delete [] compr;
            compr = NULL;
            goto exit;
        }            

        // Msg to send is the compressed data.
        msgToSend = (void*)compr;
        int uncomprLenght = contentLength;
        contentLength = comprLen;

        wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nUncompressed-Content-Length: %d\r\nContent-Encoding: deflate"),
                          SYNCML_CONTENT_TYPE, contentLength, uncomprLenght);                                    
    }
#endif


#ifndef USE_ZLIB
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);
#endif


    // Timeout to receive a rensponse from server (default = 5 min).
    DWORD timeoutMsec = timeout*1000;
    InternetSetOption(request, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeoutMsec, sizeof(DWORD));


    //
    // Try 5 times to send http request: used to retry sending request in case
    // of authentication (proxy/server).
    //
    for (int i=0;; i++) {

        // 5th error -> OUT
        if (i == MAX_AUTH_ATTEMPT) {
            lastErrorCode = ERR_HTTP;
            sprintf(lastErrorMsg, "HttpSendRequest error: %d attempts failed.", i);
            LOG.error(lastErrorMsg);
            goto exit;
        }

        //
        // Send a request to the HTTP server.
        //
        fireTransportEvent(contentLength, SEND_DATA_BEGIN);
        if (!HttpSendRequest (request, headers, wcslen(headers), msgToSend, contentLength)) {
            DWORD code = GetLastError();

            if (code == ERROR_INTERNET_OFFLINE_MODE) {
                // IExplorer in offline mode: retry.
                LOG.debug("Offline mode detected: retry sending request");
                WCHAR* wurl = toWideChar(url.fullURL);
                InternetGoOnline(wurl, NULL, NULL);
                delete [] wurl;
                continue;
            }
            else if (code == ERROR_INTERNET_CONNECTION_RESET) {
                // Some proxy need to resend the http request.
                LOG.debug("Error internet connection reset: retry sending request");

                fireTransportEvent(contentLength, SEND_DATA_BEGIN);
                if (!HttpSendRequest (request, headers, wcslen(headers), msgToSend, contentLength)) {
                    lastErrorCode = ERR_CONNECT;
                    char* tmp = createHttpErrorMessage(code);
                    sprintf (lastErrorMsg, "HttpSendRequest Error: %d - %s", code, tmp);
                    delete [] tmp;
                    goto exit;
                }
            }
            else {
                // This is another type of error (e.g. cannot find server) -> exit
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
        if (status == HTTP_STATUS_NOT_FOUND) {
            lastErrorCode = ERR_HTTP_NOT_FOUND;
            sprintf(lastErrorMsg, "HttpQueryInfo error: resource not found (status %d)", status);
        }
        else if (status == HTTP_STATUS_REQUEST_TIMEOUT) {
            lastErrorCode = ERR_HTTP_REQUEST_TIMEOUT;
            sprintf(lastErrorMsg, "HttpQueryInfo error: server timed out waiting for request (status %d)", status);
        }
        else {
            lastErrorCode = ERR_HTTP;
            DWORD code = GetLastError();
            char* tmp = createHttpErrorMessage(code);
            sprintf(lastErrorMsg, "HttpQueryInfo error (status received = %d): %s (code %d)", status, tmp, code);
		    delete [] tmp;
        }
        goto exit;
    }

    //Initialize response
    contentLength=0;
    HttpQueryInfo (request,
                   HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                   (LPDWORD)&contentLength,
                   (LPDWORD)&size,
                   NULL);



#ifdef USE_ZLIB 

    if (compr) {
        // Delete the compressed message sent.
        delete [] compr;
        compr = NULL;
    }

    //
    // Read headers: get contentLenght/Uncompressed-Content-Length.
    //
    int uncompressedContentLenght = 0;
    wchar_t* wbuffer = new wchar_t[1024];
    DWORD ddsize = 1024;
    if (!HttpQueryInfo(request,HTTP_QUERY_RAW_HEADERS_CRLF ,(LPVOID)wbuffer,&ddsize,NULL)) {
        if (ERROR_HTTP_HEADER_NOT_FOUND == GetLastError()) {
            isToDeflate = FALSE;
        }
    }
    LOG.debug("Header: %ls", wbuffer);
    delete [] wbuffer; wbuffer = NULL;
           
    // isToDeflate to be set
    DWORD dwSize = 512;
    wchar_t* buffer = new wchar_t[dwSize];     
    memset(buffer, 0, dwSize*sizeof(wchar_t));
                   
    wcscpy(buffer, TEXT("Accept-Encoding"));
    HttpQueryInfo(request, HTTP_QUERY_CUSTOM, (LPVOID)buffer, &dwSize, NULL);
    if (ERROR_HTTP_HEADER_NOT_FOUND == GetLastError()) {
        isToDeflate = FALSE;
    } else {
        isToDeflate = TRUE;
    }	
    
    memset(buffer, 0, dwSize*sizeof(wchar_t));
    wcscpy(buffer, TEXT("Content-Encoding"));
    HttpQueryInfo(request, HTTP_QUERY_CUSTOM, (LPVOID)buffer, &dwSize, NULL);
    if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
        isToInflate = FALSE;
    } else {
        if (wcscmp(buffer, TEXT("deflate")) == 0)
            isToInflate = TRUE;
        else
            isToInflate = FALSE;
    }

    memset(buffer, 0, dwSize*sizeof(wchar_t));
    wcscpy(buffer, TEXT("Uncompressed-Content-Length"));

    HttpQueryInfo(request, HTTP_QUERY_CUSTOM, (LPVOID)buffer, &dwSize, NULL);
    if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
        isToInflate = FALSE;
    } else {
        uncompressedContentLenght = wcstol(buffer, NULL, 10);
    }	        	

    delete [] buffer;
    buffer = NULL;
#endif


//
// ====================================== Reading Response ======================================
//
    LOG.debug(READING_RESPONSE);
    LOG.debug("Content-length: %d", contentLength);
	
    if (contentLength <= 0) {
        LOG.debug("Undefined content-length = %d. Using the maxMsgSize = %d.", contentLength, maxmsgsize);
        contentLength = maxmsgsize;
    }

    // Allocate a block of memory for response read.
    response = new char[contentLength+1];
    if (response == NULL) {
        lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        sprintf(lastErrorMsg, "Not enough memory to allocate a buffer for the server response: %d required.", contentLength);
        LOG.error(lastErrorMsg);
        goto exit;
    }
    memset(response, 0, contentLength);
	p = response;
    (*p) = 0;
    int realResponseLenght = 0;

    // Fire Data Received Transport Event.
    fireTransportEvent(contentLength, RECEIVE_DATA_BEGIN);

    do {
        if (!InternetReadFile(request, (LPVOID)bufferA, readBufferSize, &read)) {
            DWORD code = GetLastError();
            lastErrorCode = ERR_READING_CONTENT;
            char* tmp = createHttpErrorMessage(code);
            sprintf(lastErrorMsg, "InternetReadFile Error: %d - %s", code, tmp);
            delete [] tmp;
            goto exit;
		}

        // Sanity check: some proxy could send additional bytes.
        // Correct 'read' value to be sure we won't overflow the 'response' buffer.
        if ((realResponseLenght + read) > contentLength) {
            LOG.debug("Warning! %d bytes read -> truncating data to content-lenght = %d.", (realResponseLenght + read), contentLength);
            read = contentLength - realResponseLenght;
        }

        if (read > 0) {
            memcpy(p, bufferA, read);               // Note: memcopy exactly the bytes read (could be no readable chars...)
            p += read;
            realResponseLenght += read;

            // Fire Data Received Transport Event
            fireTransportEvent(read, DATA_RECEIVED);
        }

    } while (read);

    if (realResponseLenght <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        sprintf(lastErrorMsg, "Error reading HTTP response from Server: received data of size = %d.", realResponseLenght);
        goto exit;
    }

    // Should be already the same...
    contentLength = realResponseLenght;

    // Fire Receive Data End Transport Event
    fireTransportEvent(contentLength, RECEIVE_DATA_END);


#ifdef USE_ZLIB
    if (isToInflate) {
        //
        // INFLATE (decompress data)
        //
        uLong uncomprLen = uncompressedContentLenght;
        Bytef* uncompr = new Bytef[uncomprLen + 1];        

        // Decompresses the source buffer into the destination buffer.
        int err = uncompress(uncompr, &uncomprLen, (Bytef*)response, contentLength);

        if (err == Z_OK) {
            delete [] response;
            response = (char*)uncompr;
            response[uncompressedContentLenght] = 0;
        }   
        else if (err < 0) {
            delete [] response;
            response = NULL;
            lastErrorCode = ERR_HTTP_INFLATE;
            sprintf(lastErrorMsg, "ZLIB: error occurred decompressing data from Server.");
            goto exit;
        }
    }
#endif

    LOG.debug("Response read:\n%s", response);

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
    if (wurlHost)     delete [] wurlHost;
    if (wurlResource) delete [] wurlResource;
    if (bufferA)      delete [] bufferA;

    EXITING(L"TransportAgent::sendMessage");

    return response;
}


/**
 * Utility function to retrieve the correspondant message for the Wininet error code passed.
 * Pointer returned is allocated new, must be freed by caller.
 */
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


