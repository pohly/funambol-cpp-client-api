/*
 * Copyright (C) 2005-2006 Funambol
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



#ifndef INCL_EMI
#define INCL_EMI

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"

class EMI : public ArrayElement {
    
     // ------------------------------------------------------------ Private data
    private:
      
        BCHAR* value;
    // ---------------------------------------------------------- Protected data    
    public:
                
        EMI(BCHAR* value);
        ~EMI();


        // ---------------------------------------------------------- Public methods
        /**
         * Gets the value of experimental meta information
         * 
         * @return the value of experimental meta information
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
