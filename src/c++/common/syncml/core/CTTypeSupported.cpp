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
 
#include "base/util/utils.h"
#include "syncml/core/CTTypeSupported.h"
 

CTTypeSupported::CTTypeSupported(){
    ctType       = NULL;
    ctPropParams = NULL;    // CTPropParam[]   
}

CTTypeSupported::~CTTypeSupported() {
    if (ctType) {
        delete [] ctType; ctType = NULL;
    }
    if (ctPropParams) {
        ctPropParams->clear(); 
    }
}

/**
 * Creates a new CTTypeSupported object with the given information
 *
 * @param ctType an String CTType - NOT NULL
 * @param ctPropParams the array of content type properties and/or content
 *                     content type parameters - NOT NULL
 *
 */
CTTypeSupported::CTTypeSupported(wchar_t* ctType, ArrayList* ctPropParams ) {
     setCTType(ctType);
     setCTPropParams(ctPropParams);
}


/**
 * Get a CTType String
 *
 * @return a CTType String
 */
wchar_t* CTTypeSupported::getCTType(wchar_t* retCTType) {
    if (retCTType == NULL) {
        return ctType;
    }
    return wcscpy(retCTType, ctType);
}

/**
 * Sets a CTType object
 *
 * @param ctType a CTType object
 */
void CTTypeSupported::setCTType(wchar_t* ctType) {
     if (this->ctType) {
        delete [] this->ctType; this->ctType = NULL;
    }
    this->ctType = stringdup(ctType);
}

/**
 * Gets an array of content type properties and parameters
 *
 * @return an array of content type properties and parameters
 *
 */
ArrayList* CTTypeSupported::getCTPropParams() {
    return ctPropParams;
}

/**
 * Sets an array of content type properties and parameters
 *
 * @param ctPropParams array of content type properties and parameters
 *
 */
void CTTypeSupported::setCTPropParams(ArrayList* ctPropParams) {
     if (this->ctPropParams) {
		this->ctPropParams->clear();
    } 
	this->ctPropParams = ctPropParams->clone();

}

ArrayElement* CTTypeSupported::clone() {
    CTTypeSupported* ret = new CTTypeSupported(ctType, ctPropParams);
    return ret;
}
