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


#ifndef INCL_SOURCE_REF
#define INCL_SOURCE_REF

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/Source.h"


class SourceRef : public ArrayElement{
    
     // ------------------------------------------------------------ Private data
    private:
         wchar_t* value;
         Source*  source;
    
    // ---------------------------------------------------------- Protected data    
    public:
        SourceRef();
        ~SourceRef();
    
        /**
         * Creates a new SourceRef object given the referenced value. A null value
         * is considered an empty string
         *
         * @param value the referenced value - NULL ALLOWED
         *
         */
        SourceRef(const wchar_t* value);
    
        /**
         * Creates a new SourceRef object from an existing Source.
         *
         * @param source the source to extract the reference from - NOT NULL
         *         
         *
         */
        SourceRef(Source* source);
    
        // ---------------------------------------------------------- Public methods
    
        /**
         * Returns the value
         *
         * @return the value
         */
        const wchar_t* getValue();
    
        /**
         * Sets the reference value. If value is null, the empty string is adopted.
         *
         * @param value the reference value - NULL
         */
        void setValue(const wchar_t* value);
    
        /**
         * Gets the Source property
         * 
         * @return source the Source object property
         */
        Source* getSource();
    
        /**
         * Sets the Source property
         * 
         * @param source the Source object property - NOT NULL
         */
        void setSource(Source* source);

        ArrayElement* clone();
   
};

#endif
