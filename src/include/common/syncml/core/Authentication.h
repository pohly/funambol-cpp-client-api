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


#ifndef INCL_AUTHENTICATION
#define INCL_AUTHENTICATION

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/base64.h"
#include "syncml/core/Meta.h"

class Authentication {
    
     // ------------------------------------------------------------ Private data
    private:
        wchar_t* data;
        wchar_t* username;
        wchar_t* password;
        BOOL encode;
        wchar_t* deviceId;
        wchar_t* syncMLVerProto;
        wchar_t* principalId;
        Meta* meta;

        void initialize();
    
    // ---------------------------------------------------------- Public data    
    public:

    // ---------------------------------------------------------- Constructor
        Authentication();
        ~Authentication();
        
       /**
        * used for clone action. It clone every value.
        */
        Authentication(Authentication* auth);
    
        /**
         * Creates a new Authentication object with the given data
         *
         * @param meta the Meta object with authentication type and format
         * @param data the data of authentication
         *
         */
        Authentication(Meta* meta, wchar_t* data);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param data the data of authentication
         *
         */
        Authentication(wchar_t* type, wchar_t* data);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param data the data of authentication
         * @param encode true if data is encoded, false otherwise
         *
         */
        Authentication(wchar_t* type,
                              wchar_t* data,
                              BOOL encode);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param username the username
         * @param password the password
         *
         */
        Authentication(wchar_t* type,
                              wchar_t* username,
                              wchar_t* password);
        
        // ---------------------------------------------------------- Public methods

        void createAuthentication(wchar_t* type, wchar_t* data);

        /**
         * Gets the type property
         *
         * @return the type property
         */
        wchar_t* getType(wchar_t*retType);

        /**
         * Sets the type property
         *
         * @param type the type property
         */
        void setType(wchar_t* type);

        /**
         * Gets the format property
         *
         * @return the format property
         */
        wchar_t* getFormat(wchar_t* retFormat);

        /**
         * Sets the format property
         *
         * @param format the format property
         */
        void setFormat(wchar_t* format);

        /**
         * Gets the data property
         *
         * @return the data property
         */
        wchar_t* getData(wchar_t* retData);

        /**
         * Sets the data property
         *
         * @param data the data property
         *
         */
        void setData(wchar_t* data);


        /**
         * Gets username property
         *
         * @return the username property
         */
        wchar_t* getUsername(wchar_t* retPassword);

        /**
         * Sets the username property
         *
         * @param username the username property
         */
        void setUsername(wchar_t* username);

        /**
         * Gets password property
         *
         * @return the password property
         */
        wchar_t* getPassword(wchar_t* retPassword);

        /**
         * Sets the password property
         *
         * @param password the password property
         */
        void setPassword(wchar_t* password);

        /**
         * Gets the nextNonce property
         *
         * @return nextNonce the nextNonce property
         */
        NextNonce* getNextNonce();

        /**
         * Sets the nextNonce property
         *
         * @param nextNonce the nextNonce property
         *
         */
        void setNextNonce(NextNonce* nextNonce);

        /**
         * Gets the meta property
         *
         * @return meta the meta property
         */
        Meta* getMeta();

        /**
         * Sets the meta property
         *
         * @param meta the meta property
         *
         */
        void setMeta(Meta* meta);

        /**
         * Gets the device id
         *
         * @return deviceId the device identificator
         */
        wchar_t* getDeviceId(wchar_t* retDeviceId);

        /**
         * Sets the device identificator
         *
         * @param deviceId the device identificator
         */
        void setDeviceId(wchar_t* deviceId);

        /**
         * Gets the SyncML Protocol version. It is useful to decide how calculate
         * the digest with MD5 authentication.
         *
         * @return syncMLVerProto the SyncML Protocol version.
         */
        wchar_t* getSyncMLVerProto(wchar_t* retSyncMLVerProto);

        /**
         * Sets the SyncML Protocol version. It is useful to decide how calculate
         * the digest with MD5 authentication.
         *
         * @param syncMLVerProto the SyncML Protocol version.
         *
         */
         void setSyncMLVerProto(wchar_t* syncMLVerProto);

        /**
         * Gets the principal id
         *
         * @return principalId the principal identificator
         */
        wchar_t* getPrincipalId(wchar_t* retPrincipalId);

        /**
         * Sets the principal identificator
         *
         * @param principalId the principal identificator
         */
        void setPrincipalId(wchar_t* principalId);
        
        Authentication* clone();

};

#endif
