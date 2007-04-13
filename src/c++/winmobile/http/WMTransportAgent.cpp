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
 How to test SSL connections 
 ----------------------------

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

 On the emulator:
 1) Copy myroot.cer in a device/emulator directory
 2) Click on it to import the certificate as a trusted CA

**/

#include "base/Log.h"
#include "base/messages.h"
#include "base/util/utils.h"
#include "http/constants.h"
#include "http/errors.h"
#include "http/WMTransportAgent.h"

#include "http/GPRSConnection.h"
#include "spdm/spdmutils.h"
#include "event/FireEvent.h"

#define ENTERING(func) LOG.debug("Entering %ls", func);
#define EXITING(func)  LOG.debug("Exiting %ls", func);


#ifdef USE_ZLIB
#include "zlib.h"
#endif

/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
WMTransportAgent::WMTransportAgent(
                    URL& newURL, Proxy& newProxy, 
                    unsigned int maxResponseTimeout, 
                    unsigned int maxmsgsize)
            // Use base class constructor to initialize common attributes
            : TransportAgent(newURL,
                             newProxy,
                             maxResponseTimeout,
                             maxmsgsize)
{

#ifdef USE_ZLIB
    isToDeflate = FALSE;
    isToInflate = FALSE;
    isFirstMessage = TRUE;    
#endif

    // used by default. check connection before...
    if (!EstablishConnection()) {
#ifdef WIN32_PLATFORM_PSPC
        lastErrorCode = ERR_INTERNET_CONNECTION_MISSING;
        sprintf(lastErrorMsg, "%s: %d",
                 "Internet Connection Missing",
                 ERR_INTERNET_CONNECTION_MISSING);
#else
        LOG.error("Warning: internet connection missing.");
#endif
    }
}

WMTransportAgent::~WMTransportAgent(){}



/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 */
char* WMTransportAgent::sendMessage(const char* msg) {

    ENTERING(L"TransportAgent::sendMessage");
    int status = -1;
    unsigned int contentLength = 0;
    WCHAR* wurlHost;
    WCHAR* wurlResource;
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
                  INTERNET_FLAG_KEEP_CONNECTION |     // This is necessary if authentication is required.
                  INTERNET_FLAG_NO_COOKIES;           // This is used to avoid possible server errors on successive sessions.

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
    inet = InternetOpen(ua, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
    if (ua) {delete [] ua; ua = NULL; }

    if (!inet) {
        lastErrorCode = ERR_NETWORK_INIT;
        sprintf (lastErrorMsg, "%s: %d", "InternetOpen Error", GetLastError());
        LOG.error(lastErrorMsg);
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
        sprintf (lastErrorMsg, "%s: %d", "InternetConnect Error", GetLastError());
        LOG.error(lastErrorMsg);
        goto exit;
    }

    
    LOG.debug("Requesting resource %s", url.resource);

    //
    // Open an HTTP request handle.
	//
    wurlResource = toWideChar(url.resource);
    if (!(request = HttpOpenRequest(connection,
                                    METHOD_POST,
                                    wurlResource,
                                    HTTP_VERSION,
                                    NULL,
                                    acceptTypes,
                                    flags, 0))) {
        lastErrorCode = ERR_CONNECT;
        sprintf (lastErrorMsg, "%s: %d", "HttpOpenRequest Error", GetLastError());
        LOG.error(lastErrorMsg);
        goto exit;
    }


    //
    // Prepares headers
    //
    WCHAR headers[512];
    contentLength = strlen(msg);

    // Msg to send is the original msg by default.
    // If compression is enabled, it will be switched to
    // compr. We don't want to touch this pointer, so
    // it's const (msg is also const).
    const void* msgToSend = (const void*)msg;

#ifdef USE_ZLIB
    // This is the locally allocated buffer for the compressed message.
    // Must be deleted after send.
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
        Bytef* compr = new Bytef[contentLength];
        
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
        msgToSend = (const void*)compr;
        int uncomprLenght = contentLength;
        contentLength = comprLen;

        wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d\r\nAccept-Encoding: deflate\r\nUncompressed-Content-Length: %d\r\nContent-Encoding: deflate"),
                          SYNCML_CONTENT_TYPE, contentLength, uncomprLenght);                                    
    }
