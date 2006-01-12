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



#ifndef INCL_FILTER
#define INCL_FILTER

#include "syncml/core/Item.h"
#include "syncml/core/Meta.h"

/**
 * This class represents a SyncML 1.2 Filter element.
 * 
 */

class Filter {
    // ------------------------------------------------------------ Private data
    private:
       Item*   field      ;
       Item*   record     ;
       wchar_t* filterType;
       Meta*    meta;

       void initialize();
    
    // ------------------------------------------------------------- Public data
    public:

        /**
         * Creates a new Filter object.
         */
        Filter();

        /**
         * Creates a new Filter object.
         *
         * @param meta       the meta information - NOT NULL
         * @param field      the field item data
         * @param record     the record item data
         * @param filterType the type of filtering
         */
        Filter(Meta*    meta      ,
               Item*    field     ,
               Item*    record    ,
               wchar_t* filterType);

        ~Filter();

        /**
         * Returns the filter meta element
         *
         * @return the filter meta element
         */
        Meta* getMeta();
    
        /**
         * Sets the filter meta
         *
         * @param the filter meta element
         *
         */
        void setMeta(Meta* meta);

        /**
         * Returns the filter field element
         *
         * @return the filter field element
         */
        Item* getField();
    
        /**
         * Sets the filter field
         *
         * @param the filter field element
         *
         */
        void setField(Item* meta);

        /**
         * Returns the filter record element
         *
         * @return the filter record element
         */
        Item* getRecord();
    
        /**
         * Sets the filter record
         *
         * @param the filter record element
         *
         */
        void setRecord(Item* meta);

        /**
         * Returns the filter type
         *
         * @return the filter type
         */
        wchar_t* getFilterType(wchar_t* type = NULL);
    
        /**
         * Sets the filter type
         *
         * @param the filter type
         *
         */
        void setFilterType(wchar_t* type);

        /**
         * Creates a clone of this Filter
         *
         * @return the newly created instance
         */
        Filter* clone();
};

#endif
