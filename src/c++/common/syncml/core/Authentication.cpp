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
 
 
#include "syncml/core/Authentication.h"
#include "syncml/core/Constants.h"
#include "syncml/core/Cred.h"


Authentication::Authentication() {

    initialize();
}

Authentication::~Authentication() {

     if (data)      {delete [] data; data = NULL; }
     if (username)  {delete [] username; username = NULL; }
     if (password)  {delete [] password; password = NULL; }
     if (deviceId)  {delete [] deviceId; deviceId = NULL; }
     if (syncMLVerProto) {delete [] syncMLVerProto; syncMLVerProto = NULL; }
     if (principalId)    {delete [] principalId; principalId = NULL; }
     if (meta)           {delete  meta; meta = NULL; }
     
     encode = FALSE;     

}

Authentication::Authentication(Authentication* auth) {
    
    initialize();
    this->data            = stringdup(auth->getData(NULL));
    this->username        = stringdup(auth->getUsername(NULL));
    this->password        = stringdup(auth->getPassword(NULL));
    this->encode          = auth->encode;
    this->deviceId        = stringdup(auth->getDeviceId(NULL));
    this->syncMLVerProto  = stringdup(auth->getSyncMLVerProto(NULL));
    this->principalId     = stringdup(auth->getPrincipalId(NULL));
    this->meta            = auth->getMeta()->clone();

}
    

void Authentication::initialize() {
    this->data            = NULL;
    this->username        = NULL;
    this->password        = NULL;
    this->encode          = FALSE;
    this->deviceId        = NULL ;
    this->syncMLVerProto  = NULL ;
    this->principalId     = NULL;
    this->meta            = NULL;
}


/**
 * Creates a new Authentication object with the given data
 *
 * @param meta the Meta object with authentication type and format
 * @param data the data of authentication
 *
 */
Authentication::Authentication(Meta* meta, char* data){
    initialize();
    
    this->meta = meta->clone();
    createAuthentication(meta->getType(NULL), data);

}

/**
 * Creates a new Authentication object with the given data
 *
 * @param type the authentication type
 * @param data the data of authentication
 *
 */
Authentication::Authentication(char* type, char* data) {
    initialize();
    
    createAuthentication(type, data);
}

/**
 * Creates a new Authentication object with the given data
 *
 * @param type the authentication type
 * @param data the data of authentication
 * @param encode true if data is encoded, false otherwise
 *
 */
Authentication::Authentication(char* type,
                      char* data,
                      BOOL encode) {

    
    initialize();
    
    this->encode = encode;
    createAuthentication(type, data);
}


/**
 * Creates a new Authentication object with the given data
 *
 * @param type the authentication type
 * @param username the username
 * @param password the password
 *
 */
Authentication::Authentication(char* type,
                      char* username,
                      char* password) {
    
    
    initialize();

    if (username == NULL || password == NULL) {
           // tbd
    }

    encode = TRUE;
    char auth[DIM_512];
    sprintf(auth, "%s:%s", username, password);
    createAuthentication(type, auth);

}

void Authentication::createAuthentication(char* type, char* data) {
    
    BOOL del = FALSE;    

    if (strstr(AUTH_SUPPORTED_TYPES, type) == NULL) {
        type = new char[DIM_64];
        sprintf(type, AUTH_TYPE_BASIC);         
        del = TRUE;
    }

    if (strcmp(type, AUTH_TYPE_BASIC) == 0) {
        this->setType(AUTH_TYPE_BASIC);
        this->setFormat(FORMAT_B64);
        this->setData(data);
    } else if (strcmp(type, AUTH_TYPE_MD5) == 0) {
        this->setType(AUTH_TYPE_MD5);
        this->setData(data);
    }
    
    if (del) {
        delete [] type; type = NULL;
    }
}

 /**
 * Gets the type property
 *
 * @return the type property
 */
