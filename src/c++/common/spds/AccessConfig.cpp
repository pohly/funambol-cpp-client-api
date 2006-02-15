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


#include "base/fscapi.h"
#include "base/util/utils.h"
#include "spds/AccessConfig.h"
#include "spdm/constants.h"


AccessConfig::AccessConfig() {
	
	useProxy = FALSE;
    proxyPort = 8080;
    firstTimeSyncMode = SYNC_SLOW;
    dirty = FALSE;
    
    username   = NULL;
    password   = NULL;
    deviceId   = NULL;
    proxyHost  = NULL;
    syncURL    = NULL;
    
    endTimestamp = beginTimestamp = 0;

    serverNonce           = NULL;  // from client to server
    clientNonce           = NULL;  // from server to client
    serverID              = NULL;
    serverPWD             = NULL;
    clientAuthType        = NULL;
    serverAuthType        = NULL;
    isServerAuthRequired  = FALSE;  // F = FALSE, T = TRUE

    maxMsgSize            = 0;
    maxModPerMsg          = 0;
    encryption            = FALSE;  // F = FALSE, T = TRUE
}

AccessConfig::AccessConfig(AccessConfig& s) {
    assign(s);
}

AccessConfig::~AccessConfig() {
	safeDelete(&username );
	safeDelete(&password );
	safeDelete(&deviceId );
	safeDelete(&proxyHost);
	safeDelete(&syncURL  );
    
    safeDelete(&serverNonce         );
    safeDelete(&clientNonce         );         
    safeDelete(&serverID            );
    safeDelete(&serverPWD           );
    safeDelete(&clientAuthType      );    
    safeDelete(&serverAuthType      );    

}

BOOL AccessConfig::getServerAuthRequired() {
    return isServerAuthRequired;
}

void AccessConfig::setServerAuthRequired(BOOL v) {
    isServerAuthRequired = v;
    
    dirty |= DIRTY_SERVERAUTH_REQUIRED;
}

const wchar_t* AccessConfig::getServerAuthType(const wchar_t* buf) {
    if (buf == NULL) {
        return serverAuthType;
    }

    return wcscpy((wchar_t*)buf, serverAuthType);
}


void AccessConfig::setServerAuthType(const wchar_t* v){
	set(&serverAuthType, v);	    
}


const wchar_t* AccessConfig::getClientAuthType(const wchar_t* buf) {
    if (buf == NULL) {
        return clientAuthType;
    }

    return wcscpy((wchar_t*)buf, clientAuthType);
}


void AccessConfig::setClientAuthType(const wchar_t* v){
	set(&clientAuthType, v);
	
    dirty |= DIRTY_CLIENTAUTHTYPE; 
}

const wchar_t* AccessConfig::getServerPWD(const wchar_t* buf) {
    if (buf == NULL) {
		return serverPWD;
    }

    return wcscpy((wchar_t*)buf, serverPWD);
}


void AccessConfig::setServerPWD(const wchar_t* v){
	set(&serverPWD, v);
	
    dirty |= DIRTY_SERVERPWD;
}

const wchar_t* AccessConfig::getServerID(const wchar_t* buf) {
    if (buf == NULL) {
		return serverID;
    }

    return wcscpy((wchar_t*)buf, serverID);
}


void AccessConfig::setServerID(const wchar_t* v){
	set(&serverID, v);
	
    dirty |= DIRTY_SERVERID;
}

const wchar_t* AccessConfig::getServerNonce(const wchar_t* buf) {
    if (buf == NULL) {
		return serverNonce;
    }

    return wcscpy((wchar_t*)buf, serverNonce);;
}


void AccessConfig::setServerNonce(const wchar_t* v){
	set(&serverNonce, v);
	
    dirty |= DIRTY_SERVER_NONCE;
}

const wchar_t* AccessConfig::getClientNonce(const wchar_t* buf) {
    if (buf == NULL) {
		return clientNonce;
    }

    return wcscpy((wchar_t*)buf, clientNonce);
}


void AccessConfig::setClientNonce(const wchar_t* v){
	set(&clientNonce, v);
	
    dirty |= DIRTY_CLIENT_NONCE;
}

const wchar_t* AccessConfig::getUsername(const wchar_t* buf) {
    if (buf == NULL) {
		return username;
    }

    return wcscpy((wchar_t*)buf, username);
}


void AccessConfig::setUsername(const wchar_t* v){
	set(&username, v);
	
    dirty |= DIRTY_USERNAME;
}


const wchar_t* AccessConfig::getPassword(const wchar_t* buf) {
    if (buf == NULL) {
		return password;
    }

    return wcscpy((wchar_t*)buf, password);
}

void AccessConfig::setPassword(const wchar_t* v) {	
	set(&password, v);
	 
    dirty |= DIRTY_PASSWORD;
}


const wchar_t* AccessConfig::getDeviceId(const wchar_t* buf) {
    if (buf == NULL) {
		return deviceId;
    }

    return wcscpy((wchar_t*)buf, deviceId);
}

