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
 
 
#include "syncml/core/Replace.h"
 
Replace::Replace() {
    COMMAND_NAME = new BCHAR[bstrlen(REPLACE_COMMAND_NAME) + 1];
    bsprintf(COMMAND_NAME, REPLACE_COMMAND_NAME);
}

Replace::~Replace() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}

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
Replace::Replace(CmdID *cmdID   ,
        BOOL noResp    ,
        Cred* cred     ,
        Meta*  meta    ,
        ArrayList* items  ): ModificationCommand(cmdID, meta, items) {
   
    setNoResp(noResp); 
    setCred(cred);

    COMMAND_NAME = new BCHAR[bstrlen(REPLACE_COMMAND_NAME) + 1];
    bsprintf(COMMAND_NAME, REPLACE_COMMAND_NAME);
}
/** 
 * Returns the command name
 *
 * @return the command name
 */
BCHAR* Replace::getName(){
    return COMMAND_NAME;
}

ArrayElement* Replace::clone() {
    Replace* ret = new Replace(getCmdID(),getNoResp(), getCred(), getMeta(), getItems());
    return ret;
}