char* Authentication::getType(char* retType) {
    
    if (meta == NULL) {
        return NULL;
    }
    return meta->getType(retType);
}

/**
 * Sets the type property
 *
 * @param type the type property
 */
void Authentication::setType(char* type) {
    if (meta == NULL) {
        meta = new Meta();
    }
    meta->setType(type);

}

/**
 * Gets the format property
 *
 * @return the format property
 */
char* Authentication::getFormat(char* retFormat) {
    if (meta == NULL) {
        return NULL;
    }
    return meta->getFormat(retFormat);

}

/**
 * Sets the format property
 *
 * @param format the format property
 */
void Authentication::setFormat(char* format) {
    if (meta == NULL) {
        meta = new Meta();
    }
    meta->setFormat(format);
}

/**
 * Gets the data property
 *
 * @return the data property
 */
char* Authentication::getData(char* retData) {
    
    if (retData == NULL) {
        return data;
    }
    return strcpy(retData, data);
}

/**
 * Sets the data property
 *
 * @param data the data property
 *
 */
void Authentication::setData(char* data) {

    if (data == NULL) {
        // TBD
        return;
    }
    
    char* type = this->getType(NULL);

    if (strcmp(type,AUTH_TYPE_BASIC) == 0) {
        char* clearData = NULL;

        if (encode) {
            unsigned long len = 0;
            len = strlen(data);
            char* tmp = stringdup(data);        
            char* b64tmp2 = new char[(len/3+1)<<2];
            len = b64_encode(b64tmp2, tmp, len);                        
            char* b64tmp = new char[len + 1];
            memset(b64tmp, 0, len + 1);
            strncpy(b64tmp, b64tmp2, len);
            if (this->data) {
                delete [] this->data; this->data = NULL;
            }
            this->data = stringdup(b64tmp);                         
            
            clearData = new char[strlen(data) + 1];
            sprintf(clearData, data);
            
            if (b64tmp2) {                
                delete [] b64tmp2; b64tmp2 = NULL;
            }
            if (b64tmp) {
                delete [] b64tmp; b64tmp = NULL;
            }
            if (tmp) {
                delete [] tmp; tmp = NULL;
            }
            
        } else {
            unsigned long len = 0;
            len = strlen(data);
            char* tmp = stringdup(data);        
            char* b64tmp = new char[len];
            len = b64_decode(b64tmp, tmp);                                            
            
            clearData = stringdup(b64tmp);
            if (this->data) {
                delete [] this->data; this->data = NULL;
            }
            //this->data = new char[strlen(clearData) + 1];            
            //sprintf(this->data, clearData);
            this->data = stringdup(data);

            if (tmp) {
                delete [] tmp; tmp = NULL;
            }
            if (b64tmp) {
                delete [] b64tmp; b64tmp = NULL;
            }            
        }
        
        unsigned int len = strlen(clearData);
        char* p1 = clearData;
        BOOL charFound = FALSE;
        for (unsigned int k = 0; k < len; k++) {             
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == ':') {
                charFound = TRUE;
                p1 = p1 + 1;
                break;
            }            
            p1 = p1 + 1;
        }        

        if (charFound == FALSE) {
            this->setUsername(clearData);
            this->setPassword(NULL);
        } else {
            char* p2 = p1 - 1;
            *p2 = 0;
            if (strlen(clearData) > 0 ) {
                this->setUsername(clearData);
            } else {
                this->setUsername("");
            }
            if (strlen(p1) > 0) {
                this->setPassword(p1);
            } else {
                this->setPassword("");
            }
            
        }
        if (clearData) { delete [] clearData; clearData = NULL; }
    }

    if (strcmp(type, AUTH_TYPE_MD5) == 0) {
        if (meta->getFormat(NULL) == NULL) {
            this->setFormat(FORMAT_B64);
        }
        this->setUsername(data);
        this->data     = stringdup(data);
    }
}


