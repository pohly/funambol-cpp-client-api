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

 How to test SSL connections on WindowsMobile 2003 device/emulator
 -----------------------------------------------------------------

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
#include "http/PPC2003TransportAgent.h"

#include "http/GPRSConnection.h"
#include "spdm/spdmutils.h"
#include "event/FireEvent.h"


#define TIME  500
#define TIMEOUT 300000  // 5 min
#define SEND_TIMEOUT 20000
#define ERR_HTTP_TIME_OUT 2007
#define MAX_SEND_RETRIES 3
#define MAX_RETRIES 2


typedef struct
{
    WCHAR* pHost;
    INTERNET_PORT nServerPort;

} PARM_INTERNET_CONNECT;

typedef struct
{
    WCHAR* pResource;
    BOOL     secure;
} PARM_HTTP_OPEN_REQUEST;

typedef struct
{
    WCHAR* pHeaders;
    int      headersLength;
    char*    pMsg;
    int      msgLength;
} PARM_HTTP_SEND_REQUEST;

#if 0
// This code is not used at the moment.
DWORD WINAPI WorkerFunctionInternetConnect( IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionHttpOpenRequest( IN LPVOID vThreadParm);
#endif

DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm);
DWORD WINAPI FunctionHttpSendRequest( IN PARM_HTTP_SEND_REQUEST vParm);

int sumRead, previousNumRead;
int sumByteSent, previousNumWrite;
BOOL cont;

#define ENTERING(func) sprintf(logmsg, T("Entering %s"), func); LOG.debug(logmsg)
#define EXITING(func)  sprintf(logmsg, T("Exiting %s"), func);  LOG.debug(logmsg)

BOOL UseHttpSendReqEx(HINTERNET hRequest, DWORD dwPostSize);
#define BUFFSIZE 500

HINTERNET inet       = NULL,
          connection = NULL,
          request    = NULL;
/*
 * This is the PPC 2003 implementation of the TransportAgent object
 */


/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
PPC2003TransportAgent::PPC2003TransportAgent(URL& newURL, Proxy& newProxy,
                                             unsigned int maxResponseTimeout,
                                             unsigned int maxmsgsize) : TransportAgent() {

    url   = newURL  ;

    if (maxResponseTimeout == 0) {
        setTimeout(DEFAULT_MAX_TIMEOUT);
    } else {
        setTimeout(maxResponseTimeout);
    }

    // used by default. check connection before...
    if (!EstablishConnection()) {
#ifdef WIN32_PLATFORM_PSPC
        lastErrorCode = ERR_INTERNET_CONNECTION_MISSING;
        sprintf(lastErrorMsg, T("%s: %d"),
                 T("Internet Connection Missing"),
                 ERR_INTERNET_CONNECTION_MISSING);
#else
        LOG.error("Warning: internet connection missing.");
#endif
    }

}


PPC2003TransportAgent::~PPC2003TransportAgent(){}


/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */

int bufferSize = 0; // copy the readBufferSize member value
char* bufferA = NULL;
char*  response = NULL;

