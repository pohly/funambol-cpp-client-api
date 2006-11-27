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



#ifndef INCL_CMD_ID
#define INCL_CMD_ID

#include "base/fscapi.h"

/**
 * This class implements an abstract command. It must be derived to be used by other classes.
 * 
 */

class CmdID {
    
     // ------------------------------------------------------------ Private data
    private:
        char*  cmdID;
            
    
    // ---------------------------------------------------------- Protected data    
    public:

    CmdID();
    ~CmdID();

     /**
     * Creates a new CmdID object with the given String cmdID
     *
     * @param cmdID the cmdID of CmdID - NOT NULL
     *
     */
    CmdID(const char*  cmdID);

    /**
     * Creates a new CmdID object with the given numeric cmdID
     *
     * @param cmdID the cmdID of CmdID
     *
     */
    CmdID(long cmdID);

    // ---------------------------------------------------------- Public methods

    /**
     * Gets cmdID properties
     *
     * @return cmdID properties
     */
    const char* getCmdID();

    CmdID* clone();

};

#endif
