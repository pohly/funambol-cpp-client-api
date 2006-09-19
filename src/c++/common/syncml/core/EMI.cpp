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
#include "syncml/core/EMI.h"

 
EMI::EMI(char* value) {
    this->value = NULL;
    setValue(value);
}

EMI::~EMI() {
    if (value) {
        delete [] value; value = NULL;
    }
}


/**
 * Gets the value of experimental meta information
 * 
 * @return the value of experimental meta information
 */
char* EMI::getValue(char* retValue) {
    if (retValue == NULL) {
        return value;
    }
    return strcpy(retValue, value);

}
    
/**
 * Sets the value of experimental meta information
 *
 * @param value the value of experimental meta information
 *
 */
void EMI::setValue(char* value) {
    if (value == NULL || strlen(value) == 0) {
        // tbd
    }        
    this->value = stringdup(value);
}

ArrayElement* EMI::clone() {
    EMI* ret = NULL;
    if (this) {
        ret = new EMI(value);  
    }
    return ret;
}
