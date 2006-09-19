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
        char*  data;
        char*  username;
        char*  password;
        BOOL encode;
        char*  deviceId;
        char*  syncMLVerProto;
        char*  principalId;
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
        Authentication(Meta* meta, char*  data);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param data the data of authentication
         *
         */
        Authentication(char*  type, char*  data);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param data the data of authentication
         * @param encode true if data is encoded, false otherwise
         *
         */
        Authentication(char*  type,
                              char*  data,
                              BOOL encode);

        /**
         * Creates a new Authentication object with the given data
         *
         * @param type the authentication type
         * @param username the username
         * @param password the password
         *
         */
        Authentication(char*  type,
                              char*  username,
                              char*  password);
        
        // ---------------------------------------------------------- Public methods

        void createAuthentication(char*  type, char*  data);

        /**
         * Gets the type property
         *
         * @return the type property
         */
        char*  getType(char* retType);

        /**
         * Sets the type property
         *
         * @param type the type property
         */
        void setType(char*  type);

        /**
         * Gets the format property
         *
         * @return the format property
         */
        char*  getFormat(char*  retFormat);

        /**
         * Sets the format property
         *
         * @param format the format property
         */
        void setFormat(char*  format);

        /**
         * Gets the data property
         *
         * @return the data property
         */
        char*  getData(char*  retData);

        /**
         * Sets the data property
         *
         * @param data the data property
         *
         */
        void setData(char*  data);


        /**
         * Gets username property
         *
         * @return the username property
         */
        char*  getUsername(char*  retPassword);

        /**
         * Sets the username property
         *
         * @param username the username property
         */
        void setUsername(char*  username);

        /**
         * Gets password property
         *
         * @return the password property
         */
        char*  getPassword(char*  retPassword);

        /**
         * Sets the password property
         *
         * @param password the password property
         */
        void setPassword(char*  password);

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
        char*  getDeviceId(char*  retDeviceId);

        /**
         * Sets the device identificator
         *
         * @param deviceId the device identificator
         */
        void setDeviceId(char*  deviceId);

        /**
         * Gets the SyncML Protocol version. It is useful to decide how calculate
         * the digest with MD5 authentication.
         *
         * @return syncMLVerProto the SyncML Protocol version.
         */
        char*  getSyncMLVerProto(char*  retSyncMLVerProto);

        /**
         * Sets the SyncML Protocol version. It is useful to decide how calculate
         * the digest with MD5 authentication.
         *
         * @param syncMLVerProto the SyncML Protocol version.
         *
         */
         void setSyncMLVerProto(char*  syncMLVerProto);

        /**
         * Gets the principal id
         *
         * @return principalId the principal identificator
         */
        char*  getPrincipalId(char*  retPrincipalId);

        /**
         * Sets the principal identificator
         *
         * @param principalId the principal identificator
         */
        void setPrincipalId(char*  principalId);
        
        Authentication* clone();

};

#endif
