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


#ifndef INCL_ITEMIZED_COMMAND
#define INCL_ITEMIZED_COMMAND

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/Meta.h"
#include "syncml/core/Item.h"
#include "syncml/core/CmdID.h"
#include "syncml/core/AbstractCommand.h"

class ItemizedCommand : public AbstractCommand {
    
     // ------------------------------------------------------------ Private data
    protected:
        ArrayList* items;  // Item[]
        Meta*      meta ;                
        
        void initialize();

    // ---------------------------------------------------------- Protected data    
    public:
        
        ItemizedCommand();
        ~ItemizedCommand();
        /**
         * Create a new ItemizedCommand object with the given commandIdentifier, 
         * meta object and an array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param meta the meta object
         * @param items an array of item - NOT NULL
         *
         */
        ItemizedCommand(CmdID* cmdID, Meta* meta, ArrayList* items);
    
        /**
         * Create a new ItemizedCommand object with the given commandIdentifier
         * and an array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param items an array of item - NOT NULL
         *
         */
        ItemizedCommand(CmdID*  cmdID, ArrayList* items);
        
        /**
         * Gets the array of items
         *
         * @return the array of items
         */    
        ArrayList* getItems();
    
        /**
         * Sets an array of Item object
         *
         * @param items an array of Item object
         */    
        void setItems(ArrayList* items);
    
        /**
         * Gets the Meta object
         *
         * @return the Meta object
         */
        Meta* getMeta();

        /**
         * Sets the Meta object
         *
         * @param meta the Meta object
         *
         */
        void setMeta(Meta* meta);
    
        /**
         * Gets the name of the command
         *
         * @return the name of the command
         */
        virtual wchar_t* getName() = 0;
        
        virtual ArrayElement* clone() = 0;
};  

#endif
