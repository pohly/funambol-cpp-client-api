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
#include "syncml/core/Data.h"
 

Data::Data() {
    initialize();
}

Data::~Data() {
    if (data) {
        delete [] data; data = NULL;
    }
}

/** 
* Creates a new Data object with the given data value
*
* @param data the data value
*
*/
Data::Data(const BCHAR* data) {
    initialize();
    setData(data);
}

void Data::initialize() {
    data = NULL;
}

/** 
* Creates a new Data object with the given data value
*
* @param data the data value
*
*/
Data::Data(long data) {
    initialize();    
    BCHAR tmp[DIM_64];
    bsprintf(tmp, T("%i"), data);
    setData(tmp);
}

// ---------------------------------------------------------- Public methods

/**
* Sets the data property
*
* @param data the data property
*/
void Data::setData(const BCHAR* data) {
    if (this->data) {
        delete [] this->data; this->data = NULL;
    }

    this->data = stringdup(data);
}

/**
* Gets the data properties
*
* @return the data properties
*/
BCHAR* Data::getData(BCHAR* retData) {
    if (retData == NULL) {
        return data;
    }
    return bstrcpy(retData, data);
}

Data* Data::clone() {
    Data* ret = new Data(data);
    return ret;
}
