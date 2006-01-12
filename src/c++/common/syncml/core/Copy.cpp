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
 
 
#include "syncml/core/Copy.h"
 

Copy::Copy() {
    COMMAND_NAME = new wchar_t[wcslen(COPY_COMMAND_NAME) + 1];
    wsprintf(COMMAND_NAME, COPY_COMMAND_NAME);
}
Copy::~Copy() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}

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
Copy::Copy(CmdID* cmdID,
     BOOL noResp,
     Cred* cred,
     Meta* meta,
     ArrayList* items) : ModificationCommand(cmdID, meta, items) {
   
    setNoResp(noResp); 
    setCred(cred);

    COMMAND_NAME = new wchar_t[wcslen(COPY_COMMAND_NAME) + 1];
    wsprintf(COMMAND_NAME, COPY_COMMAND_NAME);


}

// ----------------------------------------------------------- Public methods

/**
 * Gets the command name property
 *
 * @return the command name property
 */    
wchar_t* Copy::getName() {
    return COMMAND_NAME;
}

ArrayElement* Copy::clone() {
    Copy* ret = new Copy(getCmdID(), getNoResp(), getCred(), getMeta(), getItems());
    return ret;
}
