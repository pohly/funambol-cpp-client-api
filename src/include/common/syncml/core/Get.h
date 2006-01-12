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


#ifndef INCL_GET
#define INCL_GET

#include "base/fscapi.h"
#include "syncml/core/ItemizedCommand.h"

#define GET_COMMAND_NAME TEXT("Get")

class Get : public ItemizedCommand {
    
    private:       
        wchar_t* lang;
        wchar_t* COMMAND_NAME;
    
    public:
       
        Get();
        ~Get();

        /** 
         * Creates a new Get object with the given command identifier, 
         * noResponse, language, credential, meta and an array of item
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp true if no response is required
         * @param lang the preferred language for results data
         * @param cred the authentication credential
         * @param meta the meta information
         * @param items the array of item - NOT NULL
         *
         */
        Get(CmdID* cmdID,
            BOOL noResp,
            wchar_t* lang,
            Cred* cred,
            Meta* meta,
            ArrayList* items);

        
        /**
         * Returns the preferred language
         *
         * @return the preferred language
         *
         */
        wchar_t* getLang(wchar_t* retLang);
    
        /**
         * Sets the preferred language
         *
         * @param lang new preferred language
         */
        void setLang(wchar_t* lang);

        /**
         * Gets the command name property
         *
         * @return the command name property
         */   
        wchar_t* getName();
    
        ArrayElement* clone();

       
};

#endif
