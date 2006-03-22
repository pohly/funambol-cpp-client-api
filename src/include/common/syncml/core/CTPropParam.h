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


#ifndef INCL_CTPROP_PARAM
#define INCL_CTPROP_PARAM

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/ContentTypeParameter.h"
#include "syncml/core/StringElement.h"

class CTPropParam : public ArrayElement{
    
     // ------------------------------------------------------------ Private data
    private:
        BCHAR*   propName;
        ArrayList* valEnum;       //String[]
        BCHAR*   displayName;
        BCHAR*   dataType;
        int size;
        ArrayList* ctParameters; //ContentTypeParameter[]
    
    // ---------------------------------------------------------- Protected data    
    public:
        CTPropParam();
        ~CTPropParam();

        CTPropParam(BCHAR*   propName,
                    ArrayList* valEnum,
                    BCHAR*   displayName,
                    ArrayList* ctParameters);

    
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
        CTPropParam(BCHAR* propName,
                           BCHAR* dataType,
                           int size,
                           BCHAR* displayName,
                           ArrayList* ctParameters);
        // ---------------------------------------------------------- Public methods

        /**
         * Gets the property name
         *
         * @return the property name
         */
        BCHAR* getPropName(BCHAR* retPropName);
    
        /**
         * Sets the property name
         *
         * @param propName the property name
         */
        void setPropName(BCHAR* propName);
    
        /**
         * Gets the array of value for the property
         *
         * @return the array of value for the property
         */
        ArrayList* getValEnum();
    
        /**
         * Sets the array of enumerated value property
         *
         * @param valEnum the array of enumerated value property
         */
        void setValEnum(ArrayList* valEnum);
    
        /**
         * Gets the display name property
         *
         * @return the display name property
         */
        BCHAR* getDisplayName(BCHAR* retDisplayName);
    
        /**
         * Sets the display name of a given content type property
         *
         * @param displayName the display name of a given content type property
         */
        void setDisplayName(BCHAR* displayName);
    
        /**
         * Gets the data type propeties
         *
         * @return the data type propeties
         */
        BCHAR* getDataType(BCHAR* retDataType);
    
        /**
         * Sets the data type of a given content type property
         *
         * @param dataType the data type of a given content type property
         */
        void setDataType(BCHAR* dataType);

        /**
         * Gets the size propeties
         *
         * @return the size propeties
         */
        int getSize();

        /**
         * Sets the size of a given content type property
         *
         * @param size the size of a given content type property
         *
         */
        void setSize(int size);
    
        /**
         * Gets the array of ContentTypeParameter
         *
         * @return the size propeties
         */
        ArrayList* getContentTypeParameters();
    
        /**
         * Sets an array of content type properties
         *
         * @param ctParameters array of content type properties
         *
         */
        void setContentTypeParameters(ArrayList* ctParameters);
        
        ArrayElement* clone();
};

#endif
