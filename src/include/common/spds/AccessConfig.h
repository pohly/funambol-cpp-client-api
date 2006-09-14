/*
 * Copyright (C) 2005-2006 Funambol
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
#ifndef INCL_ACCESS_CONFIG
#define INCL_ACCESS_CONFIG

#include "base/fscapi.h"
#include "spds/constants.h"


/*
 * -------------------------- AccessConfig class -----------------------------
 * This class groups all configuration properties to estabilish a 
 * connection with a sync server.
 * AccessConfig is a part of SyncManagerConfig (along with DeviceConfig 
 * and an array of SyncSourceConfig).
 *
 * Class members:
 * --------------
 * username             : string for username
 * password             : string for password
 * useProxy             : Should the sync engine use a HTTP proxy?
 * proxyHost            : the host for proxy connection
 * proxyPort            : the port for proxy connection
 * proxyUsername        : the proxy username (if proxy needs authentication)
 * proxyPassword        : the proxy password (if proxy needs authentication)
 * beginTimestamp       : The beginSync timestamp
 * endTimestamp         : The endSync timestamp
 * firstTimeSyncMode    : The SyncMode that the sync engine should use 
 *                        the first time a source is synced
 * serverNonce          : The server nonce value: from client to server
 * clientNonce          : The client nonce value: from server to client
 * serverID             : the server ID value
 * serverPWD            : the server password
 * clientAuthType       : the type of client authentication used by client
 * isServerAuthRequired : Does the server require authentication?
 * maxMsgSize           : The maximum message size (Byte) accepted for XML 
 *                        messages received from server (server to client)
 * maxModPerMsg         : The maximum number of modifications sent in each XML 
 *                        message from client to server
 * readBufferSize       : 
 * encryption           : Do we use ciphering?
 * userAgent            : The user agent string, will be attached to http 
 *                        messages to identify the client on server side.
 *                        It shoud be a short description with the client 
 *                        name plus its version
 * checkConn            : 
 * responseTimeout      : 
 * dirty                : The dirty flag, used to select which properties 
 *                        have been modified. Not used by now (T.B.D)
 */
class AccessConfig {
    private:
        BCHAR*          username            ;
        BCHAR*          password            ;
        BOOL            useProxy            ;
        BCHAR*          proxyHost           ;
        int             proxyPort           ;
        BCHAR*          proxyUsername       ;
        BCHAR*          proxyPassword       ;
        BCHAR*          syncURL             ;
        unsigned long   beginTimestamp      ;
        unsigned long   endTimestamp        ;
        SyncMode        firstTimeSyncMode   ;
        BCHAR*          serverNonce         ;
        BCHAR*          clientNonce         ;
        BCHAR*          serverID            ;
        BCHAR*          serverPWD           ;
        BCHAR*          clientAuthType      ;
        BCHAR*          serverAuthType      ;
        BOOL            isServerAuthRequired;
        unsigned long   maxMsgSize          ;
        unsigned long   maxModPerMsg        ;
        unsigned long   readBufferSize      ;
        BOOL            encryption          ;
        BCHAR*          userAgent           ;
        BOOL            checkConn           ;
        unsigned int    responseTimeout     ;

        unsigned int dirty;

        /**
         * Sets the given buffer with the given value, dealing correctly with
         * NULL values. If a NULL value is passed, the empty string is used.
         *
         * @param buf the destination buffer
         * @param v the new value (CAN BE NULL)
         */
        void set(BCHAR** buf, const BCHAR* v) EXTRA_SECTION_02;

    public:

        AccessConfig() EXTRA_SECTION_02;
        AccessConfig(AccessConfig& s) EXTRA_SECTION_02;
        ~AccessConfig() EXTRA_SECTION_02;

        /**
         * Returns the username value.
         *
         * @return The username value. The caller MUST NOT release
         *         the memory itself.
         *
         */
        const BCHAR* getUsername() const EXTRA_SECTION_02;

        /**
         *  Sets the username value. The given data are copied in an internal
         *  buffer so that the caller is assured that the given address can be
         *  released after the call.
         *
         *  @param username the new username value
         */
        void setUsername(const BCHAR* username) EXTRA_SECTION_02;

        /**
         * Returns the password value.
         */
        const BCHAR* getPassword() const EXTRA_SECTION_02;

        /**
         * Sets a new password value. The given data are copied in an internal
         * buffer so that the caller is assured that the given address can be
         * released after the call.
         *
         * @param password the new password value
         */
        void setPassword(const BCHAR* password) EXTRA_SECTION_02;

        /**
         * Returns the SyncMode that the sync engine should use the first time
         * a source is synced
         */
        SyncMode getFirstTimeSyncMode() const EXTRA_SECTION_02;

        /**
         * Sets the SyncMode that the sync engine should use the first time
         * a source is synced
         *
         * @param syncMode the new sync mode
         */
        void setFirstTimeSyncMode(SyncMode syncMode) EXTRA_SECTION_02;

        /**
         * Should the sync engine use a HTTP proxy?
         */
        BOOL getUseProxy() const EXTRA_SECTION_02;

