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
#include "http/constants.h"
#include "http/errors.h"
#include "http/PPC2003TransportAgent.h"

#include "http/GPRSConnection.h"
#include "spdm/spdmutils.h"


#define T  1000
#define TO 10000
#define TO_SEND 20000
#define ERR_HTTP_TIME_OUT 2007

DWORD WINAPI WorkerFunctionInternetConnect( IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionHttpOpenRequest( IN LPVOID vThreadParm);
DWORD WINAPI WorkerFunctionHttpSendRequest( IN LPVOID vThreadParm);

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


int sumRead, previousNumRead;
int sumByteSent, previousNumWrite;
wchar_t* response;
BOOL cont;

#define ENTERING(func) //wsprintf(logmsg, L"Entering %s", func); LOG.debug(logmsg)
#define EXITING(func)  //wsprintf(logmsg, L"Exiting %s", func);  LOG.debug(logmsg)

HINTERNET inet       = NULL,
          connection = NULL,
          request    = NULL;
/*
 * This is the PPC 2003 implementation of the TransportAgent object
 */


/*
 * This function translate a UNICODE string into a UTF string without
 * allocating additional memory. The translation is performed removing
 * the second byte of the UNICODE coding.
 *
 * @param s the string to translate
 */
void toUTF(wchar_t* s) {
    ENTERING(L"toUTF");
    int l = wcslen(s);
    wchar_t* w = s;
    char*    c = (char*)s;

    while (l--) {
        *c = (char)*w;
        ++c; ++w;
    }

    *c = 0;
    EXITING(L"toUTF");
}


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
    ENTERING(L"PPC2003TransportAgent::PPC2003TransportAgent");
    
    url   = newURL  ;
    
    if (maxResponseTimeout == 0) {
        setTimeout(DEFAULT_MAX_TIMEOUT);
    } else {
        setTimeout(maxResponseTimeout);
    }
    /* currently not used
    if (useCheckConnection == TRUE) {
        if (!EstablishConnection()) {
            lastErrorCode = ERR_INTERNET_CONNECTION_MISSING;
            wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("Internet Connection Missing"), ERR_INTERNET_CONNECTION_MISSING);
        }    
    }   
    */
    EXITING(L"PPC2003TransportAgent::PPC2003TransportAgent");
}


PPC2003TransportAgent::~PPC2003TransportAgent(){}


