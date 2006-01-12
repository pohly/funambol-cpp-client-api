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


#ifndef INCL_COMPLEX_DATA
#define INCL_COMPLEX_DATA

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/Data.h"
#include "syncml/core/Anchor.h"
#include "syncml/core/DevInf.h"

class ComplexData : public Data {
    
     // ------------------------------------------------------------ Private data
    private:
        Anchor* anchor;
        DevInf* devInf;
        ArrayList* properties;  // array of Property objects
        
        void initialize();

    // ---------------------------------------------------------- Public data    
    public:
    
        ComplexData();
        ~ComplexData();
    
        /**
         * Creates a Data object from the given anchors string.
         *
         * @param data the data
         *
         */
        ComplexData(const wchar_t* data);
    
        // ---------------------------------------------------------- Public methods

        /**
         * Gets the Anchor object property
         * 
         * @return anchor the Anchor object
         */
        Anchor* getAnchor();
    
        /**
         * Sets the Anchor object property
         *
         * @param anchor the Anchor object
         */
        void setAnchor(Anchor* anchor);

        /**
         * Gets the DevInf object property
         *
         * @return devInf the DevInf object property
         */
        DevInf* getDevInf();
    
        /**
         * Sets the DevInf object property
         *
         * @param devInf the DevInf object property
         *
         */
        void setDevInf(DevInf* devInf);

        /*
        * Gets properties
        *
        * @return  the current properties's value
        *
        */
        ArrayList* getProperties();
        
        /*
        * Sets properties
        *
        * @param properties the new value
        *
        */
        void setProperties(ArrayList* properties);


        ComplexData* clone();
   
};

#endif
