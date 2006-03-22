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


#ifndef INCL_EXT
#define INCL_EXT

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/StringElement.h"



class Ext : public ArrayElement {
    
     // ------------------------------------------------------------ Private data
    private:
        BCHAR* xNam;
        ArrayList* xVal;   // String[]
        void initialize();

    // ---------------------------------------------------------- Public data    
    public:
    
        /**
         * In order to expose the server configuration like WS this constructor
         * must be public
         */
        Ext();
        ~Ext();

        /**
         * Creates a new Ext object with the given name and value
         *
         *  @param xNam corresponds to the &lt;XNam&gt; element - NOT NULL
         *  @param xVal an array of parameters that corresponds to the &ltXVal&gt
         *                  element
         *
         */
        Ext(BCHAR* xNam, ArrayList* xVal);
        
        /**
         * Gets the name of the extension
         *
         * @return the name of the extension
         */
        BCHAR* getXNam(BCHAR* retXNam);

        /**
         * Sets the name of extension
         *
         * @param xNam the name of extension
         *
         */
        void setXNam(BCHAR* xNam);

        /**
         * Gets an array of extension values, if exist
         *
         * @return an array of extension values, if exist
         */
        ArrayList* getXVal();

        /**
         * Sets the array of extension value
         *
         * @param xVal the array of extension value
         *
         */
        void setXVal(ArrayList* xVal);

        ArrayElement* clone();
   
};

#endif
