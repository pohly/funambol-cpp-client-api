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
    readBufferSize        = 0;
    userAgent             = NULL;
    proxyUsername         = NULL;
    proxyPassword         = NULL;

    checkConn             = FALSE;
    responseTimeout       = 0;
}

AccessConfig::AccessConfig(AccessConfig& s) {
    assign(s);
}

AccessConfig::~AccessConfig() {
	safeDelete(&username );
	safeDelete(&password );
	safeDelete(&proxyHost);
	safeDelete(&syncURL  );
    
    safeDelete(&serverNonce         );
    safeDelete(&clientNonce         );         
    safeDelete(&serverID            );
    safeDelete(&serverPWD           );
    safeDelete(&clientAuthType      );    
    safeDelete(&serverAuthType      );  
    safeDelete(&userAgent           );
    safeDelete(&proxyUsername       );
    safeDelete(&proxyPassword       );
}

BOOL AccessConfig::getServerAuthRequired() const {
    return isServerAuthRequired;
}

void AccessConfig::setServerAuthRequired(BOOL v) {
    isServerAuthRequired = v;
    
    dirty |= DIRTY_SERVERAUTH_REQUIRED;
}

const BCHAR* AccessConfig::getServerAuthType() const {
    return serverAuthType;
}


void AccessConfig::setServerAuthType(const BCHAR* v){
	set(&serverAuthType, v);	    
}


const BCHAR* AccessConfig::getClientAuthType() const {
    return clientAuthType;
}


void AccessConfig::setClientAuthType(const BCHAR* v){
	set(&clientAuthType, v);
	
    dirty |= DIRTY_CLIENTAUTHTYPE; 
}

const BCHAR* AccessConfig::getServerPWD() const {
    return serverPWD;
}


void AccessConfig::setServerPWD(const BCHAR* v){
	set(&serverPWD, v);
	
    dirty |= DIRTY_SERVERPWD;
}

const BCHAR* AccessConfig::getServerID() const {
    return serverID;
}


void AccessConfig::setServerID(const BCHAR* v){
	set(&serverID, v);
	
    dirty |= DIRTY_SERVERID;
}

const BCHAR* AccessConfig::getServerNonce() const {
    return serverNonce;
}


void AccessConfig::setServerNonce(const BCHAR* v){
	set(&serverNonce, v);
	
    dirty |= DIRTY_SERVER_NONCE;
}

const BCHAR* AccessConfig::getClientNonce() const {
    return clientNonce;
}


void AccessConfig::setClientNonce(const BCHAR* v){
	set(&clientNonce, v);
	
    dirty |= DIRTY_CLIENT_NONCE;
}

const BCHAR* AccessConfig::getUsername() const {
    return username;
}


void AccessConfig::setUsername(const BCHAR* v){
	set(&username, v);
	
    dirty |= DIRTY_USERNAME;
}


const BCHAR* AccessConfig::getPassword() const {
    return password;
}

void AccessConfig::setPassword(const BCHAR* v) {	
	set(&password, v);
	 
    dirty |= DIRTY_PASSWORD;
}

SyncMode AccessConfig::getFirstTimeSyncMode() const {
    return firstTimeSyncMode;
}

void AccessConfig::setFirstTimeSyncMode(SyncMode v) {
    firstTimeSyncMode = v;
    
    dirty |= DIRTY_FIRST_TIME_SYNC_MODE;
}

BOOL AccessConfig::getUseProxy() const {
    return useProxy;
}

void AccessConfig::setUseProxy(BOOL v) {
    useProxy = v;
    
    dirty |= DIRTY_USE_PROXY;
}

const BCHAR* AccessConfig::getProxyHost() const {
    return proxyHost;
}

void AccessConfig::setProxyHost(const BCHAR* v) {
	set(&proxyHost, v);
	
    dirty |= DIRTY_PROXY_HOST;
}

int AccessConfig::getProxyPort() const {
    return proxyPort;
}

void AccessConfig::setProxyPort(int v) {
	proxyPort = v;
	
    dirty |= DIRTY_PROXY_PORT;
}

BCHAR* AccessConfig::getProxyUsername() const {
    return proxyUsername;
}

void AccessConfig::setProxyUsername(const BCHAR* v) {
	set(&proxyUsername, v);
}

BCHAR* AccessConfig::getProxyPassword() const {
    return proxyPassword;
}

void AccessConfig::setProxyPassword(const BCHAR* v) {
	set(&proxyPassword, v);
}

const BCHAR* AccessConfig::getUserAgent() const {    
    return userAgent;	
}

void AccessConfig::setUserAgent(const BCHAR* v) {
	set(&userAgent, v);	    
}


unsigned int AccessConfig::getResponseTimeout() const {
    return responseTimeout;
}
void AccessConfig::setResponseTimeout(unsigned int v) {
	responseTimeout = v;
}

BOOL AccessConfig::getCheckConn() const {
    return checkConn;
}
void AccessConfig::setCheckConn(BOOL v) {
	checkConn = v;
}


const BCHAR* AccessConfig::getSyncURL() const {
    return syncURL;
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

unsigned long AccessConfig::getBeginSync() const {
    return beginTimestamp;
}

void AccessConfig::setMaxMsgSize(unsigned long msgSize) {
    maxMsgSize = msgSize;
}

unsigned long AccessConfig::getMaxMsgSize() const {
    return maxMsgSize;
}

void AccessConfig::setReadBufferSize(unsigned long bufferSize) {
    readBufferSize = bufferSize;
}

unsigned long AccessConfig::getReadBufferSize() const {
    return readBufferSize;
}

void AccessConfig::setMaxModPerMsg(unsigned long mod){
    maxModPerMsg = mod;
}

unsigned long AccessConfig::getMaxModPerMsg() const {
    return maxModPerMsg;
}

void AccessConfig::setEndSync(unsigned long timestamp) {
    endTimestamp = timestamp;
    dirty |= DIRTY_SYNC_END;
}

unsigned long AccessConfig::getEndSync() const {
    return endTimestamp;
}


unsigned int AccessConfig::getDirty() const {
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

BOOL AccessConfig::getEncryption() const {
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

void AccessConfig::assign(const AccessConfig& s) {
	setUsername (s.getUsername() );
	setPassword (s.getPassword() );
	setSyncURL  (s.getSyncURL()  );
	setProxyHost(s.getProxyHost());
    setProxyPort(s.getProxyPort());
    setUserAgent(s.getUserAgent());

    setProxyUsername(s.getProxyUsername());
    setProxyPassword(s.getProxyPassword());
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
    setReadBufferSize(s.getReadBufferSize());
    setEncryption  (s.getEncryption()  );
    setCheckConn(s.getCheckConn());
    setResponseTimeout(s.getResponseTimeout());

	dirty = s.getDirty();
}

