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


#include "spds/CredentialHandler.h"

/*
 * Default constructor
 */
CredentialHandler::CredentialHandler() {
    initialize();
}

/**
 * Initializes private members
 */
void CredentialHandler::initialize() {
    username       = NULL;
    password       = NULL;
    clientAuthType = NULL;
    clientNonce    = NULL;
                  
    serverID       = NULL;
    serverPWD      = NULL;
    serverAuthType = NULL;
    serverNonce    = NULL;
}

/*
 * Destructor. Free the allocated memory (if any)
 */
CredentialHandler::~CredentialHandler() {
    safeDel(&username       );
    safeDel(&password       );
    safeDel(&clientAuthType );
    safeDel(&clientNonce    );
    
    safeDel(&serverID       );
    safeDel(&serverPWD      );
    safeDel(&serverAuthType );
    safeDel(&serverNonce    );

}

void CredentialHandler::setUsername(const BCHAR* t) {
    safeDel(&username);
    username = stringdup(t);
}

void CredentialHandler::setPassword(const BCHAR* t) {
    safeDel(&password);
    password = stringdup(t);
}

void CredentialHandler::setClientAuthType(const BCHAR* t){
    safeDel(&clientAuthType);
    clientAuthType = stringdup(t);
}

const BCHAR* CredentialHandler::getClientAuthType(BCHAR* t){    
    if (t == NULL) {
        return clientAuthType;
    }
    return bstrcpy(t, clientAuthType);    
}


void CredentialHandler::setClientNonce(const BCHAR* t){
    safeDel(&clientNonce);
    clientNonce = stringdup(t);
}

const BCHAR* CredentialHandler::getClientNonce(BCHAR* t) {
     if (t == NULL) {
        return clientNonce;
    }
    return bstrcpy(t, clientNonce);        
}

void CredentialHandler::setServerID(const BCHAR* t) {
    safeDel(&serverID);
    serverID = stringdup(t);
}

void CredentialHandler::setServerPWD(const BCHAR* t) {
    safeDel(&serverPWD);
    serverPWD = stringdup(t);
}

void CredentialHandler::setServerAuthType(const BCHAR* t) {
    safeDel(&serverAuthType);
    serverAuthType = stringdup(t);
}

const BCHAR* CredentialHandler::getServerAuthType(BCHAR* t) {
     if (t == NULL) {
        return serverAuthType;
    }
    return bstrcpy(t, serverAuthType);        
}

void CredentialHandler::setServerNonce(const BCHAR* t) {
    safeDel(&serverNonce);
    serverNonce = stringdup(t); 
}

const BCHAR* CredentialHandler::getServerNonce(BCHAR* t) {
     if (t == NULL) {
        return serverNonce;
    }
    return bstrcpy(t, serverNonce);        
}

void CredentialHandler::setServerAuthRequired(BOOL t) {
    isServerAuthRequired = t;    
}

BOOL CredentialHandler::getServerAuthRequired() {
    return isServerAuthRequired;    
}

Cred* CredentialHandler::getClientCredential() {
    
    Authentication* auth = NULL;
    BCHAR* credential  = NULL;
    if (bstrcmp(clientAuthType, AUTH_TYPE_BASIC) == 0) {
        auth = new Authentication(AUTH_TYPE_BASIC, username, password);
    } else {
        credential = MD5CredentialData(username, password, clientNonce);
        auth = new Authentication(AUTH_TYPE_MD5, credential);                   
        if (credential) { delete [] credential; credential = NULL; }
    }
    
    Cred* cred           = new Cred(auth);    
    
    deleteAuthentication(&auth);    
    return cred;

}

/*
* it returns the server credential. The method is used both during the request of authentication
* and the creation of the status as response of server authentication.
* Therefore, if the server is arleady authenticated, no credential are sent back. 
*/


Cred* CredentialHandler::getServerCredential() {
    
    Authentication* auth = NULL;
    Cred* cred           = NULL;
    BCHAR* credential  = NULL;
    if (bstrcmp(serverAuthType, AUTH_TYPE_BASIC) == 0) {
        auth = new Authentication(AUTH_TYPE_BASIC, serverID, serverPWD);
    } else {
        credential = MD5CredentialData(serverID, serverPWD, serverNonce);
        auth = new Authentication(AUTH_TYPE_MD5, credential);                    
    }    
    
    cred = new Cred(auth);        

    deleteAuthentication(&auth);    
    return cred;

}

BOOL CredentialHandler::performServerAuth(Cred* cred) {
    
    BOOL ret = FALSE;
    Cred* currentCred = getServerCredential();
    
    if (cred == NULL || currentCred == NULL) {
        goto finally;
    }    
    
    if (bstrcmp(cred->getData(NULL), currentCred->getData(NULL)) == 0) {
        ret = TRUE;
    }
finally:
    
    return ret;
}

Chal* CredentialHandler::getServerChal(BOOL isServerAuthenticated) {
    
    Chal* chal = NULL;

    if (bstrcmp(serverAuthType, AUTH_TYPE_BASIC) == 0 && isServerAuthenticated == FALSE) {
        chal = Chal::getBasicChal();

    } else if (bstrcmp(serverAuthType, AUTH_TYPE_MD5) == 0) { // MD5
        chal = Chal::getMD5Chal();
        char nonce[16];
        generateNonce(nonce);
        NextNonce* nextNonce = new NextNonce(nonce, 16);
        chal->setNextNonce(nextNonce);
        setServerNonce(nextNonce->getValueAsBase64());
    }
        
    return chal;
}


// private
void CredentialHandler::generateNonce(char nonce[16]) {
    srand((unsigned int)time(NULL));
    for (unsigned int i = 0; i < 16; i++) {
        nonce[i] = ((rand()%100) * (rand()%100))%100;

        if (nonce[i] < 32) {
            nonce[i] +=96;
        }
    }
}


