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


#ifndef INCL_CONTENT_TYPE_INFO
#define INCL_CONTENT_TYPE_INFO

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"


class ContentTypeInfo : public ArrayElement {
    
     // ------------------------------------------------------------ Private data
    private:
        wchar_t* ctType;
        wchar_t* verCT;
    
    
    // ---------------------------------------------------------- Protected data    
    public:
    
        
        ContentTypeInfo();
        ~ContentTypeInfo();

        /**
         * Creates a new ContentTypeCapability object with the given content type 
         * and versione
         *
         * @param ctType corresponds to &lt;CTType&gt; element in the SyncML 
         *                    specification - NOT NULL
         * @param verCT corresponds to &lt;VerCT&gt; element in the SyncML 
         *                specification - NOT NULL
         *
         */
        ContentTypeInfo(wchar_t* ctType, wchar_t* verCT);

        /**
         * Gets the content type properties
         *
         * @return the content type properties
         */
        wchar_t* getCTType(wchar_t* retCTType);
    
        /**
         * Sets the content type properties
         *
         * @param ctType the content type properties
         */
        void setCTType(wchar_t* ctType);

        /**
         * Gets the version of the content type
         *
         * @return the version of the content type
         */
        wchar_t* getVerCT(wchar_t* retVerCT);

        /**
         * Sets the version of the content type
         *
         * @param verCT the version of the content type
         */
        void setVerCT(wchar_t* verCT);

        ArrayElement* clone();
};

#endif