        /**
         * Sets if the sync engine should use a HTTP proxy to access the server.
         *
         * @param useProxy FALSE for not use a proxy, TRUE otherwise
         */
        void setUseProxy(BOOL useProxy) EXTRA_SECTION_02;

        /**
         * Returns the proxyHost value.
         */
        const BCHAR* getProxyHost() const EXTRA_SECTION_02;

        /**
         * Sets a new proxyHost value.
         *
         * @param proxyHost the new proxyHost value
         */
        void setProxyHost(const BCHAR* proxyHost) EXTRA_SECTION_02;

        int getProxyPort() const EXTRA_SECTION_02;
        void setProxyPort(int v) EXTRA_SECTION_02;

        /**
         * Returns the proxyUsername value.
         */
        BCHAR* getProxyUsername() const EXTRA_SECTION_02;

        /**
         * Sets a new proxyUsername value.
         *
         * @param proxyUsername the new proxyUsername value
         */
        void setProxyUsername(const BCHAR* proxyUsername) EXTRA_SECTION_02;

        /**
         * Returns the proxyPassword value.
         */
        BCHAR* getProxyPassword() const EXTRA_SECTION_02;

        /**
         * Sets a new proxyPassword value.
         *
         * @param proxyPassword the new proxyPassword value
         */
        void setProxyPassword(const BCHAR* proxyPassword) EXTRA_SECTION_02;

        /**
         * Returns the syncURL value. If the URL does not start with http://
         * (or HTTP://) or https:// (or HTTPS://), http:// is prepended to the
         * given string.
         */
        const BCHAR* getSyncURL() const EXTRA_SECTION_02;

        /**
         * Sets a new the syncURL value. The given data are copied in an internal
         * buffer so that the caller is assured that the given address can be
         * released after the call.
         *
         * @param syncURL the new syncURL value
         */
        void setSyncURL(const BCHAR* syncURL) EXTRA_SECTION_02;

        /**
         * Sets the new "beginSync" timestamp.
         *
         * @param timestamp the beginSync timestamp
         */
        void setBeginSync(unsigned long timestamp) EXTRA_SECTION_02;

        /**
         * Returns the beginSync timestamp
         */
        unsigned long getBeginSync() const EXTRA_SECTION_02;

        /**
         * Sets the new "endSync" timestamp.
         *
         * @param timestamp the endSync timestamp
         */
        void setEndSync(unsigned long timestamp) EXTRA_SECTION_02;

        /**
         * Returns the endSync timestamp
         */
        unsigned long getEndSync() const EXTRA_SECTION_02;

        BOOL getServerAuthRequired() const EXTRA_SECTION_02;

        void setServerAuthRequired(BOOL v) EXTRA_SECTION_02;

        const BCHAR* getClientAuthType() const EXTRA_SECTION_02;

        void setClientAuthType(const BCHAR* v) EXTRA_SECTION_02;

        const BCHAR* getServerAuthType() const EXTRA_SECTION_02;

        void setServerAuthType(const BCHAR* v) EXTRA_SECTION_02;

        const BCHAR* getServerPWD() const EXTRA_SECTION_02;

        void setServerPWD(const BCHAR* v) EXTRA_SECTION_02;

        const BCHAR* getServerID() const EXTRA_SECTION_02;

        void setServerID(const BCHAR* v) EXTRA_SECTION_02;

        const BCHAR* getServerNonce() const EXTRA_SECTION_02;

        void setServerNonce(const BCHAR* v) EXTRA_SECTION_02;

        const BCHAR* getClientNonce() const EXTRA_SECTION_02;

        void setClientNonce(const BCHAR* v) EXTRA_SECTION_02;

        void setMaxMsgSize(unsigned long msgSize) EXTRA_SECTION_02;

        unsigned long getMaxMsgSize() const EXTRA_SECTION_02;

        void setMaxModPerMsg(unsigned long msgSize) EXTRA_SECTION_02;

        unsigned long getMaxModPerMsg() const EXTRA_SECTION_02;
        
        void setReadBufferSize(unsigned long bufferSize);

        unsigned long getReadBufferSize() const EXTRA_SECTION_02;

        const BCHAR* getUserAgent() const EXTRA_SECTION_02;

        void setUserAgent(const BCHAR* v) EXTRA_SECTION_02;


        void setCheckConn(BOOL v) EXTRA_SECTION_02;
        BOOL getCheckConn() const EXTRA_SECTION_02;

        void setResponseTimeout(unsigned int bufferSize)    EXTRA_SECTION_02;
        unsigned int getResponseTimeout() const             EXTRA_SECTION_02;

        /**
         * Has some of this values changed?
         */
        unsigned int getDirty() const EXTRA_SECTION_02;

        /**
         * Sets the values of this object with with the values from the given
         * AccessConfig source object.
         *
         * @param config the new value.
         */
        void assign(const AccessConfig& s) EXTRA_SECTION_02;

        BOOL getEncryption() const EXTRA_SECTION_02;
        void setEncryption(BOOL useEncryption) EXTRA_SECTION_02;

        /*
         * Assign operator
         */
        AccessConfig& operator = (const AccessConfig& ac) {
            assign(ac);
            return *this;
        }

};

#endif
