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
#include <syncml/core/PropParam.h>


PropParam::PropParam() {
    paramName = dataType = displayName = NULL;
    valEnums = NULL;
}

PropParam::~PropParam() {
    if (paramName  ) delete [] paramName  ;
    if (dataType   ) delete [] dataType   ;
    if (displayName) delete [] displayName;
    if (valEnums)    delete    valEnums   ;
}


PropParam::PropParam(wchar_t* paramName, wchar_t* dataType, ArrayList* valEnums, wchar_t* displayName) {
    this->paramName   = NULL;
    this->dataType    = NULL;
    this->displayName = NULL;
    this->valEnums    = NULL;

    setParamName  (paramName  );
    setDataType   (dataType   );
    setValEnums   (valEnums   );
    setDisplayName(displayName);
}


/*
 * Gets displayName
 *
 * @return  the current displayName's value
 *
 */
wchar_t* PropParam::getDisplayName(wchar_t* displayName) {
    if (displayName == NULL) {
        return this->displayName;
    }

    return wcscpy(displayName, this->displayName);
}

/*
 * Sets displayName
 *
 * @param displayName the new value
 *
 */
void PropParam::setDisplayName(wchar_t* displayName) {
    if (this->displayName) {
        delete [] this->displayName; this->displayName = NULL;
    }

    if (displayName) {
        this->displayName = stringdup(displayName);
    }
}


/*
 * Gets paramName
 *
 * @return  the current paramName's value
 *
 */
wchar_t* PropParam::getParamName(wchar_t* paramName) {
    if (paramName == NULL) {
        return this->paramName;
    }

    return wcscpy(paramName, this->paramName);
}

/*
 * Sets paramName
 *
 * @param paramName the new value
 *
 */
void PropParam::setParamName(wchar_t* paramName) {
    if (this->paramName) {
        delete [] this->paramName; this->paramName = NULL;
    }

    if (paramName) {
        this->paramName = stringdup(paramName);
    }
}

/*
 * Gets dataType
 *
 * @return  the current dataType's value
 *
 */
wchar_t* PropParam::getDataType(wchar_t* dataType) {
    if (dataType == NULL) {
        return this->dataType;
    }

    return wcscpy(dataType, this->dataType);
}

/*
 * Sets dataType
 *
 * @param dataType the new value
 *
 */
void PropParam::setDataType(wchar_t* dataType) {
    if (this->dataType) {
        delete [] this->dataType; this->dataType = NULL;
    }

    if (dataType) {
        this->dataType = stringdup(dataType);
    }
}


ArrayList* PropParam::getValEnums() {
    return valEnums;
}

void PropParam::setValEnums(ArrayList* p0) {
    if (this->valEnums) {
        delete this->valEnums; this->valEnums = NULL;
    }

    if (valEnums) {
        this->valEnums = valEnums->clone();
    }

}

ArrayElement* PropParam::clone() {
  return (ArrayElement*)new PropParam(paramName, dataType, valEnums, displayName);
}