void AccessConfig::setDeviceId(const wchar_t* v) {	
	set(&deviceId, v);
	
    dirty |= DIRTY_DEVICE_ID;
}

SyncMode AccessConfig::getFirstTimeSyncMode() {
    return firstTimeSyncMode;
}

void AccessConfig::setFirstTimeSyncMode(SyncMode v) {
    firstTimeSyncMode = v;
    
    dirty |= DIRTY_FIRST_TIME_SYNC_MODE;
}

BOOL AccessConfig::getUseProxy() {
    return useProxy;
}

void AccessConfig::setUseProxy(BOOL v) {
    useProxy = v;
    
    dirty |= DIRTY_USE_PROXY;
}

const wchar_t* AccessConfig::getProxyHost(const wchar_t* buf) {
    if (buf == NULL) {
		return proxyHost;
	}

    return wcscpy((wchar_t*)buf, proxyHost);
}

void AccessConfig::setProxyHost(const wchar_t* v) {
	set(&proxyHost, v);
	
    dirty |= DIRTY_PROXY_HOST;
}

const wchar_t* AccessConfig::getSyncURL(const wchar_t* buf) {
    if (buf == NULL) {
		return syncURL;
	}

    return wcscpy((wchar_t*)buf, syncURL);
}

void AccessConfig::setSyncURL(const wchar_t* v) {	
	//
	// Checks if the url starts with http(s)://; if not, http:// is prepended
	//
	set(&syncURL, v);
	
	// Adds default protocol if not set AND the string is not empty
    if (*syncURL							  &&
		wcsncmp(syncURL, TEXT("http://"), 7)  && 
	    wcsncmp(syncURL, TEXT("HTTP://"), 7)  &&
	    wcsncmp(syncURL, TEXT("https://"), 8) &&
	    wcsncmp(syncURL, TEXT("HTTPS://"), 8) ) {
        
		wchar_t* dest = new wchar_t[wcslen(syncURL)+8];
		wcscpy((wchar_t*)dest, TEXT("http://")); wcscat(dest, syncURL);
		
		set(&syncURL, dest);

        if (dest != NULL)
            delete dest;
	}
	
    dirty |= DIRTY_SYNC_URL;
}

void AccessConfig::setBeginSync(unsigned long timestamp) {
    beginTimestamp = timestamp;
    dirty |= DIRTY_SYNC_BEGIN;
}

unsigned long AccessConfig::getBeginSync() {
    return beginTimestamp;
}

void AccessConfig::setMaxMsgSize(unsigned long msgSize) {
    maxMsgSize = msgSize;
}

unsigned long AccessConfig::getMaxMsgSize() {
    return maxMsgSize;
}

void AccessConfig::setMaxModPerMsg(unsigned long mod){
    maxModPerMsg = mod;
}

unsigned long AccessConfig::getMaxModPerMsg() {
    return maxModPerMsg;
}

void AccessConfig::setEndSync(unsigned long timestamp) {
    endTimestamp = timestamp;
    dirty |= DIRTY_SYNC_END;
}

unsigned long AccessConfig::getEndSync() {
    return endTimestamp;
}

unsigned int AccessConfig::getDirty() {
    return dirty;
}

void AccessConfig::set(wchar_t** buf, const wchar_t* v) {
	safeDelete(buf);
	
	if (v == NULL) {
		v = TEXT("");
	}
	int len = wcslen(v);
	*buf = new wchar_t[len+2];
	
	wcscpy(*buf, v);
}

BOOL AccessConfig::getEncryption() {
    return encryption;   
}

/**
 * Sets if the sync engine should use a HTTP proxy to access the server.
 *
 * @param useProxy FALSE for not use a proxy, TRUE otherwise
 */
void AccessConfig::setEncryption(BOOL useEncryption) {
    encryption = useEncryption;   
}

void AccessConfig::assign(AccessConfig& s) {
	setUsername (s.getUsername() );
	setPassword (s.getPassword() );
	setSyncURL  (s.getSyncURL()  );
	setProxyHost(s.getProxyHost());
	setDeviceId (s.getDeviceId() );
	
	setBeginSync(s.getBeginSync());
	setEndSync(s.getEndSync());
	setFirstTimeSyncMode(s.getFirstTimeSyncMode());
	
    setServerAuthRequired(s.getServerAuthRequired());
	setClientAuthType(s.getClientAuthType());
    setServerAuthType(s.getServerAuthType());
    setServerPWD(s.getServerPWD());
    setServerID(s.getServerID());
    setServerNonce(s.getServerNonce());
    setClientNonce(s.getClientNonce());
    setMaxMsgSize(s.getMaxMsgSize());
    setMaxModPerMsg(s.getMaxModPerMsg());
    
    setEncryption  (s.getEncryption()  );

	dirty = s.getDirty();
}