char*  PPC2003TransportAgent::sendMessage(const char*  msg) {

    int status        = -1;
    int contentLength = 0, contentLengthResponse = 0;
    DWORD compare     = 0;
    DWORD size = 0, read = 0;
    DWORD    dwTimeout = 10000;
    //char* bufferA = NULL;
    //char*  response = NULL;

    response = NULL;
    bufferA = NULL;

    bufferA = new char[readBufferSize+1];
    memset(bufferA, 0, (readBufferSize+1) * sizeof(char));

    // for thread
    bufferSize = readBufferSize;

    HANDLE   hThread = NULL;
    DWORD    dwThreadID = 0;
    //

    BOOL containsFinalTag = FALSE;
    cont = TRUE;
    int t = 0, k = 0, numretries = 0;
    unsigned int m = 0;
    BOOL queryInfo = TRUE;
    int recsize = 0;
    char*  p        = NULL;
    WCHAR* wurlResource = NULL;
    WCHAR* wurlHost = NULL;
    DWORD r = 0;

    char*  toLog = NULL;

    inet       = NULL,
    connection = NULL,
    request    = NULL;

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES;

	LPCWSTR acceptTypes[2] = {TEXT("*/*"), NULL};

    // Set flags for secure connection (https).
    if (url.isSecure()) {
        flags = flags
              | INTERNET_FLAG_SECURE
              | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
              | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
              ;
    }
    WCHAR* ua = toWideChar(userAgent);
    inet = InternetOpen (ua, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);
    if (ua) {delete [] ua; ua = NULL; }

    if (!inet) {
        lastErrorCode = ERR_NETWORK_INIT;
        sprintf (lastErrorMsg, T("%s: %d"), T("InternetOpen Error"), GetLastError());
        LOG.error(lastErrorMsg);
        goto exit;
    }

    sprintf(logmsg, T("Connecting to %s:%d"), url.host, url.port);
    LOG.debug(logmsg);

    // Open an HTTP session for a specified site by using lpszServer.

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
      sprintf (lastErrorMsg, T("%s: %d"), T("InternetConnect Error"), GetLastError());
	  LOG.error(lastErrorMsg);
      goto exit;
    }
     if(wurlHost) { delete [] wurlHost; wurlHost = NULL;}

    sprintf(logmsg, T("Requesting resource %s"), url.resource);
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
      sprintf (lastErrorMsg, T("%s: %d"), T("HttpOpenRequest Error"), GetLastError());
	  LOG.error(lastErrorMsg);
      goto exit;
    }

    if(wurlResource) { delete [] wurlResource; wurlResource = NULL;}

    //
    // Prepares headers
    //
    WCHAR headers[512];

    contentLength = strlen(msg);
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"),
                      SYNCML_CONTENT_TYPE, contentLength);
    toLog = toMultibyte(headers);
    LOG.debug(toLog);
    if (toLog) { delete [] toLog; toLog = NULL; }

    if (strstr(msg, T("<Final")) != NULL)
        containsFinalTag = TRUE;

    // Send a request to the HTTP server.
    PARM_HTTP_SEND_REQUEST     threadParmHttpSendRequest;

    threadParmHttpSendRequest.pHeaders       = headers;
    threadParmHttpSendRequest.headersLength  = wcslen(headers);
    threadParmHttpSendRequest.pMsg           = (char *)msg;
    threadParmHttpSendRequest.msgLength      = contentLength;

    //
    //
    //
    for (numretries = 0; numretries < MAX_RETRIES; numretries++) {

        for (k = 0; k < MAX_SEND_RETRIES; k++) {

            // Fire Send Data Begin Transport Event
            fireTransportEvent(contentLength, SEND_DATA_BEGIN);

            r = FunctionHttpSendRequest(threadParmHttpSendRequest);
            if (r == 8001) {
                LOG.info("Network error in writing data from client: retry %i time...", k + 1);
            } else {
                LOG.debug(MESSAGE_SENT);

                // Fire Send Data End Transport Event
                fireTransportEvent(contentLength, SEND_DATA_END);

                size = sizeof(status);
                queryInfo = HttpQueryInfo (request,
                               HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                               (LPDWORD)&status,
                               (LPDWORD)&size,
                               NULL);

                if (status == HTTP_ERROR) { // 400 bad request error. retry to send the message
                    LOG.info("Network error in server reciving data. Server responds 400: retry %i time...", k + 1);
                } else if (status == 12002) {
                    lastErrorCode = ERR_HTTP_TIME_OUT;
                    sprintf(lastErrorMsg, T("Network error: the request has timed out. %d"), status);
                    LOG.debug(lastErrorMsg);
                    goto exit;
                } else if (status == 12019) {
                    lastErrorCode = ERR_HTTP_TIME_OUT;
                    sprintf(lastErrorMsg, T("Network error: the handle supplied is not in the correct state. %d"), status);
                    LOG.debug(lastErrorMsg);
                    goto exit;
                } else if (status != STATUS_OK) {
                    lastErrorCode = ERR_READING_CONTENT;
                    sprintf(lastErrorMsg, T("HTTP request error: %d"), status);
                    LOG.debug(lastErrorMsg);
                    goto exit;
                } else { //status_ok
                    LOG.debug("Data sent succesfully to server. Server responds OK");
                    break;
                }
            }
        }
        if (k == MAX_SEND_RETRIES) {
            lastErrorCode = ERR_HTTP_TIME_OUT;
            sprintf (lastErrorMsg, T("%s"), T("Network error: max number of send retries reached: exit"));
            LOG.error(lastErrorMsg);
            goto exit;

        }

        size = sizeof(status);
        queryInfo = HttpQueryInfo (request,
                           HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                           (LPDWORD)&contentLengthResponse,
                           (LPDWORD)&size,
                           NULL);

        LOG.debug(READING_RESPONSE);

        // Fire Data Received Transport Event
        fireTransportEvent(size, RECEIVE_DATA_BEGIN);

        sprintf(logmsg, T("Content-length: %d"), contentLengthResponse);
        LOG.debug(logmsg);

        if (contentLengthResponse <= 0) {
            lastErrorCode = ERR_READING_CONTENT;
            sprintf(lastErrorMsg, T("Invalid content-length: %d"), contentLengthResponse);
            LOG.error(lastErrorMsg);
            goto exit;
        }

        // Allocate a block of memory for lpHeadersW.
        response = new char[contentLengthResponse+1];
        memset (response, 0, (contentLengthResponse+1)*sizeof(char));
        if (response == NULL) {
            lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
            sprintf(lastErrorMsg, T("Not enough memory to allocate a buffer for the server response: %d required"), contentLengthResponse);
            LOG.error(lastErrorMsg);
            goto exit;

        }

        /*
	    p = response;
        (*p) = 0;
        do {
            if (!InternetReadFile (request, (LPVOID)bufferA, readBufferSize, &read)) {
                lastErrorCode = ERR_READING_CONTENT;
                sprintf(lastErrorMsg, T("%s: %d"), T("InternetReadFile Error"), GetLastError());
			    LOG.error(lastErrorMsg);
                goto exit;
            }

            if (read != 0) {
                recsize += read;
                if(recsize > contentLengthResponse) {
                    lastErrorCode = ERR_READING_CONTENT;
                    sprintf(lastErrorMsg, "Message size greater than content-lenght.");
                    LOG.debug(lastErrorMsg);
                    goto exit;
                }

                LOG.debug("Size: %d", recsize);

                bufferA[read] = 0;
                strcpy(p, bufferA);
                p += strlen(bufferA);
            }

        } while (read);
        */

        cont = TRUE;
        hThread = CreateThread(
                     NULL,                      // Pointer to thread security attributes
                     0,                         // Initial thread stack size, in bytes
                     WorkerFunctionInternetReadFile,    // Pointer to thread function
                     NULL,                      // The argument for the new thread
                     0,                         // Creation flags
                     &dwThreadID                // Pointer to returned thread identifier
                 );

        previousNumRead = -1;
        sumRead = 0;
        t = 0;
        do {
            if (previousNumRead == -1)
                LOG.debug("In loop read bytes. Start");
            else
                LOG.debug("In loop read bytes: %i ", previousNumRead);

            Sleep(TIME);
            if (previousNumRead == sumRead) {
                t += TIME;
                if (t > TIMEOUT) {

                    if (response) { delete [] response; response = NULL; }

                    lastErrorCode = ERR_HTTP_TIME_OUT;
                    sprintf(lastErrorMsg, T("Network error in reading response from server. Time out error: %d"), lastErrorCode);
                    if (!containsFinalTag) {
                        LOG.info(lastErrorMsg);
                        if (numretries + 1 != MAX_RETRIES) {
                            LOG.info("The client message doesn't contain the <Final> tag. Retry %i time...", numretries + 1);
                        } else {
                            LOG.info("The client message doesn't contain the <Final> tag but max retries number reached.");
                        }
                        break;
                    }
                    else {
                        LOG.info("The client message contains the <Final> tag. It is not possible to send again");
                        LOG.error(lastErrorMsg);
                        goto exit;
                    }

                }
            } else {
                previousNumRead = sumRead;
                t = 0;
            }

        } while(cont);

        // the read is succesfully excecuted. now it can exit... :)
        if(cont == FALSE) {
            LOG.debug("In loop read bytes. End.");
            break;
        }

    }
    if (numretries == MAX_RETRIES) {
        lastErrorCode = ERR_HTTP_TIME_OUT;
        sprintf (lastErrorMsg, T("%s"), T("Network error: max number of read retries reached: exit"));
        LOG.error(lastErrorMsg);
        goto exit;
    }

    // Fire Receive Data End Transport Event
    fireTransportEvent(size, RECEIVE_DATA_END);

    LOG.debug(T("Response read"));
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
        if (response)  {delete [] response; response = NULL;}
    }

    if (lastErrorCode != 0 && lastErrorCode == ERR_HTTP_TIME_OUT) {
        if (response)  {delete [] response; response = NULL;}
    }

    if (bufferA) {
        delete [] bufferA; bufferA = NULL;
    }
    if (wurlHost) { delete [] wurlHost; wurlHost = NULL;}
    if (wurlResource) { delete [] wurlResource; wurlResource = NULL;}

    if (hThread) {
        CloseHandle(hThread);
    }

   return response;
}



