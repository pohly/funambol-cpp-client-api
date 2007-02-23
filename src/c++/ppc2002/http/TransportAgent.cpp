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


#include "common/Log.h"

#include "http/Constants.h"
#include "http/TransportAgent.h"

#define T  1000
#define TO 10000

DWORD WINAPI WorkerFunctionInternetConnect( IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionHttpOpenRequest( IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionHttpSendRequest( IN LPVOID vThreadParm);

typedef struct
{
    WCHAR* pHost;
    INTERNET_PORT nServerPort;  

} PARM_INTERNET_CONNECT;

typedef struct
{
    WCHAR* pResource;
    
} PARM_HTTP_OPEN_REQUEST;

typedef struct
{
    WCHAR* pHeaders;
    int      headersLength;
    char*    pMsg;
    int      msgLength;

    
} PARM_HTTP_SEND_REQUEST;


int sumRead, previousNumRead;
WCHAR* response;
BOOL cont;

HINTERNET inet       = NULL,
          connection = NULL,
          request    = NULL;
/*
 * This is the Pocket PC 2002 implementation of the TransportAgent object
 */


/*
 * This function translate a UNICODE string into a UTF string without
 * allocating additional memory. The translation is performed removing
 * the second byte of the UNICODE coding.
 *
 * @param s the string to translate
 */
void toUTF(WCHAR* s) {
    int l = wcslen(s);
    WCHAR* w = s;
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
TransportAgent::TransportAgent(URL& newURL, Proxy& newProxy, BOOL useCheckConnection, unsigned int maxResponseTimeout) {
    url   = newURL  ;
    
    if (maxResponseTimeout == 0) {
        setResponseTimeout(DEFAULT_MAX_TIMEOUT);
    } else {
        setResponseTimeout(maxResponseTimeout);
    }

    // the flag useCheckConnection is not used by this transportAgent for PPC 2002
        
}

TransportAgent::~TransportAgent(){}


/*
 * Change the URL the subsequent calls to setMessage() should
 * use as target url.
 *
 * @param url the new target url
 */
void TransportAgent::setURL(URL& newURL) {
    url = newURL;

}

void TransportAgent::setResponseTimeout(unsigned int resTimeout) {
    responseTimeout = resTimeout;
}

unsigned int TransportAgent::getResponseTimeout() {
    return responseTimeout;
}

/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */
WCHAR* TransportAgent::sendMessage(WCHAR* msg) {
    
    
    int status        = -1;
    int contentLength = 0;
    DWORD compare     = 0;    
    DWORD size = 0, read = 0;          
    DWORD    dwTimeout = 10000; 
    HANDLE   hThread;
    DWORD    dwThreadID;    
    DWORD    dwExitCode;
    cont    = TRUE;
    int t   = 0;            
    unsigned int m = 0;
    BOOL queryInfo = TRUE;


    inet = InternetOpen (TEXT(USER_AGENT), INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);    

    if (!inet) {
        lastErrorCode = ERR_TPCIP_INIT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("InternetOpen Error"), GetLastError());
        goto exit;
    }
    
    
    wsprintf(logmsg, TEXT("Connecting to %s:%d"), url.host, url.port);
    LOG.debug(logmsg);
    

    // Open an HTTP session for a specified site by using lpszServer.
    
    PARM_INTERNET_CONNECT    threadParm;
    threadParm.pHost       = url.host;
    threadParm.nServerPort = url.port;   

    hThread = CreateThread(
                 NULL,            // Pointer to thread security attributes
                 0,               // Initial thread stack size, in bytes
                 WorkerFunctionInternetConnect,  // Pointer to thread function
                 &threadParm,     // The argument for the new thread
                 0,               // Creation flags
                 &dwThreadID      // Pointer to returned thread identifier
             );
    
    
    
    compare = WaitForSingleObject ( hThread, dwTimeout );

    if (compare == WAIT_TIMEOUT ) {
        
        LOG.debug(TEXT("InternetConnect failed: timeout error."));     
        lastErrorCode = ERR_CONNECT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("InternetConnect timeout error"), GetLastError());
        goto exit ;

    } else if (compare == WAIT_OBJECT_0){
        LOG.debug(TEXT("InternetConnect success!!"));
    }

    // The state of the specified object (thread) is signaled
    dwExitCode = 0;

    if ( !GetExitCodeThread( hThread, &dwExitCode ) ) {       
        
        lastErrorCode = ERR_CONNECT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("InternetConnect Error"), GetLastError());
        LOG.debug(TEXT("InternetConnect failed: closing thread error."));    
        goto exit ;
    }
    
    CloseHandle (hThread);    

    wsprintf(logmsg, TEXT("Requesting resource %s"), url.resource);
    LOG.debug(logmsg);
    
    PARM_HTTP_OPEN_REQUEST     threadParmHttpOpenRequest;    
    threadParmHttpOpenRequest.pResource       = url.resource;        

    hThread = CreateThread(
                 NULL,            // Pointer to thread security attributes
                 0,               // Initial thread stack size, in bytes
                 WorkerFunctionHttpOpenRequest,  // Pointer to thread function
                 &threadParmHttpOpenRequest,     // The argument for the new thread
                 0,               // Creation flags
                 &dwThreadID      // Pointer to returned thread identifier
             );
    
    compare = WaitForSingleObject ( hThread, dwTimeout );
    if (compare == WAIT_TIMEOUT ) {
        
        LOG.debug(TEXT("HttpOpenRequest failed: timeout error!"));      
        lastErrorCode = ERR_CONNECT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpOpenRequest timeout error"), GetLastError());
        goto exit;

    } else if (compare == WAIT_OBJECT_0){
        
        LOG.debug(TEXT("HttpOpenRequest success!!"));
    }

    // The state of the specified object (thread) is signaled
    dwExitCode = 0;

    if ( !GetExitCodeThread( hThread, &dwExitCode ) ) {
        
        lastErrorCode = ERR_CONNECT;        
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpOpenRequest Error"),GetLastError());
        LOG.debug(TEXT("HttpOpenRequest failed: closing thread  error!"));            
        goto exit ;
    }

    CloseHandle (hThread);    
    
    //
    // Prepares headers
    //
    WCHAR headers[512];

    contentLength = wcslen(msg);
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), TEXT(SYNCML_CONTENT_TYPE), contentLength);
    
    // Send a request to the HTTP server.
    toUTF(msg);
    
    PARM_HTTP_SEND_REQUEST     threadParmHttpSendRequest;    

    threadParmHttpSendRequest.pHeaders       = headers;        
    threadParmHttpSendRequest.headersLength  = wcslen(headers);
    threadParmHttpSendRequest.pMsg           = (char*) msg;
    threadParmHttpSendRequest.msgLength      = contentLength;
      
    
    hThread = CreateThread(
                 NULL,                      // Pointer to thread security attributes
                 0,                         // Initial thread stack size, in bytes
                 WorkerFunctionHttpSendRequest,    // Pointer to thread function
                 &threadParmHttpSendRequest,       // The argument for the new thread
                 0,                         // Creation flags
                 &dwThreadID                // Pointer to returned thread identifier
             );
    
    compare = WaitForSingleObject (hThread, getResponseTimeout() * 1000);
    
    if (compare == WAIT_TIMEOUT ) {
        
        LOG.debug(TEXT("HttpSendRequest failed: timeout error!"));  
        lastErrorCode = ERR_HTTP_TIME_OUT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpSendRequest Error Timeout "), ERR_HTTP_TIME_OUT);
        goto exit ;

    } else if (compare == WAIT_OBJECT_0){
        LOG.debug(TEXT("HttpSendRequest success!!"));
    }

    // The state of the specified object (thread) is signaled
    dwExitCode = 0;
    
    if ( !GetExitCodeThread( hThread, &dwExitCode ) ) {
        
        lastErrorCode = ERR_CONNECT;        
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpSendRequest Error"),GetLastError());
        LOG.debug(TEXT("HttpSendRequest failed: closing thread error!"));
        goto exit ;
    }

    CloseHandle (hThread);  
    
    LOG.debug(MESSAGE_SENT);
    
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
    
    LOG.debug(READING_RESPONSE);

    wsprintf(logmsg, TEXT("Content-length: %d"), contentLength);
    LOG.debug(logmsg);
    
    if (contentLength <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        wsprintf(lastErrorMsg, TEXT("Invalid content-length: %d"), contentLength);
        goto exit;
    }   
    
    // Allocate a block of memory for lpHeadersW.
    response = new WCHAR[contentLength+1];   
    if (response == NULL) {
        lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        wsprintf(lastErrorMsg, TEXT("Not enough memory to allocate a buffer for the server response: %d required"), contentLength);
        goto exit;        

    }

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
       
        Sleep(T);
        if (previousNumRead == sumRead) {
            t += T;
            if (t > TO) {
                lastErrorCode = ERR_HTTP_TIME_OUT;
                wsprintf(lastErrorMsg, TEXT("InternetReadFile Time out: %d"), contentLength);
                goto exit;
            }
        } else {
            previousNumRead = sumRead;
            t = 0;
        }


    } while(cont);
     
    LOG.debug(TEXT("Response read")); 

    exit:
    CloseHandle (hThread);
    
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

    return response;
}

