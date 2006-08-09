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

 #ifndef INCL_CREDENTIAL_HANDLER
    #define INCL_CREDENTIAL_HANDLER

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "base/util/utils.h"
    #include "spds/constants.h"
    #include "syncml/core/TagNames.h"
    #include "syncml/core/ObjectDel.h"

    class CredentialHandler{

    private:

        BCHAR* username;
        BCHAR* password;
        BCHAR* clientAuthType;
        BCHAR* clientNonce;
        
        BCHAR* serverID;
        BCHAR* serverPWD;
        BCHAR* serverAuthType;
        BCHAR* serverNonce;
        
        BOOL isServerAuthRequired;

        /**
         * Initializes private members
         */
        void initialize() EXTRA_SECTION_01;
        void  generateNonce(char nonce[16]) EXTRA_SECTION_01;

    public:
        /*
         * Default constructor
         */
        CredentialHandler() EXTRA_SECTION_01;

        ~CredentialHandler() EXTRA_SECTION_01;

        /*
         * Constructs a new SyncItem identified by the given key. The key must
         * not be longer than DIM_KEY (see SPDS Constants).
         *
         * @param key - the key
         */
        CredentialHandler(const BCHAR* key) EXTRA_SECTION_01;

        /*
         * Returns the SyncItem's key. If key is NULL, the internal buffer is
         * returned; if key is not NULL, the value is copied in the caller
         * allocated buffer and the given buffer pointer is returned.
         *
         * @param key - buffer where the key will be stored
         */        
        
        void setUsername(const BCHAR* t) EXTRA_SECTION_01;
        const BCHAR* getUsername() EXTRA_SECTION_01;
        void setPassword(const BCHAR* t) EXTRA_SECTION_01;
        const BCHAR* getPassword() EXTRA_SECTION_01;
        void setClientAuthType(const BCHAR* t) EXTRA_SECTION_01;
        void setClientNonce(const BCHAR* t) EXTRA_SECTION_01;
        const BCHAR* getClientAuthType(BCHAR* t=0 /* deprecated */) EXTRA_SECTION_01;
        const BCHAR* getClientNonce(BCHAR* t=0 /* deprecated */) EXTRA_SECTION_01;

        void setServerID(const BCHAR* t) EXTRA_SECTION_01;
        void setServerPWD(const BCHAR* t) EXTRA_SECTION_01;
        void setServerAuthType(const BCHAR* t) EXTRA_SECTION_01;
        void setServerNonce(const BCHAR* t) EXTRA_SECTION_01;
        const BCHAR* getServerAuthType(BCHAR* t=0 /* deprecated */) EXTRA_SECTION_01;
        const BCHAR* getServerNonce(BCHAR* t=0 /* deprecated */) EXTRA_SECTION_01;
        
        void setServerAuthRequired(BOOL t) EXTRA_SECTION_01;
        BOOL getServerAuthRequired() EXTRA_SECTION_01;
        
        Cred* getClientCredential() EXTRA_SECTION_01;
        Cred* getServerCredential() EXTRA_SECTION_01;
        Chal* getServerChal(BOOL isServerAuthenticated) EXTRA_SECTION_01;
        BOOL  performServerAuth(Cred* cred) EXTRA_SECTION_01;

    };

#endif