/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */
wchar_t* PPC2003TransportAgent::sendMessage(wchar_t* msg) {
    
    ENTERING(L"TransportAgent::sendMessage");
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

    inet = InternetOpen (USER_AGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, 0, 0);    

    if (!inet) {
        lastErrorCode = ERR_NETWORK_INIT;
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
    threadParmHttpOpenRequest.pResource = url.resource;
    threadParmHttpOpenRequest.secure    = url.isSecure(); 

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
    wchar_t headers[512];

    contentLength = wcslen(msg);
    wsprintf(headers, TEXT("Content-Type: %s\r\nContent-Length: %d"), SYNCML_CONTENT_TYPE, contentLength);
    
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

    previousNumWrite = -1;
    sumByteSent      = 0;
    t = 0;
    do {
        Sleep(T);
        wchar_t tmp[128];
        wsprintf(tmp, TEXT("t: %i - previous: %i - sumByteSent: %i"), t, previousNumWrite, sumByteSent);
        LOG.debug(tmp);
        if (previousNumWrite == sumByteSent) {
           
            t += T;
            if (t > TO_SEND) {
                lastErrorCode = 2007;
                wsprintf(lastErrorMsg, TEXT("InternetWriteFile Time out. %i"), ERR_HTTP_TIME_OUT);
                goto exit;
            }
        } else {            
            previousNumWrite = sumByteSent;
            t = 0;
        }

    } while(cont);
    
    CloseHandle (hThread);    
    LOG.debug(MESSAGE_SENT);
    //
    // restore cont variable for the InternetReadFile
    //
    cont = TRUE;

    for ( m = 0; m < getTimeout(); m++) { 
        Sleep(1000); 
        size = sizeof(status);
        queryInfo = HttpQueryInfo (request,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       (LPDWORD)&status,
                       (LPDWORD)&size,
                       NULL);                       

        queryInfo = HttpQueryInfo (request,
                       HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
                       (LPDWORD)&contentLength,
                       (LPDWORD)&size,
                       NULL);

        if( status == HTTP_OK ) {
            LOG.debug(L"Status: HTTP_OK 200");
            break;
        }
        else if( status == HTTP_SERVER_ERROR ) {
            LOG.error(L"Status: HTTP_SERVER_ERROR");
            break;
        }
        else if( status == HTTP_UNAUTHORIZED ) {
            LOG.error(L"Status: HTTP_UNAUTHORIZED");
            break;
        }
        else if( status == HTTP_ERROR ) {
            LOG.error(L"Status: HTTP_ERROR");
            break;
        }
        else if( status == HTTP_NOT_FOUND ) {
            LOG.error(L"Status: HTTP_NOT_FOUND");
            break;
        }
        else if( status == X_HTTP_420_IPPRV ) {
            LOG.error(L"Status: X_HTTP_420_IPPRV 420");
            break;
        }    
        else {
            //wchar_t dbg[200];
            //wcsprintf(dbg, TEXT("Status: %d"), status);
            //LOG.debug(dbg);
        }
    }
        
    //
    // If status code is not OK, returns immediately, otherwise reads the response
    //
    if (status != STATUS_OK) {
        lastErrorCode = ERR_HTTP;
        wsprintf(lastErrorMsg, TEXT("HTTP request error: %d"), status);
        goto exit;
    }
    LOG.debug(READING_RESPONSE);
    wsprintf(logmsg, TEXT("Content-length: %d"), contentLength);
    LOG.debug(logmsg);
    
    if (contentLength <= 0) {
        lastErrorCode = ERR_READING_CONTENT;
        wsprintf(lastErrorMsg, TEXT("Invalid content-length: %d"), contentLength);
        goto exit;
    }   
    
    // Allocate a block of memory for lpHeadersW.
    response = new wchar_t[contentLength+1];   
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
    LOG.debug(response);
    

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
    EXITING(L"TransportAgent::sendMessage");
    return response;
}



 /*
* The function try to read the content of a file with InternetReadFile . It was called by a thread in the main
* procedure. 
*/

DWORD WINAPI WorkerFunctionInternetReadFile(IN LPVOID vThreadParm) {
    ENTERING(L"WorkerFunctionInternetReadFile");
    wchar_t* p        = NULL;
    p = response;
    (*p) = 0;
    char bufferA[5000+1];
    DWORD size = 0, read = 0;
          
    do {
        sumRead += read;
        if (!InternetReadFile (request, (LPVOID)bufferA, 5000, &read)) {
            lastErrorCode = ERR_READING_CONTENT;
            wsprintf(lastErrorMsg, TEXT("%s: %d"), TEXT("InternetReadFile Error"), GetLastError());
            break;
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
    EXITING(L"WorkerFunctionInternetReadFile");
    return 0;
}

/*
* The function to open a request with HttpOpenRequest . It was called by a thread in the main
* procedure. 
*/

DWORD WINAPI WorkerFunctionHttpOpenRequest(IN LPVOID vThreadParm) {
    ENTERING(L"WorkerFunctionHttpOpenRequest");
    
    DWORD ret = 0; // success

    PARM_HTTP_OPEN_REQUEST* pThreadParm;
    // Initialize local pointer to void pointer passed to thread
    pThreadParm = (PARM_HTTP_OPEN_REQUEST*)vThreadParm;

    DWORD flags = INTERNET_FLAG_RELOAD 
                | INTERNET_FLAG_NO_CACHE_WRITE;
    if (pThreadParm->secure) {
        flags = flags
              | INTERNET_FLAG_SECURE
              | INTERNET_FLAG_IGNORE_CERT_CN_INVALID
              | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
              ;
    }

    LPTSTR acceptTypes[2] = {TEXT("*/*"), NULL};

        
    if (!(request = HttpOpenRequest (connection,
                                     METHOD_POST,
                                     pThreadParm->pResource,
                                     HTTP_VERSION,
                                     NULL,
                                     (LPCTSTR*)acceptTypes,
                                     flags, 0))) {
      lastErrorCode = ERR_CONNECT;
      wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("HttpOpenRequest Error"), GetLastError());
      ret =  1; // failure
    }
    
    EXITING(L"WorkerFunctionHttpOpenRequest");
    
    return ret;
    
}

/*
* The function try to send a message with HttpSendRequest . It was called by a thread in the main
* procedure. 
*/
DWORD WINAPI WorkerFunctionHttpSendRequest(IN LPVOID vThreadParm) {
    ENTERING(L"WorkerFunctionHttpSendRequest");
    
    INTERNET_BUFFERS BufferIn;
    
    DWORD dwBytesWritten = 0;
    char  pBuffer[4096 + 1];
    memset(pBuffer, 0, 4096);
    char* ptrBuffer = NULL;   

    sumByteSent = 0;

    PARM_HTTP_SEND_REQUEST* pThreadParm;
    
    pThreadParm = (PARM_HTTP_SEND_REQUEST*)vThreadParm;
        
    BufferIn.dwStructSize       = sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
    BufferIn.Next               = NULL; 
    BufferIn.lpcszHeader        = pThreadParm->pHeaders;
    BufferIn.dwHeadersLength    = pThreadParm->headersLength;
    BufferIn.dwHeadersTotal     = pThreadParm->headersLength;
    BufferIn.lpvBuffer          = NULL;                
    BufferIn.dwBufferLength     = 0;
    BufferIn.dwBufferTotal      = pThreadParm->msgLength; // This is the only member used other than dwStructSize
    BufferIn.dwOffsetLow        = 0;
    BufferIn.dwOffsetHigh       = 0;
    

    if(!HttpSendRequestEx( request, &BufferIn, NULL, 0, 0)) {   
        lastErrorCode = ERR_CONNECT;        
        wsprintf (lastErrorMsg, TEXT("%s: %d"), TEXT("Error on HttpSendRequestEx"),GetLastError());
        LOG.debug(lastErrorMsg);                
    }        

    ptrBuffer = pThreadParm->pMsg;
    
    do {
        
        strncpy (pBuffer, ptrBuffer, 4096);
        if (!InternetWriteFile (request, pBuffer, strlen(pBuffer), &dwBytesWritten)) {
            lastErrorCode = 8001;
            wsprintf(lastErrorMsg, TEXT("%s: %d"), TEXT("InternetWriteFile Error"), GetLastError());                   
        } else {          
           LOG.debug(TEXT("InternetWriteFile success..."));
        }
        
        sumByteSent += dwBytesWritten;
        ptrBuffer = ptrBuffer + (dwBytesWritten * (sizeof(char)));

        if (sumByteSent < pThreadParm->msgLength) {
            
        } else {
            
            break;
        }

    } while (1);
    
    cont = FALSE;    
    
    if(!HttpEndRequest(request, NULL, 0, 0))
    {
        LOG.debug(TEXT("Error in httpEndRequest"));
       
    }
    EXITING(L"WorkerFunctionHttpSendRequest");
    return 0;

}


/*
* The function try to connect to internet. It was called by a thread in the main
* procedure. 
*/
DWORD WINAPI WorkerFunctionInternetConnect( IN LPVOID vThreadParm) {
    
    ENTERING(L"WorkerFunctionInternetConnect");

    DWORD ret = 0; // success
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
        ret = 1; // failure
    }
    
    EXITING(L"WorkerFunctionInternetConnect");
    return ret;
}