/*
* The function try to send a message with HttpSendRequest . It was called by a thread in the main
* procedure.
*/
DWORD WINAPI FunctionHttpSendRequest(IN PARM_HTTP_SEND_REQUEST vParm) {
    ENTERING(T("FunctionHttpSendRequest"));
    int status        = -1;
    DWORD size        = 0;
    INTERNET_BUFFERS BufferIn;
    BOOL queryInfo;
    DWORD dwBytesWritten = 0;
    char  pBuffer[1024 + 1];
    memset(pBuffer, 0, 1024+1);
    char* ptrBuffer = NULL;
    char dbg[300];
    sumByteSent = 0;
    DWORD ret = 0;

    PARM_HTTP_SEND_REQUEST pParm;

    pParm = (PARM_HTTP_SEND_REQUEST)vParm;

    BufferIn.dwStructSize       = sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
    BufferIn.Next               = NULL;
    BufferIn.lpcszHeader        = pParm.pHeaders;
    BufferIn.dwHeadersLength    = pParm.headersLength;
    BufferIn.dwHeadersTotal     = pParm.headersLength;
    BufferIn.lpvBuffer          = NULL;
    BufferIn.dwBufferLength     = 0;
    BufferIn.dwBufferTotal      = pParm.msgLength; // This is the only member used other than dwStructSize
    BufferIn.dwOffsetLow        = 0;
    BufferIn.dwOffsetHigh       = 0;

    if(!HttpSendRequestEx( request, &BufferIn, NULL, HSR_INITIATE, 0)) {
        ret = 8001;
        sprintf (dbg, T("%s: %d"), T("HttpSendRequestEx error"), GetLastError());
        LOG.debug(dbg);
        goto finally;
    }

    ptrBuffer = pParm.pMsg;

    do {
        strncpy (pBuffer, ptrBuffer, 1024);
        if (!InternetWriteFile (request, pBuffer, strlen(pBuffer), &dwBytesWritten)) {
            ret = 8001;
            sprintf(dbg, T("%s: %d"), T("InternetWriteFile error"), GetLastError());
            LOG.debug(dbg);
            goto finally;
        } else {
           //LOG.debug(T("InternetWriteFile success..."));
        }

        size = sizeof(status);
        queryInfo = HttpQueryInfo (request,
                   HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                   (LPDWORD)&status,
                   (LPDWORD)&size,
                   NULL);

        if( status == HTTP_SERVER_ERROR ) {
            LOG.error(T("Status: HTTP_SERVER_ERROR"));
            break;
        }
        else if( status == HTTP_UNAUTHORIZED ) {
            LOG.error(T("Status: HTTP_UNAUTHORIZED"));
            break;
        }
        else if( status == HTTP_ERROR ) {
            LOG.error(T("Status: HTTP_ERROR"));
            break;
        }
        else if( status == HTTP_NOT_FOUND ) {
            LOG.error(T("Status: HTTP_NOT_FOUND"));
            break;
        }
        else if( status == X_HTTP_420_IPPRV ) {
            LOG.error(T("Status: X_HTTP_420_IPPRV 420"));
            break;
        }
        else {
            if (status != -1) {
                sprintf(dbg, T("Status: %d"), status);
                LOG.debug(dbg);
            }
        }

        sumByteSent += dwBytesWritten;
        sprintf(dbg, "FunctionHttpSendRequest: total byte sent: %i", sumByteSent);
        LOG.debug(dbg);

        ptrBuffer = ptrBuffer + (dwBytesWritten * (sizeof(char)));

        if (sumByteSent < pParm.msgLength) {

        } else {

            break;
        }

    } while (1);

    cont = FALSE;

finally:

    LOG.debug("Now HttpEndRequest is called");
    if(!HttpEndRequest(request, NULL, 0, 0))
    {
        sprintf(dbg, T("%s: %d"), T("HttpEndRequest"), GetLastError());
        LOG.debug(dbg);
        ret = 8001;

    }
    EXITING(T("FunctionHttpSendRequest"));
    return ret;

}


 /*
* The function try to read the content of a file with InternetReadFile . It was called by a thread in the main
* procedure.
*/
DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm) {
    ENTERING(T("WorkerFunctionInternetReadFile"));
    char*  p        = NULL;
    p = response;
    (*p) = 0;
    DWORD size = 0, read = 0;
    int recsize = 0;

    do {
        sumRead += read;
        if (!InternetReadFile (request, (LPVOID)bufferA, bufferSize, &read)) {
            lastErrorCode = ERR_READING_CONTENT;
            sprintf(lastErrorMsg, T("%s: %d"), T("InternetReadFile Error"), GetLastError());
            LOG.debug(lastErrorMsg);
            break;
        }

        if (read != 0) {
            recsize += read;
            LOG.debug("Size: %d", recsize);
            bufferA[read] = 0;

            strcpy(p, bufferA);
            p += strlen(bufferA);

            // Fire Data Received Transport Event
            fireTransportEvent(read, DATA_RECEIVED);
        }

    } while (read);
    cont = FALSE;
    EXITING(T("WorkerFunctionInternetReadFile"));
    return 0;
}
