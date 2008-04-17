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

#include "http/MacTransportAgent.h"
#include "http/constants.h"

USE_NAMESPACE

MacTransportAgent::MacTransportAgent(){}

MacTransportAgent::~MacTransportAgent() {}

/*
 * Constructor.
 * In this implementation newProxy is ignored, since proxy configuration
 * is taken from the WinInet subsystem.
 *
 * @param url the url where messages will be sent with sendMessage()
 * @param proxy proxy information or NULL if no proxy should be used
 */
MacTransportAgent::MacTransportAgent(URL& newURL, Proxy& newProxy, unsigned int maxResponseTimeout) : 
            TransportAgent(newURL, newProxy, maxResponseTimeout){
                
            }
    

/*
 * Sends the given SyncML message to the server specified
 * by the instal property 'url'. Returns the response status code or -1
 * if it was not possible initialize the connection.
 *
 * Use getResponse() to get the server response.
 */
char* MacTransportAgent::sendMessage(const char* msg){

    if(!msg){
        LOG.error("MacTransportAgent::sendMessage error: NULL message.");
        setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: NULL message.");
        return NULL;
    }
    
   // size_t size = strlen(msg);
    LOG.debug("Requesting resource %s at %s:%d", url.resource, url.host, url.port);
    
    
    //CFStringRef bodyStr = CFSTR(msg); // Usually used for POST data
    CFStringRef bodyStr = CFStringCreateWithCString(NULL, msg, kCFStringEncodingUTF8);
    
    
    CFStringRef headerFieldName = CFSTR("Content-Type");
    
    CFStringRef headerFieldValue = CFSTR("application/vnd.syncml+xml");
    
    
    
   // CFStringRef CFurl = CFSTR(url.host);
    CFStringRef CFurl =  CFStringCreateWithCString(NULL, url.fullURL, kCFStringEncodingUTF8);
    
    CFURLRef myURL = CFURLCreateWithString(kCFAllocatorDefault, CFurl, NULL);
    
    
    
    CFStringRef requestMethod = CFSTR("POST");
    
    CFHTTPMessageRef myRequest =
                    CFHTTPMessageCreateRequest(kCFAllocatorDefault, 
                                               requestMethod, myURL, kCFHTTPVersion1_1);
    
    if(!myRequest){
        LOG.error("MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
        setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
        return NULL;
    }
    
    CFDataRef bodyData = CFStringCreateExternalRepresentation(kCFAllocatorDefault, bodyStr, kCFStringEncodingUTF8, 0);	
    if (!bodyData){
        LOG.error("MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
        setError(ERR_NETWORK_INIT, "MacTransportAgent::sendMessage error: CFHTTPMessageCreateRequest Error.");
        return NULL;
    }        
    CFHTTPMessageSetBody(myRequest,bodyData);
    
    CFHTTPMessageSetHeaderFieldValue(myRequest, headerFieldName, headerFieldValue);
    
    CFDataRef mySerializedRequest = CFHTTPMessageCopySerializedMessage(myRequest);
    
        
    char* tempret = (char*)CFDataGetBytePtr(mySerializedRequest);
    
    char * ret = new char[strlen(tempret)];
    strcpy(ret, tempret);
    
    CFRelease(bodyStr);
    CFRelease(headerFieldName);
    CFRelease(headerFieldValue);
    CFRelease(CFurl);
    CFRelease(myURL);
    CFRelease(myRequest);
    CFRelease(bodyData);
    CFRelease(mySerializedRequest);
    
    
    return ret;
}

