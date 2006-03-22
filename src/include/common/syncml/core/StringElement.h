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


/**
* This class represent a String class used to store a single element in a value.
* It can permit to store a string value and set it into an ArrayList.
*
*/

#ifndef INCL_STRING_ELEMENT
#define INCL_STRING_ELEMENT

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"

class StringElement : public ArrayElement {
    

     // ------------------------------------------------------------ Private data
    private:
      
        BCHAR* value;
    // ---------------------------------------------------------- Protected data    
    public:
                
        StringElement(BCHAR* value);
        ~StringElement();


        // ---------------------------------------------------------- Public methods
        /**
         * Gets the value of string element
         * 
         * @return the value of string element
         */
        BCHAR* getValue(BCHAR* retValue);

    
        /**
         * Sets the value of experimental meta information
         *
         * @param value the value of experimental meta information
         *
         */
        void setValue(BCHAR* value);
        
        ArrayElement* clone();

};

#endif
