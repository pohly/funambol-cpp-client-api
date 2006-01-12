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


class AccessConfig {
    private:
        wchar_t*      username         ;
        wchar_t*      password         ;
        wchar_t*      deviceId         ;
        BOOL          useProxy         ;
        wchar_t*      proxyHost        ;
        int           proxyPort        ;
        wchar_t*      syncURL          ;
        unsigned long beginTimestamp   ;
        unsigned long endTimestamp     ;
        SyncMode      firstTimeSyncMode;

        // added for authentication improvments
        wchar_t* serverNonce           ;  // from client to server
        wchar_t* clientNonce           ;  // from server to client
        wchar_t* serverID              ;
        wchar_t* serverPWD             ;
        wchar_t* clientAuthType        ;
        wchar_t* serverAuthType        ;
        BOOL isServerAuthRequired      ;  // F = FALSE, T = TRUE

        unsigned long maxMsgSize       ;
        unsigned long maxModPerMsg     ;


        unsigned int dirty;

        /**
         * Sets the given buffer with the given value, dealing correctly with
         * NULL values. If a NULL value is passed, the empty string is used.
         *
         * @param buf the destination buffer
         * @param v the new value (CAN BE NULL)
         */
        void set(wchar_t** buf, const wchar_t* v) EXTRA_SECTION_02;

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
        const wchar_t* getUsername(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        /**
         *  Sets the username value. The given data are copied in an internal
         *  buffer so that the caller is assured that the given address can be
         *  released after the call.
         *
         *  @param username the new username value
         */
        void setUsername(const wchar_t* username) EXTRA_SECTION_02;

        /**
         * Returns the password value.
         */
        const wchar_t* getPassword(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        /**
         * Sets a new password value. The given data are copied in an internal
         * buffer so that the caller is assured that the given address can be
         * released after the call.
         *
         * @param password the new password value
         */
        void setPassword(const wchar_t* password) EXTRA_SECTION_02;

        /**
         * Returns the deviceId value.
         */
        const wchar_t* getDeviceId(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        /**
         * Sets a new  deviceId value. The given data are copied in an internal
         * buffer so that the caller is assured that the given address can be
         * released after the call.
         *
         * @param deviceId the new deviceId value
         */
        void setDeviceId(const wchar_t* deviceId) EXTRA_SECTION_02;

        /**
         * Returns the SyncMode that the sync engine should your the first time
         * a source is synced
         */
        SyncMode getFirstTimeSyncMode() EXTRA_SECTION_02;

        /**
         * Sets the SyncMode that the sync engine should your the first time
         * a source is synced
         *
         * @param syncMode the new sync mode
         */
        void setFirstTimeSyncMode(SyncMode syncMode) EXTRA_SECTION_02;

        /**
         * Should the sync engine use a HTTP proxy?
         */
        BOOL getUseProxy() EXTRA_SECTION_02;

        /**
         * Sets if the sync engine should use a HTTP proxy to access the server.
         *
         * @param useProxy FALSE for not use a proxy, TRUE otherwise
         */
        void setUseProxy(BOOL useProxy) EXTRA_SECTION_02;

        /**
         * Returns the proxyHost value.
         */
        const wchar_t* getProxyHost(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        /**
         * Sets a new proxyHost value.
         *
         * @param proxyHost the new proxyHost value
         */
        void setProxyHost(const wchar_t* proxyHost) EXTRA_SECTION_02;

        /**
         * Returns the syncURL value. If the URL does not start with http://
         * (or HTTP://) or https:// (or HTTPS://), http:// is prepended to the
         * given string.
         */
        const wchar_t* getSyncURL(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        /**
         * Sets a new the syncURL value. The given data are copied in an internal
         * buffer so that the caller is assured that the given address can be
         * released after the call.
         *
         * @param syncURL the new syncURL value
         */
        void setSyncURL(const wchar_t* syncURL) EXTRA_SECTION_02;

        /**
         * Sets the new "beginSync" timestamp.
         *
         * @param timestamp the beginSync timestamp
         */
        void setBeginSync(unsigned long timestamp) EXTRA_SECTION_02;

        /**
         * Returns the beginSync timestamp
         */
        unsigned long getBeginSync() EXTRA_SECTION_02;

        /**
         * Sets the new "endSync" timestamp.
         *
         * @param timestamp the endSync timestamp
         */
        void setEndSync(unsigned long timestamp) EXTRA_SECTION_02;

        /**
         * Returns the endSync timestamp
         */
        unsigned long getEndSync() EXTRA_SECTION_02;

        BOOL getServerAuthRequired() EXTRA_SECTION_02;

        void setServerAuthRequired(BOOL v) EXTRA_SECTION_02;

        const wchar_t* getClientAuthType(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        void setClientAuthType(const wchar_t* v) EXTRA_SECTION_02;

        const wchar_t* getServerAuthType(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        void setServerAuthType(const wchar_t* v) EXTRA_SECTION_02;

        const wchar_t* getServerPWD(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        void setServerPWD(const wchar_t* v) EXTRA_SECTION_02;

        const wchar_t* getServerID(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        void setServerID(const wchar_t* v) EXTRA_SECTION_02;

        const wchar_t* getServerNonce(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        void setServerNonce(const wchar_t* v) EXTRA_SECTION_02;

        const wchar_t* getClientNonce(const wchar_t* buf = NULL) EXTRA_SECTION_02;

        void setClientNonce(const wchar_t* v) EXTRA_SECTION_02;

        void setMaxMsgSize(unsigned long msgSize) EXTRA_SECTION_02;

        unsigned long getMaxMsgSize() EXTRA_SECTION_02;

        void setMaxModPerMsg(unsigned long msgSize) EXTRA_SECTION_02;

        unsigned long getMaxModPerMsg() EXTRA_SECTION_02;

        /**
         * Has some of this values changed?
         */
        unsigned int getDirty() EXTRA_SECTION_02;

        /**
         * Sets the values of this object with with the values from the given
         * AccessConfig source object.
         *
         * @param config the new value.
         */
        void assign(AccessConfig& s) EXTRA_SECTION_02;

};

#endif
