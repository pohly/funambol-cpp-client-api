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
Authentication::Authentication(Meta* meta, wchar_t* data){
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
Authentication::Authentication(wchar_t* type, wchar_t* data) {
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
Authentication::Authentication(wchar_t* type,
                      wchar_t* data,
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
Authentication::Authentication(wchar_t* type,
                      wchar_t* username,
                      wchar_t* password) {
    
    
    initialize();

    if (username == NULL || password == NULL) {
           // tbd
    }

    encode = TRUE;
    wchar_t auth[DIM_512];
    wsprintf(auth, TEXT("%s:%s"), username, password);
    createAuthentication(type, auth);

}

void Authentication::createAuthentication(wchar_t* type, wchar_t* data) {
    
    BOOL del = FALSE;    

    if (wcsstr(AUTH_SUPPORTED_TYPES, type) == NULL) {
        type = new wchar_t[DIM_64];
        wsprintf(type, AUTH_TYPE_BASIC);         
        del = TRUE;
    }

    if (wcscmp(type, AUTH_TYPE_BASIC) == 0) {
        this->setType(AUTH_TYPE_BASIC);
        this->setFormat(FORMAT_B64);
        this->setData(data);
    } else if (wcscmp(type, AUTH_TYPE_MD5) == 0) {
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
wchar_t* Authentication::getType(wchar_t* retType) {
    
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
void Authentication::setType(wchar_t* type) {
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
wchar_t* Authentication::getFormat(wchar_t* retFormat) {
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
void Authentication::setFormat(wchar_t* format) {
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
wchar_t* Authentication::getData(wchar_t* retData) {
    
    if (retData == NULL) {
        return data;
    }
    return wcscpy(retData, data);
}

/**
 * Sets the data property
 *
 * @param data the data property
 *
 */
void Authentication::setData(wchar_t* data) {

    if (data == NULL) {
        // TBD
        return;
    }
    
    wchar_t* type = this->getType(NULL);

    if (wcscmp(type,AUTH_TYPE_BASIC) == 0) {
        wchar_t* clearData = NULL;

        if (encode) {
            unsigned long len = 0;
            len = utf8len(data);
            char* tmp = wc2utf8(data, NULL, 0);        
            char* b64tmp2 = new char[(len/3+1)<<2];
            len = b64_encode(b64tmp2, tmp, len);                        
            char* b64tmp = new char[len + 1];
            memset(b64tmp, 0, len + 1);
            strncpy(b64tmp, b64tmp2, len);
            if (this->data) {
                delete [] this->data; this->data = NULL;
            }
            this->data = utf82wc(b64tmp, NULL, 0);                         
            
            clearData = new wchar_t[wcslen(data) + 1];
            wsprintf(clearData, data);
            
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
            len = utf8len(data);
            char* tmp = wc2utf8(data, NULL, 0);        
            char* b64tmp = new char[len];
            len = b64_decode(b64tmp, tmp);                                            
            
            clearData = utf82wc(b64tmp, NULL, 0);
            if (this->data) {
                delete [] this->data; this->data = NULL;
            }
            //this->data = new wchar_t[wcslen(clearData) + 1];            
            //wsprintf(this->data, clearData);
            this->data = stringdup(data);

            if (tmp) {
                delete [] tmp; tmp = NULL;
            }
            if (b64tmp) {
                delete [] b64tmp; b64tmp = NULL;
            }            
        }
        
        unsigned int len = wcslen(clearData);
        wchar_t* p1 = clearData;
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
            wchar_t* p2 = p1 - 1;
            *p2 = 0;
            if (wcslen(clearData) > 0 ) {
                this->setUsername(clearData);
            } else {
                this->setUsername(TEXT(""));
            }
            if (wcslen(p1) > 0) {
                this->setPassword(p1);
            } else {
                this->setPassword(TEXT(""));
            }
            
        }
    }

    if (wcscmp(type, AUTH_TYPE_MD5) == 0) {
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
wchar_t* Authentication::getUsername(wchar_t* retUsername) {
    if (retUsername == NULL) {
        return username;
    }
    return wcscpy(retUsername, username);


}

/**
* Sets the username property
*
* @param username the username property
*/
void Authentication::setUsername(wchar_t* username) {
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
wchar_t* Authentication::getPassword(wchar_t* retPassword) {
    if (retPassword == NULL) {
        return password;
    }
    return wcscpy(retPassword, password);
}

/**
* Sets the password property
*
* @param password the password property
*/
void Authentication::setPassword(wchar_t* password) {
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
wchar_t* Authentication::getDeviceId(wchar_t* retDeviceId) {
    if (retDeviceId == NULL) {
        return deviceId;
    }
    return wcscpy(retDeviceId, deviceId);


}

/**
 * Sets the device identificator
 *
 * @param deviceId the device identificator
 */
void Authentication::setDeviceId(wchar_t* deviceId) {
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
wchar_t* Authentication::getSyncMLVerProto(wchar_t* retSyncMLVerProto) {
    if (retSyncMLVerProto == NULL) {
        return syncMLVerProto;
    }
    return wcscpy(retSyncMLVerProto, syncMLVerProto);    
}

/**
 * Sets the SyncML Protocol version. It is useful to decide how calculate
 * the digest with MD5 authentication.
 *
 * @param syncMLVerProto the SyncML Protocol version.
 *
 */
void Authentication::setSyncMLVerProto(wchar_t* syncMLVerProto) {
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
wchar_t* Authentication::getPrincipalId(wchar_t* retPrincipalId) {
    if (retPrincipalId == NULL) {
        return principalId;
    }
    return wcscpy(retPrincipalId, principalId);    
}
/**
 * Sets the principal identificator
 *
 * @param principalId the principal identificator
 */
void Authentication::setPrincipalId(wchar_t* principalId) {
     if (this->principalId) {
        delete [] this->principalId; this->principalId = NULL;
    }
    this->principalId = stringdup(principalId);
}


Authentication* Authentication::clone() {

    Authentication* ret = new Authentication(this);
    return ret;
}

