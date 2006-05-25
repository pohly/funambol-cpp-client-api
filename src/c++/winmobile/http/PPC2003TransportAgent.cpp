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


#define TIME  1000
#define TIMEOUT 10000
#define SEND_TIMEOUT 20000
#define ERR_HTTP_TIME_OUT 2007


typedef struct
{
    wchar_t* pHost;
    INTERNET_PORT nServerPort;  

} PARM_INTERNET_CONNECT;

typedef struct
{
    wchar_t* pResource;
    BOOL     secure;
} PARM_HTTP_OPEN_REQUEST;

typedef struct
{
    wchar_t* pHeaders;
    int      headersLength;
    char*    pMsg;
    int      msgLength;   
} PARM_HTTP_SEND_REQUEST;

#if 0
// This code is not used at the moment.
DWORD WINAPI WorkerFunctionInternetConnect( IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionHttpOpenRequest( IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionHttpSendRequest( IN PARM_HTTP_SEND_REQUEST vThreadParm);
#endif

int sumRead, previousNumRead;
int sumByteSent, previousNumWrite;
BCHAR* response;
BOOL cont;

#define ENTERING(func) //bsprintf(logmsg, L"Entering %s", func); LOG.debug(logmsg)
#define EXITING(func)  //bsprintf(logmsg, L"Exiting %s", func);  LOG.debug(logmsg)

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
    ENTERING(T("PPC2003TransportAgent::PPC2003TransportAgent"));
    
    url   = newURL  ;
    
    if (maxResponseTimeout == 0) {
        setTimeout(DEFAULT_MAX_TIMEOUT);
    } else {
        setTimeout(maxResponseTimeout);
    }

    // used by default. check connection before...
    if (!EstablishConnection()) {
//        lastErrorCode = ERR_INTERNET_CONNECTION_MISSING;
//        bsprintf(lastErrorMsg, T("%s: %d"),
//                 T("Internet Connection Missing"),
//                 ERR_INTERNET_CONNECTION_MISSING);
        LOG.error("Warning: internet connection missing.");
    }
    
    EXITING(T("PPC2003TransportAgent::PPC2003TransportAgent"));
}


PPC2003TransportAgent::~PPC2003TransportAgent(){}


/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */
BCHAR* PPC2003TransportAgent::sendMessage(const BCHAR* msg) {
    
    ENTERING(L"TransportAgent::sendMessage");
    int status        = -1;
    int contentLength = 0, contentLengthResponse = 0;
    DWORD compare     = 0;    
    DWORD size = 0, read = 0;          
    DWORD    dwTimeout = 10000; 
    char* bufferA = new char[readBufferSize+1];
    memset(bufferA, 0, readBufferSize+1 * sizeof(char));
    
    cont    = TRUE;
    int t   = 0;            
    unsigned int m = 0;
    BOOL queryInfo = TRUE;

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;

	LPCWSTR acceptTypes[2] = {TEXT("*/*"), NULL};
    

    // Set flags for secure connection (https).
    if (url.isSecure()) {
        flags = flags
              | INTERNET_FLAG_SECURE
              | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
              | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
              ;
    }
    wchar_t* ua = toWideChar(userAgent);
    inet = InternetOpen (ua, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);    
    if (ua) {delete [] ua; ua = NULL; }
    
    if (!inet) {
        lastErrorCode = ERR_NETWORK_INIT;
        bsprintf (lastErrorMsg, T("%s: %d"), T("InternetOpen Error"), GetLastError());
        goto exit;
    }   
    
    bsprintf(logmsg, T("Connecting to %s:%d"), url.host, url.port);
    LOG.debug(logmsg);
    
    // Open an HTTP session for a specified site by using lpszServer.
    
    wchar_t* wurlHost = toWideChar(url.host);
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
    wchar_t* wurlResource = toWideChar(url.resource);
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
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"),
                      SYNCML_CONTENT_TYPE, contentLength);
    
    
#if 0
    // Send a request to the HTTP server.
    PARM_HTTP_SEND_REQUEST     threadParmHttpSendRequest;    

    threadParmHttpSendRequest.pHeaders       = headers;        
    threadParmHttpSendRequest.headersLength  = wcslen(headers);
    threadParmHttpSendRequest.pMsg           = (char *)msg;
    threadParmHttpSendRequest.msgLength      = contentLength;
    
    if (WorkerFunctionHttpSendRequest(threadParmHttpSendRequest)) {
        bsprintf (lastErrorMsg, T("%s: %d"), T("HttpOpenRequest Error"), GetLastError());
	    LOG.error(lastErrorMsg);
        goto exit;
    }
#endif

    // Send a request to the HTTP server.
    if (!HttpSendRequest (request, headers, wcslen(headers), (void*)msg, contentLength)) {
        lastErrorCode = ERR_CONNECT;
        bsprintf (lastErrorMsg, T("%s: %d"), T("HttpSendRequest Error"), GetLastError());
		LOG.error(lastErrorMsg);
        goto exit;
    }
    LOG.debug(MESSAGE_SENT);
    //
    // restore cont variable for the InternetReadFile
    //
    cont = TRUE;
            
    //
    // If status code is not OK, returns immediately, otherwise reads the response
    //
    size = sizeof(status);
    queryInfo = HttpQueryInfo (request,
                   HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                   (LPDWORD)&status,
                   (LPDWORD)&size,
                   NULL);       

    if (status != STATUS_OK) {
        lastErrorCode = ERR_HTTP;
        bsprintf(lastErrorMsg, T("HTTP request error: %d"), status);
        goto exit;
    }
    
    queryInfo = HttpQueryInfo (request,
                       HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                       (LPDWORD)&contentLengthResponse,
                       (LPDWORD)&size,
                       NULL);

    LOG.debug(READING_RESPONSE);
    bsprintf(logmsg, T("Content-length: %d"), contentLengthResponse);
    LOG.debug(logmsg);
    
    if (contentLengthResponse <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        bsprintf(lastErrorMsg, T("Invalid content-length: %d"), contentLengthResponse);
        goto exit;
    }   
    
    // Allocate a block of memory for lpHeadersW.
    response = new BCHAR[contentLengthResponse+1];   
    if (response == NULL) {
        lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        bsprintf(lastErrorMsg, T("Not enough memory to allocate a buffer for the server response: %d required"), contentLengthResponse);
        goto exit;        

    }
    
    BCHAR* p        = NULL;  
	p = response;
    (*p) = 0;

    int recsize = 0;

    do {
        if (!InternetReadFile (request, (LPVOID)bufferA, readBufferSize, &read)) {
            lastErrorCode = ERR_READING_CONTENT;
            bsprintf(lastErrorMsg, T("%s: %d"), T("InternetReadFile Error"), GetLastError());
			LOG.error(lastErrorMsg);
            goto exit;
        }

        if (read != 0) {
            recsize += read;
            if(recsize > contentLengthResponse) {
                lastErrorCode = ERR_READING_CONTENT;
                bsprintf(lastErrorMsg, "Message size greater than content-lenght.");
                LOG.debug(lastErrorMsg);
                goto exit;
            }
            
            LOG.debug("Size: %d", recsize);

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
    
    if (lastErrorCode != 0 && lastErrorCode == ERR_HTTP_TIME_OUT) {
        delete [] response; response = NULL;
    }

    if (bufferA) {
        delete [] bufferA; bufferA = NULL;
    }
    EXITING(L"TransportAgent::sendMessage");
    return response;
}


#if 0
 /*
* The function try to read the content of a file with InternetReadFile . It was called by a thread in the main
* procedure. 
*/
DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm) {
    ENTERING(L"WorkerFunctionInternetReadFile");
    BCHAR* p        = NULL;
    p = response;
    (*p) = 0;
    char bufferA[5000+1];
    DWORD size = 0, read = 0;
          
    do {
        sumRead += read;
        if (!InternetReadFile (request, (LPVOID)bufferA, 5000, &read)) {
            lastErrorCode = ERR_READING_CONTENT;
            bsprintf(lastErrorMsg, T("%s: %d"), T("InternetReadFile Error"), GetLastError());
            break;
        }        
                
        if (read != 0) {
            bufferA[read] = 0;

            bstrcpy(p, bufferA);
            p += strlen(bufferA);
        }
        
    } while (read);
    cont = FALSE;
    EXITING(L"WorkerFunctionInternetReadFile");
    return 0;
}


/*
* The function try to send a message with HttpSendRequest . It was called by a thread in the main
* procedure. 
*/
DWORD WINAPI WorkerFunctionHttpSendRequest(IN PARM_HTTP_SEND_REQUEST vThreadParm) {
    ENTERING(L"WorkerFunctionHttpSendRequest");
    int status        = -1;   
    DWORD size        = 0; 
    INTERNET_BUFFERS BufferIn;
    BOOL queryInfo;
    DWORD dwBytesWritten = 0;
    char  pBuffer[4096 + 1];
    memset(pBuffer, 0, 4096);
    char* ptrBuffer = NULL;   

    sumByteSent = 0;

    PARM_HTTP_SEND_REQUEST pThreadParm;
    
    pThreadParm = (PARM_HTTP_SEND_REQUEST)vThreadParm;
        
    BufferIn.dwStructSize       = sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
    BufferIn.Next               = NULL; 
    BufferIn.lpcszHeader        = pThreadParm.pHeaders;
    BufferIn.dwHeadersLength    = pThreadParm.headersLength;
    BufferIn.dwHeadersTotal     = pThreadParm.headersLength;
    BufferIn.lpvBuffer          = NULL;                
    BufferIn.dwBufferLength     = 0;
    BufferIn.dwBufferTotal      = pThreadParm.msgLength; // This is the only member used other than dwStructSize
    BufferIn.dwOffsetLow        = 0;
    BufferIn.dwOffsetHigh       = 0;
    
    if(!HttpSendRequestEx( request, &BufferIn, NULL, 0, 0)) {   
        lastErrorCode = ERR_CONNECT;        
        bsprintf (lastErrorMsg, T("%s: %d"), T("Error on HttpSendRequestEx"),GetLastError());
        LOG.debug(lastErrorMsg);     
        return ERR_CONNECT;
    }        

    ptrBuffer = pThreadParm.pMsg;
    
    do {
        
        strncpy (pBuffer, ptrBuffer, 4096);
        if (!InternetWriteFile (request, pBuffer, strlen(pBuffer), &dwBytesWritten)) {
            lastErrorCode = 8001;
            bsprintf(lastErrorMsg, T("%s: %d"), T("InternetWriteFile Error"), GetLastError());    
            return lastErrorCode;
        } else {          
           LOG.debug(T("InternetWriteFile success..."));
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
            BCHAR dbg[200];
            bsprintf(dbg, T("Status: %d"), status);
            LOG.debug(dbg);
        }

        sumByteSent += dwBytesWritten;
        ptrBuffer = ptrBuffer + (dwBytesWritten * (sizeof(char)));

        if (sumByteSent < pThreadParm.msgLength) {
            
        } else {
            
            break;
        }

    } while (1);
    
    cont = FALSE;    
    
    if(!HttpEndRequest(request, NULL, 0, 0))
    {
        LOG.debug(T("Error in httpEndRequest"));
        bsprintf(lastErrorMsg, T("%s: %d"), T("InternetWriteFile Error"), GetLastError());    
        return lastErrorCode; 
       
    }
    EXITING(L"WorkerFunctionHttpSendRequest");
    return 0;

}
#endif

