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

const BCHAR* AccessConfig::getServerAuthType(const BCHAR* buf) {
    if (buf == NULL) {
        return serverAuthType;
    }

    return bstrcpy((BCHAR*)buf, serverAuthType);
}


void AccessConfig::setServerAuthType(const BCHAR* v){
	set(&serverAuthType, v);	    
}


const BCHAR* AccessConfig::getClientAuthType(const BCHAR* buf) {
    if (buf == NULL) {
        return clientAuthType;
    }

    return bstrcpy((BCHAR*)buf, clientAuthType);
}


void AccessConfig::setClientAuthType(const BCHAR* v){
	set(&clientAuthType, v);
	
    dirty |= DIRTY_CLIENTAUTHTYPE; 
}

const BCHAR* AccessConfig::getServerPWD(const BCHAR* buf) {
    if (buf == NULL) {
		return serverPWD;
    }

    return bstrcpy((BCHAR*)buf, serverPWD);
}


void AccessConfig::setServerPWD(const BCHAR* v){
	set(&serverPWD, v);
	
    dirty |= DIRTY_SERVERPWD;
}

const BCHAR* AccessConfig::getServerID(const BCHAR* buf) {
    if (buf == NULL) {
		return serverID;
    }

    return bstrcpy((BCHAR*)buf, serverID);
}


void AccessConfig::setServerID(const BCHAR* v){
	set(&serverID, v);
	
    dirty |= DIRTY_SERVERID;
}

const BCHAR* AccessConfig::getServerNonce(const BCHAR* buf) {
    if (buf == NULL) {
		return serverNonce;
    }

    return bstrcpy((BCHAR*)buf, serverNonce);;
}


void AccessConfig::setServerNonce(const BCHAR* v){
	set(&serverNonce, v);
	
    dirty |= DIRTY_SERVER_NONCE;
}

const BCHAR* AccessConfig::getClientNonce(const BCHAR* buf) {
    if (buf == NULL) {
		return clientNonce;
    }

    return bstrcpy((BCHAR*)buf, clientNonce);
}


void AccessConfig::setClientNonce(const BCHAR* v){
	set(&clientNonce, v);
	
    dirty |= DIRTY_CLIENT_NONCE;
}

const BCHAR* AccessConfig::getUsername(const BCHAR* buf) {
    if (buf == NULL) {
		return username;
    }

    return bstrcpy((BCHAR*)buf, username);
}


void AccessConfig::setUsername(const BCHAR* v){
	set(&username, v);
	
    dirty |= DIRTY_USERNAME;
}


const BCHAR* AccessConfig::getPassword(const BCHAR* buf) {
    if (buf == NULL) {
		return password;
    }

    return bstrcpy((BCHAR*)buf, password);
}

void AccessConfig::setPassword(const BCHAR* v) {	
	set(&password, v);
	 
    dirty |= DIRTY_PASSWORD;
}


const BCHAR* AccessConfig::getDeviceId(const BCHAR* buf) {
    if (buf == NULL) {
		return deviceId;
    }

    return bstrcpy((BCHAR*)buf, deviceId);
}

void AccessConfig::setDeviceId(const BCHAR* v) {	
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

const BCHAR* AccessConfig::getProxyHost(const BCHAR* buf) {
    if (buf == NULL) {
		return proxyHost;
	}

    return bstrcpy((BCHAR*)buf, proxyHost);
}

void AccessConfig::setProxyHost(const BCHAR* v) {
	set(&proxyHost, v);
	
    dirty |= DIRTY_PROXY_HOST;
}

const BCHAR* AccessConfig::getSyncURL(const BCHAR* buf) {
    if (buf == NULL) {
		return syncURL;
	}

    return bstrcpy((BCHAR*)buf, syncURL);
}

void AccessConfig::setSyncURL(const BCHAR* v) {	
	//
	// Checks if the url starts with http(s)://; if not, http:// is prepended
	//
	set(&syncURL, v);
	
	// Adds default protocol if not set AND the string is not empty
    if (*syncURL							&&
		bstrncmp(syncURL, T("http://"), 7)  && 
	    bstrncmp(syncURL, T("HTTP://"), 7)  &&
	    bstrncmp(syncURL, T("https://"), 8) &&
	    bstrncmp(syncURL, T("HTTPS://"), 8) ) {
        
		BCHAR* dest = new BCHAR[bstrlen(syncURL)+8];
        bsprintf(dest, T("http://%s"), syncURL );
		
		set(&syncURL, dest);

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

void AccessConfig::set(BCHAR** buf, const BCHAR* v) {
	safeDelete(buf);
	
	if (v == NULL) {
		v = T("");
	}
	int len = bstrlen(v);
	*buf = new BCHAR[len+2];
	
	bstrcpy(*buf, v);
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

