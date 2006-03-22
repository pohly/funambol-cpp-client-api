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


#ifndef INCL_PUT
#define INCL_PUT

#include "base/fscapi.h"
#include "syncml/core/ItemizedCommand.h"

#define PUT_COMMAND_NAME T("Put")

class Put : public ItemizedCommand {
    
    private:       
        BCHAR* lang;
        BCHAR* COMMAND_NAME;
    
       
    
    // ---------------------------------------------------------- Public data    
    public:

        Put();
        ~Put();
        /**
         * Creates a new Put object given its elements.
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp is &lt;NoResponse/&gt; required?
         * @param lang Preferred language
         * @param cred authentication credentials
         * @param meta meta information
         * @param items Item elements - NOT NULL
         *
         */
        Put( CmdID* cmdID,
                    BOOL noResp,
                    BCHAR* lang,
                    Cred* cred,
                    Meta* meta,
                    ArrayList* items ); // items[]
    

    
       // ----------------------------------------------------------- Public methods
        
        /**
         * Returns the preferred language
         *
         * @return the preferred language
         *
         */
        BCHAR* getLang(BCHAR* retLang);
    
        /**
         * Sets the preferred language
         *
         * @param lang new preferred language
         */
         void setLang(BCHAR* lang);
    
        /**
         * Returns the command name
         *
         * @return the command name
         */
         BCHAR* getName();
    
         ArrayElement* clone();
   
};

#endif