/**
* Gets username property
*
* @return the username property
*/
char* Authentication::getUsername(char* retUsername) {
    if (retUsername == NULL) {
        return username;
    }
    return strcpy(retUsername, username);


}

/**
* Sets the username property
*
* @param username the username property
*/
void Authentication::setUsername(char* username) {
    if (this->username) {
        delete [] this->username; this->username = NULL;
    }
    this->username = stringdup(username);
}

/**
* Gets password property
*
* @return the password property
*/
char* Authentication::getPassword(char* retPassword) {
    if (retPassword == NULL) {
        return password;
    }
    return strcpy(retPassword, password);
}

/**
* Sets the password property
*
* @param password the password property
*/
void Authentication::setPassword(char* password) {
    if (this->password) {
        delete [] this->password; this->password = NULL;
    }
    this->password = stringdup(password);
}

/* Gets the nextNonce property
 *
 * @return nextNonce the nextNonce property
 */
NextNonce* Authentication::getNextNonce() {
    if (meta == NULL) {
        return NULL;
    }
    return meta->getNextNonce();

}

/**
 * Sets the nextNonce property
 *
 * @param nextNonce the nextNonce property
 *
 */
void Authentication::setNextNonce(NextNonce* nextNonce) {
     if (meta == NULL)  {
        meta = new Meta();
     }
     meta->setNextNonce(nextNonce);
}

/**
 * Gets the meta property
 *
 * @return meta the meta property
 */
Meta* Authentication::getMeta() {
    return meta;
}

/**
 * Sets the meta property
 *
 * @param meta the meta property
 *
 */
void Authentication::setMeta(Meta* meta) {
    if (this->meta) {
		delete  this->meta; this->meta = NULL;
	}
    this->meta = meta->clone();
}

/**
 * Gets the device id
 *
 * @return deviceId the device identificator
 */
char* Authentication::getDeviceId(char* retDeviceId) {
    if (retDeviceId == NULL) {
        return deviceId;
    }
    return strcpy(retDeviceId, deviceId);


}

/**
 * Sets the device identificator
 *
 * @param deviceId the device identificator
 */
void Authentication::setDeviceId(char* deviceId) {
    if (this->deviceId) {
        delete [] this->deviceId; this->deviceId = NULL;
    }
    this->deviceId = stringdup(deviceId);
}

/**
 * Gets the SyncML Protocol version. It is useful to decide how calculate
 * the digest with MD5 authentication.
 *
 * @return syncMLVerProto the SyncML Protocol version.
 */
char* Authentication::getSyncMLVerProto(char* retSyncMLVerProto) {
    if (retSyncMLVerProto == NULL) {
        return syncMLVerProto;
    }
    return strcpy(retSyncMLVerProto, syncMLVerProto);    
}

/**
 * Sets the SyncML Protocol version. It is useful to decide how calculate
 * the digest with MD5 authentication.
 *
 * @param syncMLVerProto the SyncML Protocol version.
 *
 */
void Authentication::setSyncMLVerProto(char* syncMLVerProto) {
    if (this->syncMLVerProto) {
        delete [] this->syncMLVerProto; this->syncMLVerProto = NULL;
    }
    this->syncMLVerProto = stringdup(syncMLVerProto);
}

/**
 * Gets the principal id
 *
 * @return principalId the principal identificator
 */
char* Authentication::getPrincipalId(char* retPrincipalId) {
    if (retPrincipalId == NULL) {
        return principalId;
    }
    return strcpy(retPrincipalId, principalId);    
}
/**
 * Sets the principal identificator
 *
 * @param principalId the principal identificator
 */
void Authentication::setPrincipalId(char* principalId) {
     if (this->principalId) {
        delete [] this->principalId; this->principalId = NULL;
    }
    this->principalId = stringdup(principalId);
}


Authentication* Authentication::clone() {

    Authentication* ret = new Authentication(this);
    return ret;
}

