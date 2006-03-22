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


#ifndef INCL_CONTENT_TYPE_PARAMETER
#define INCL_CONTENT_TYPE_PARAMETER

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/StringElement.h"

class ContentTypeParameter : public ArrayElement {
    
     // ------------------------------------------------------------ Private data
    private:
        BCHAR*   paramName;
        ArrayList* valEnum;   //String[]
        BCHAR*   displayName;
        BCHAR*   dataType;
        int size;
    
    // ---------------------------------------------------------- Protected data    
    public:
        
        ContentTypeParameter();
        ~ContentTypeParameter();

        /**
         * Creates a new ContentTypeParameter object with the given name, value and
         * display name
         *
         * @param paramName corresponds to &lt;ParamName&gt; element in the SyncML 
         *                  specification - NOT NULL
         * @param valEnum   corresponds to &lt;ValEnum&gt; element in the SyncML 
         *                  specification
         * @param displayName corresponds to &lt;DisplayName&gt; element in the SyncML 
         *                  specification
         *
         */
        ContentTypeParameter(BCHAR* paramName,
                             ArrayList* valEnum, 
                             BCHAR* displayName);

        /**
         * Creates a new ContentTypeParameter object with the given name, data type,
         * size, display name
         *
         * @param paramName corresponds to &lt;ParamName&gt; element in the SyncML 
         *                  specification - NOT NULL
         * @param dataType  corresponds to &lt;DataType&gt; element in the SyncML 
         *                  specification
         * @param size      corresponds to &lt;Size&gt; element in the SyncML 
         *                  specification
         * @param displayName corresponds to &lt;DisplayName&gt; element in the SyncML 
         *                  specification
         *
         */
        ContentTypeParameter(BCHAR* paramName,
                             BCHAR* dataType,
                             int size,
                             BCHAR* displayName);

        
        /**
         * Gets the parameter name propeties
         *
         * @return the parameter name propeties
         */
        BCHAR*  getParamName(BCHAR* retParamName);
    
        /**
         * Sets the param name property
         *
         * @param paramName the param name property
         */
        void setParamName(BCHAR* paramName);
    
        /**
         * Gets the array of value for parameter
         *
         * @return the array of value for parameter
         */
        ArrayList* getValEnum();
    
        /**
         * Sets the array of enumerated value property
         *
         * @param valEnum the array of enumerated value property
         */
        void setValEnum(ArrayList* valEnum);
    
        /**
         * Gets the display name propeties
         *
         * @return the display name propeties
         */
        BCHAR* getDisplayName(BCHAR* retDisplayName);
    
        /**
         * Sets the display name of a given content type parameter
         *
         * @param displayName the display name of a given content type parameter
         *
         */
        void setDisplayName(BCHAR* displayName);
    
        /**
         * Gets the data type propeties
         *
         * @return the data type propeties
         */
        BCHAR* getDataType(BCHAR* retDataType);
    
        /**
         * Sets the data type of a given content type parameter
         *
         * @param dataType the data type of a given content type parameter
         *
         */
        void setDataType(BCHAR* dataType);
    
        /**
         * Gets the size propeties
         *
         * @return the size propeties
         */
        int getSize();
    
        /**
         * Sets the size of a given content type parameter
         *
         * @param size the size of a given content type parameter
         *
         */
        void setSize(int size);

        ArrayElement* clone();
   
};

#endif