/*
 * Returns the url.
 */

URL TransportAgent::getURL() {
    return url;

}


/*
 * Returns the last response read from the server.
 * Use releaseResponseBuffer() to release the memory buffer.
 */

const WCHAR* TransportAgent::getResponse();

/*
 * Releases the response buffer allocated by sendMessage().
 */
 void TransportAgent::releaseResponseBuffer();


 /*
* The function try to read the content of a file with InternetReadFile . It was called by a thread in the main
* procedure. 
*/

DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm) {
    WCHAR* p        = NULL;
    p = response;
    (*p) = 0;
    char bufferA[5000+1];
    DWORD size = 0, read = 0;
          
    do {
        sumRead += read;
        if (!InternetReadFile (request, (LPVOID)bufferA, 5000, &read)) {
            lastErrorCode = ERR_READING_CONTENT;
            wsprintf(lastErrorMsg, TEXT("%s: %d"), TEXT("InternetReadFile Error"), GetLastError());
            cont = FALSE;
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
    cont = FALSE;

    return 0;
}

/*
* The function to open a request with HttpOpenRequest . It was called by a thread in the main
* procedure. 
*/

DWORD WINAPI WorkerFunctionHttpOpenRequest(IN LPVOID vThreadParm) {
    
    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    LPTSTR acceptTypes[2] = {TEXT("*/*"), NULL};

    PARM_HTTP_OPEN_REQUEST* pThreadParm;
    // Initialize local pointer to void pointer passed to thread
    pThreadParm = (PARM_HTTP_OPEN_REQUEST*)vThreadParm;
        
    if (!(request = HttpOpenRequest (connection,
                                     METHOD_POST,
                                     pThreadParm->pResource,
                                     HTTP_VERSION,
                                     NULL,
                                     (LPCTSTR*)acceptTypes,
                                     flags, 0))) {
      lastErrorCode = ERR_CONNECT;
      wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpOpenRequest Error"), GetLastError());
      return 1; // failure
    }
    
    
    else {
        return 0;  // success
    }
}

/*
* The function try to send a message with HttpSendRequest . It was called by a thread in the main
* procedure. 
*/
DWORD WINAPI WorkerFunctionHttpSendRequest(IN LPVOID vThreadParm) {
    
    PARM_HTTP_SEND_REQUEST* pThreadParm;
    // Initialize local pointer to void pointer passed to thread
    pThreadParm = (PARM_HTTP_SEND_REQUEST*)vThreadParm;
        
    if (!HttpSendRequest (request, 
                          pThreadParm->pHeaders, 
                          pThreadParm->headersLength, 
                          pThreadParm->pMsg, 
                          pThreadParm->msgLength)) {
        lastErrorCode = ERR_CONNECT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpSendRequest Error"),GetLastError());
        return 1; // failure
    }
        
    else {
        return 0;  // success
    }

}


/*
* The function try to connect to internet. It was called by a thread in the main
* procedure. 
*/
DWORD WINAPI WorkerFunctionInternetConnect( IN LPVOID vThreadParm) {
    
    PARM_INTERNET_CONNECT* pThreadParm;
    // Initialize local pointer to void pointer passed to thread
    pThreadParm = (PARM_INTERNET_CONNECT*)vThreadParm;
        
    if ( !( connection  = InternetConnect (inet,
                                pThreadParm->pHost,
                                pThreadParm->nServerPort,
                                NULL, // username
                                NULL, // password
                                INTERNET_SERVICE_HTTP,
                                0,
                                0))) 
        {      
        lastErrorCode = ERR_CONNECT;
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("InternetConnect Error"), GetLastError());        
        return 1; // failure
    }
    else {
        return 0;  // success
    }
}

