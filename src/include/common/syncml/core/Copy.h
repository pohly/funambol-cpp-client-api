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


#ifndef INCL_COPY
#define INCL_COPY

#include "base/fscapi.h"

#include "syncml/core/ModificationCommand.h"

#define COPY_COMMAND_NAME T("Copy")

class Copy : public ModificationCommand {
    
     // ------------------------------------------------------------ Private data
    private:
        BCHAR* COMMAND_NAME;
    
    // ---------------------------------------------------------- Public data    
    public:            
        
        Copy();
        ~Copy();
        
        /** 
         * Creates a new Copy object with the given command identifier, noResponse, 
         * credential, meta and array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp true if no response is required
         * @param cred the authentication credential
         * @param meta the meta data
         * @param items the array of item - NOT NULL
         *
         */
        Copy(CmdID* cmdID,
             BOOL noResp,
             Cred* cred,
             Meta* meta,
             ArrayList* items);

       // ----------------------------------------------------------- Public methods

        /**
         * Gets the command name property
         *
         * @return the command name property
         */    
        BCHAR* getName();

        ArrayElement* clone();
};

#endif
