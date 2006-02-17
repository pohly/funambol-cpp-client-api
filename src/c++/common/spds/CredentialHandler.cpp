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

void CredentialHandler::setUsername(const wchar_t* t) {
    safeDel(&username);
    username = stringdup(t);
}

void CredentialHandler::setPassword(const wchar_t* t) {
    safeDel(&password);
    password = stringdup(t);
}

void CredentialHandler::setClientAuthType(const wchar_t* t){
    safeDel(&clientAuthType);
    clientAuthType = stringdup(t);
}

const wchar_t* CredentialHandler::getClientAuthType(wchar_t* t){    
    if (t == NULL) {
        return clientAuthType;
    }
    return wcscpy(t, clientAuthType);    
}


void CredentialHandler::setClientNonce(const wchar_t* t){
    safeDel(&clientNonce);
    clientNonce = stringdup(t);
}

const wchar_t* CredentialHandler::getClientNonce(wchar_t* t) {
     if (t == NULL) {
        return clientNonce;
    }
    return wcscpy(t, clientNonce);        
}

void CredentialHandler::setServerID(const wchar_t* t) {
    safeDel(&serverID);
    serverID = stringdup(t);
}

void CredentialHandler::setServerPWD(const wchar_t* t) {
    safeDel(&serverPWD);
    serverPWD = stringdup(t);
}

void CredentialHandler::setServerAuthType(const wchar_t* t) {
    safeDel(&serverAuthType);
    serverAuthType = stringdup(t);
}

const wchar_t* CredentialHandler::getServerAuthType(wchar_t* t) {
     if (t == NULL) {
        return serverAuthType;
    }
    return wcscpy(t, serverAuthType);        
}

void CredentialHandler::setServerNonce(const wchar_t* t) {
    safeDel(&serverNonce);
    serverNonce = stringdup(t); 
}

const wchar_t* CredentialHandler::getServerNonce(wchar_t* t) {
     if (t == NULL) {
        return serverNonce;
    }
    return wcscpy(t, serverNonce);        
}

void CredentialHandler::setServerAuthRequired(BOOL t) {
    isServerAuthRequired = t;    
}

BOOL CredentialHandler::getServerAuthRequired() {
    return isServerAuthRequired;    
}

Cred* CredentialHandler::getClientCredential() {
    
    Authentication* auth = NULL;
    wchar_t* credential  = NULL;
    if (wcscmp(clientAuthType, AUTH_TYPE_BASIC) == 0) {
        auth = new Authentication(AUTH_TYPE_BASIC, username, password);
    } else {
        credential = MD5CredentialData(username, password, clientNonce);
        auth = new Authentication(AUTH_TYPE_MD5, credential);                    
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
    wchar_t* credential  = NULL;
    if (wcscmp(serverAuthType, AUTH_TYPE_BASIC) == 0) {
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
    
    if (wcscmp(cred->getData(NULL), currentCred->getData(NULL)) == 0) {
        ret = TRUE;
    }
finally:
    
    return ret;
}

Chal* CredentialHandler::getServerChal(BOOL isServerAuthenticated) {
    
    Chal* chal = NULL;

    if (wcscmp(serverAuthType, AUTH_TYPE_BASIC) == 0 && isServerAuthenticated == FALSE) {
        chal = Chal::getBasicChal();

    } else if (wcscmp(serverAuthType, AUTH_TYPE_MD5) == 0) { // MD5
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