#else
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);
#endif

    // Set response timeout
    DWORD respTOut = getTimeout()*1000;
    InternetSetOption(request, INTERNET_OPTION_RECEIVE_TIMEOUT, &respTOut, sizeof(DWORD));

    //
    // Try MAX_RETRIES times to send http request, in case of network errors
    //
    for (int numretries = 0; numretries < MAX_RETRIES; numretries++) {

        // Fire Send Data Begin Transport Event
        fireTransportEvent(contentLength, SEND_DATA_BEGIN);

        // Send a request to the HTTP server.
        if (!HttpSendRequest(request, headers, wcslen(headers),
                             (LPVOID)msgToSend, contentLength)) {
            DWORD code = GetLastError();

            if (code == 12002) {
                lastErrorCode = ERR_HTTP_TIME_OUT;
                sprintf(lastErrorMsg, "Network error: the request has timed out. %d", code);
                LOG.debug(lastErrorMsg);
                goto exit;
            }
            else if (code == 12019) {
                lastErrorCode = ERR_HTTP_TIME_OUT;
                sprintf(lastErrorMsg, "Network error: the handle supplied is not in the correct state. %d", code);
                LOG.debug(lastErrorMsg);
                goto exit;
            }
            else if (code == ERROR_INTERNET_CANNOT_CONNECT) { // 12029
                lastErrorCode = ERROR_INTERNET_CANNOT_CONNECT;
                sprintf(lastErrorMsg, "Network error: the attempt to connect to the server failed. %d", code);
                LOG.debug(lastErrorMsg);
                goto exit;
            }
            LOG.info("Network error (%d) writing data from client: retry %i time...",
                     code, numretries + 1);
           
            continue;
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

        if (status == HTTP_ERROR) { // 400 bad request error. retry to send the message
            LOG.info("Network error in server receiving data. "
                     "Server responds 400: retry %i time...", numretries + 1);
        }        
        else if (status == HTTP_STATUS_SERVER_ERROR ) {
            lastErrorCode = ERR_SERVER_ERROR;
            sprintf(lastErrorMsg, "HTTP server error: %d. Server failure.", status);
            LOG.debug(lastErrorMsg);
            goto exit;   
        }
        
        // to handle the http error code for the tcp/ip notification with wrong credential
        else if (status == ERR_CREDENTIAL) {  // 401
            lastErrorCode = ERR_CREDENTIAL;
            sprintf(lastErrorMsg, "HTTP server error: %d. Wrong credential.", status);
            LOG.debug(lastErrorMsg);
            goto exit;   
        }
        // to handle the http error code for the tcp/ip notification and client not notifiable
        else if (status == ERR_CLIENT_NOT_NOTIFIABLE) { // 420
            lastErrorCode = ERR_CLIENT_NOT_NOTIFIABLE;
            sprintf(lastErrorMsg, "HTTP server error: %d. Client not notifiable.", status);
            LOG.debug(lastErrorMsg);
            goto exit;   
        }
        else if (status != STATUS_OK) {
            lastErrorCode = ERR_HTTP_STATUS_NOT_OK;
            //lastErrorCode = ERR_READING_CONTENT;
            sprintf(lastErrorMsg, "HTTP request error: %d", status);
            LOG.debug(lastErrorMsg);
            goto exit;
        }
        else { //status_ok
            LOG.debug("Data sent succesfully to server. Server responds OK");
            break;
        }
    }

    // If wrong status, exit immediately.
    if (status != HTTP_STATUS_OK) {
        if (status == HTTP_STATUS_NOT_FOUND) {
            lastErrorCode = ERR_HTTP_NOT_FOUND;
            sprintf(lastErrorMsg, "HTTP request error: resource not found (status %d)", status);
        }
        else {
            DWORD code = GetLastError();
            lastErrorCode = ERR_HTTP_STATUS_NOT_OK;
            sprintf(lastErrorMsg, "HTTP request error (status received = %d): code %d", status, code);
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
    // Release the send buffer (also set msgToSend to NULL, to 
    // avoid leaving a dangling pointer around.
    if (compr) {
        delete [] compr; compr = NULL;
        msgToSend = NULL;
    }

    //
    // Read headers: get contentLenght/Uncompressed-Content-Length.
    //
    long uncompressedContentLenght = 0;
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
                   
    wcscpy(buffer, TEXT("Accept-Encoding"));
    HttpQueryInfo(request,HTTP_QUERY_CUSTOM,(LPVOID)buffer,&dwSize,NULL);
    if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
        isToDeflate = FALSE;
    } else {
        isToDeflate = TRUE;
    }	
    
    wcscpy(buffer, TEXT("Content-Encoding"));
    HttpQueryInfo(request,HTTP_QUERY_CUSTOM,(LPVOID)buffer,&dwSize,NULL);
    if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
        isToInflate = FALSE;
    } else {
        if (wcscmp(buffer, TEXT("deflate")) == 0)
            isToInflate = TRUE;
        else
            isToInflate = FALSE;
    }

    if(isToInflate) {
        wcscpy(buffer, TEXT("Uncompressed-Content-Length"));
        HttpQueryInfo(request,HTTP_QUERY_CUSTOM,(LPVOID)buffer,&dwSize,NULL);
        if (GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND) {
            LOG.error("Error reading 'Uncompressed-Content-Length' header. "
                      "Can't inflate data.");
            status = ERR_HTTP_INFLATE;
            lastErrorCode = ERR_HTTP_INFLATE;
            goto exit;

        } else {
            uncompressedContentLenght = wcstol(buffer, NULL, 10);
            LOG.debug("Uncompressed-Content-Length: %ld", uncompressedContentLenght);
            if(uncompressedContentLenght < 0) {
                LOG.error("Invalid 'Uncompressed-Content-Length' header. "
                          "Can't inflate data.");
                status = ERR_HTTP_INFLATE;
                lastErrorCode = ERR_HTTP_INFLATE;
                goto exit;
            }
        }
    }

    delete [] buffer;
    buffer = NULL;
#endif


// ================================== Reading Response ==============================
    LOG.debug(READING_RESPONSE);
    LOG.debug("Content-length: %u", contentLength);
	
    if (contentLength <= 0) {
        LOG.debug("Undefined content-length = %u. Using the maxMsgSize = %u.", contentLength, maxmsgsize);
        contentLength = maxmsgsize;
    }

    response = new char[contentLength+1];
    if (response == NULL) {
        lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        sprintf(lastErrorMsg, "Not enough memory to allocate a buffer for the server response: %d required.", contentLength);
        LOG.error(lastErrorMsg);
        goto exit;
    }
    memset(response, 0, contentLength);

    char* p = response;
    int realResponseLenght = 0;
    // Allocate a block of memory for response read.
    const unsigned int bufsize = getReadBufferSize();
    char* bufferA = new char[bufsize+1];

    // Fire Data Received Transport Event.
    fireTransportEvent(contentLength, RECEIVE_DATA_BEGIN);


    do {
        if (!InternetReadFile(request, (LPVOID)bufferA, bufsize, &read)) {
            DWORD code = GetLastError();
            lastErrorCode = ERR_READING_CONTENT;
            sprintf(lastErrorMsg, "InternetReadFile Error: %d", code);
            goto exit;
	}

        // Sanity check: some proxy could send additional bytes.
        // Correct 'read' value to be sure we won't overflow the 'response' buffer.
        if ((realResponseLenght + read) > contentLength) {
            LOG.debug("Warning! %d bytes read -> truncating data to content-lenght = %d.", (realResponseLenght + read), contentLength);
            read = contentLength - realResponseLenght;
        }

        if (read > 0) {
            memcpy(p, bufferA, read);    // Note: memcopy exactly the bytes read (could be no readable chars...)
            p += read;
            realResponseLenght += read;

            // Fire Data Received Transport Event
            fireTransportEvent(read, DATA_RECEIVED);
        }

    } while (read);

    // free read buffer
    delete [] bufferA; bufferA = 0;

    if (realResponseLenght <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        sprintf(lastErrorMsg, "Error reading HTTP response from Server: received data of size = %d.", realResponseLenght);
        goto exit;
    }

    // Should be already the same...
    contentLength = realResponseLenght;

    // Fire Receive Data End Transport Event
    fireTransportEvent(contentLength, RECEIVE_DATA_END);

    //------------------------------------------------------------- Response read

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
            status = ERR_HTTP_INFLATE;
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

    if ((status != STATUS_OK) && (response != NULL)) {
        delete [] response; response = NULL;
    }
    if (wurlHost)     delete [] wurlHost;
    if (wurlResource) delete [] wurlResource;
    if (bufferA)      delete [] bufferA;

#ifdef USE_ZLIB
    if (compr)        delete [] compr;
    if (buffer)       delete [] buffer;
    if (wbuffer)      delete [] wbuffer;
#endif

    EXITING(L"TransportAgent::sendMessage");

    return response;
}
