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
#include "syncml/core/CTPropParam.h"
 

CTPropParam::CTPropParam() {
    propName        = NULL;
    valEnum         = NULL;       //String[]
    displayName     = NULL;
    dataType        = NULL;
    size            = 0;
    ctParameters    = NULL; //ContentTypeParameter[]
}

CTPropParam::~CTPropParam() {
    if (propName)        { delete [] propName;    propName      = NULL; }
    if (valEnum)         { valEnum->clear();                            }       //String[]
    if (displayName)     { delete [] displayName; displayName   = NULL; }
    if (dataType)        { delete    dataType;                          }
    size            = 0;
    if (ctParameters)    { ctParameters->clear(); ctParameters = NULL;  } //ContentTypeParameter[]

}

CTPropParam::CTPropParam(BCHAR*   propName,
                        ArrayList* valEnum,
                        BCHAR*   displayName,
                        ArrayList* ctParameters) {

    setPropName(propName);
    setValEnum(valEnum);
    setContentTypeParameters(ctParameters);
       
    this->displayName  = stringdup(displayName);

}

    
/**
 * Creates a new ContentTypeProperty object with the given name, value and
 * display name
 *
 * @param propName corresponds to &lt;PropName&gt; element in the SyncML 
 *                  specification - NOT NULL
 * @param dataType corresponds to &lt;DataType&gt; element in the SyncML 
 *                  specification
 * @param size corresponds to &lt;Size&gt; element in the SyncML 
 *                  specification
 * @param displayName corresponds to &lt;DisplayName&gt; element in the SyncML 
 *                  specification
 * @param ctParameters the array of content type parameters - NOT NULL
 *
 */
CTPropParam::CTPropParam(BCHAR* propName,
                   BCHAR* dataType,
                   int size,
                   BCHAR* displayName,
                   ArrayList* ctParameters) {

    setPropName(propName);
    setContentTypeParameters(ctParameters);
    
    this->dataType     = stringdup(dataType);
    this->size         = size;
    this->displayName  = stringdup(displayName);

}
// ---------------------------------------------------------- Public methods

/**
 * Gets the property name
 *
 * @return the property name
 */
BCHAR* CTPropParam::getPropName(BCHAR* retPropName) {
    if (retPropName == NULL) {
        return propName;
    }
    return bstrcpy(retPropName, propName);
}

/**
 * Sets the property name
 *
 * @param propName the property name
 */
void CTPropParam::setPropName(BCHAR* propName) {
    if (this->propName) {
        delete [] this->propName; this->propName = NULL;
    }
    this->propName = stringdup(propName);

}

/**
 * Gets the array of value for the property
 *
 * @return the array of value for the property
 */
ArrayList* CTPropParam::getValEnum() {
    return valEnum;
}

/**
 * Sets the array of enumerated value property
 *
 * @param valEnum the array of enumerated value property
 */
void CTPropParam::setValEnum(ArrayList* valEnum) {
    if (this->valEnum) {
		this->valEnum->clear(); 
    } 
	this->valEnum = valEnum->clone();
}

/**
 * Gets the display name property
 *
 * @return the display name property
 */
BCHAR* CTPropParam::getDisplayName(BCHAR* retDisplayName) {
    if (retDisplayName == NULL) {
        return displayName;
    }
    return bstrcpy(retDisplayName, displayName);
}

/**
 * Sets the display name of a given content type property
 *
 * @param displayName the display name of a given content type property
 */
void CTPropParam::setDisplayName(BCHAR* displayName) {
    if (this->displayName) {
        delete [] this->displayName; this->displayName = NULL;
    }
    this->displayName = stringdup(displayName);
}

/**
 * Gets the data type propeties
 *
 * @return the data type propeties
 */
BCHAR* CTPropParam::getDataType(BCHAR* retDataType) {
    if (retDataType == NULL) {
        return dataType;
    }
    return bstrcpy(retDataType, dataType);
}

/**
 * Sets the data type of a given content type property
 *
 * @param dataType the data type of a given content type property
 */
void CTPropParam::setDataType(BCHAR* dataType) {
    if (this->dataType) {
        delete [] this->dataType; this->dataType = NULL;
    }
    this->dataType = stringdup(dataType);
}

/**
 * Gets the size propeties
 *
 * @return the size propeties
 */
int CTPropParam::getSize() {
    return size;
}

/**
 * Sets the size of a given content type property
 *
 * @param size the size of a given content type property
 *
 */
void CTPropParam::setSize(int size) {
    this->size = size;
}

/**
 * Gets the array of ContentTypeParameter
 *
 * @return the size propeties
 */
ArrayList* CTPropParam::getContentTypeParameters() {
     return ctParameters;
}   

/**
 * Sets an array of content type properties
 *
 * @param ctParameters array of content type properties
 *
 */
void CTPropParam::setContentTypeParameters(ArrayList* ctParameters) {
    if (this->ctParameters) {
		this->ctParameters->clear(); 
    } 
	this->ctParameters = ctParameters->clone();
}

ArrayElement* CTPropParam::clone() {
    CTPropParam* ret = new CTPropParam(propName, valEnum, displayName, ctParameters);
    return ret;
}
