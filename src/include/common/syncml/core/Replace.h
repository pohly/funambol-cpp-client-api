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


#ifndef INCL_REPLACE
#define INCL_REPLACE

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "syncml/core/ModificationCommand.h"

#define REPLACE_COMMAND_NAME T("Replace")

class Replace : public ModificationCommand {
    
     // ------------------------------------------------------------ Private data
    private:
        char*  COMMAND_NAME;
    
    public:
        
        Replace();
        ~Replace();
        /**
         * Creates a new Replace object.
         *
         * @param cmdID the command identifier - NOT NULL
         * @param noResp is &lt;NoResponse&gt; required?
         * @param cred authentication credentials
         * @param meta meta information
         * @param items command items
         *
         */
        Replace(CmdID *cmdID   ,
                BOOL noResp    ,
                Cred* cred     ,
                Meta*  meta    ,
                ArrayList* items  );
    
    
        /** 
         * Returns the command name
         *
         * @return the command name
         */
        char*  getName();

        ArrayElement* clone();
   
};

#endif
