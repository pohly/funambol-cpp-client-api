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
 
 
#include "syncml/core/DevInfData.h"
 

DevInfData::DevInfData() {
    devInf = NULL;
}
DevInfData::~DevInfData() {
    if (devInf) {
        delete devInf; devInf = NULL;
    }
}

/**
* Creates a new DevInfData object with the given parameter
*
* @param devInf the DevInf object - NOT NULL
*
*/
DevInfData::DevInfData(DevInf* devInf) {
    setDevInf(devInf);
}

/**
* Gets the devInf object
*
* @return devInf the devInf object
*/
DevInf* DevInfData::getDevInf() {
    return devInf;
}

/**
* Sets the DevInf object
* 
* @param devInf the DevInf object
*
*/
void DevInfData::setDevInf(DevInf* devInf) {
    if (devInf == NULL) {
        // TBD
    } else {
        if (this->devInf) {
            delete this->devInf; this->devInf = NULL;
        }
        this->devInf = devInf->clone();
    }    
}

DevInfData* DevInfData::clone() {
    DevInfData* ret = new DevInfData(devInf);
    ret->setData(getData());
    return ret;

}
