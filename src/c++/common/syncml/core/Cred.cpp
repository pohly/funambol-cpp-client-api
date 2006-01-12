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
 
 
#include "syncml/core/Cred.h"
 

Cred::Cred() {
    authentication = NULL;
}

Cred::~Cred() {
    if (authentication) {
        delete authentication; authentication = NULL;
    }
}

Cred::Cred(Authentication* auth) {
    authentication = NULL;
    if (auth == NULL) {
        // TBD
    }   
    this->authentication = auth->clone();
    
}

/**
 * Gets type property
 *
 * @return type property
 */
wchar_t* Cred::getType(wchar_t* retType) {

    if (retType == NULL) {
        return authentication->getType(NULL);
    }
    return wcscpy(retType, authentication->getType(NULL));
}

/**
 * Gets format property
 *
 * @return format property
 */
wchar_t* Cred::getFormat(wchar_t* retFormat) {
    if (retFormat == NULL) {
        return authentication->getFormat(NULL);
    }
    return wcscpy(retFormat, authentication->getFormat(NULL));
}

/**
 * Gets data property
 *
 * @return data property
 */
wchar_t* Cred::getData(wchar_t* retData) {
 if (retData == NULL) {
        return authentication->getData(NULL);
    }
    return wcscpy(retData, authentication->getData(NULL));
}

/**
 * Gets the username stored in this credential
 *
 * @return the username stored in this credential
 */
wchar_t* Cred::getUsername(wchar_t* retUsername) {
    if (retUsername == NULL) {
        return authentication->getUsername(NULL);
    }
    return wcscpy(retUsername, authentication->getUsername(NULL));
}


/**
 * Create and return the Authentication object corresponding to the given
 * type and data.
 *
 * @param type the type of the required Authentication object
 * @param data the data to be interpreted based on the type
 *
 * @return the corresponding Authentication object.
 */
Authentication* Cred::createAuthentication(wchar_t* data, wchar_t* type) {
    return new Authentication(data, type);
}

/**
 * Gets the Authentication object.
 *
 * @return authentication the authentication objects
 */
Authentication* Cred::getAuthentication() {
    return authentication;
}

/**
 * Sets the Authentication object.
 *
 * @param auth the new Authentication object
 *
 */
void Cred::setAuthentication(Authentication* auth) {
    if (this->authentication) {
        delete  this->authentication; this->authentication = NULL;
    }
    this->authentication = auth->clone();
}

Cred* Cred::clone() {
    Cred* ret = new Cred(authentication);
    return ret;
}
