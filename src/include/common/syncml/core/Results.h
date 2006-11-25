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


#ifndef INCL_RESULTS
#define INCL_RESULTS

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/ResponseCommand.h"

#define RESULTS_COMMAND_NAME "Results"

class Results : public ResponseCommand {
    
    private:               
        char*  COMMAND_NAME;    
    
    // ---------------------------------------------------------- Public data    
    public:
        Results();
        ~Results();
    
        /**
         * Creates a new Results object.
         *
         * @param cmdID command identifier - NOT NULL
         * @param msgRef message reference
         * @param cmdRef command reference - NOT NULL
         * @param meta meta information
         * @param targetRef target reference
         * @param sourceRef source reference
         * @param items command items
         *        
         *
         */
        Results(CmdID*      cmdID,
                char*     msgRef,
                char*     cmdRef,
                Meta*       meta,
                ArrayList*  targetRef,  // it could be and arraylist with only the first element assigned
                ArrayList*  sourceRef,  // it could be and arraylist with only the first element assigned
                ArrayList*  items); //Item[] 
        
        /**
         * Returns the command name.
         *
         * @return the command name
         */
        char*  getName();

        ArrayElement* clone();
   
};

#endif
