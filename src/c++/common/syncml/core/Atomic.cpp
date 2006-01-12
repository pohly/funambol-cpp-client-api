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
 
 
#include "syncml/core/Atomic.h"
     
Atomic::Atomic() {
    COMMAND_NAME = new wchar_t[wcslen(ATOMIC_COMMAND_NAME) + 1];
    wsprintf(COMMAND_NAME, ATOMIC_COMMAND_NAME);
    commands = new ArrayList();
}

Atomic::~Atomic() {
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
    if (commands) {
        commands->clear(); 
    }
}

/** 
* Creates a new Atomic object with the given command identifier, noResponse,
* meta and an array of abstract command
*
* @param cmdID the command identifier - NOT NULL
* @param noResp is true if no response is required
* @param meta the meta data
* @param commands an array of abstract command - NOT NULL
*/
Atomic::Atomic( CmdID*     cmdID,
                BOOL       noResp,
                Meta*      meta,
                ArrayList* commands) : AbstractCommand(cmdID) {
    
    this->commands = new ArrayList();
    COMMAND_NAME = new wchar_t[wcslen(ATOMIC_COMMAND_NAME) + 1];
    wsprintf(COMMAND_NAME, ATOMIC_COMMAND_NAME);

    setNoResp(noResp);
    setMeta(meta);
    setCommands(commands);

}

/**
* Gets an array of AbstractCommand
*
* @return an array of command objects
*/
ArrayList* Atomic::getCommands() {
    return commands;
}

/**
* Sets an array of AbstractCommand
*
* @param commands the array of AbstractCommand
*
*/
void Atomic::setCommands(ArrayList* commands) {
     BOOL err = FALSE;
    if (commands == NULL) {
        // TBD
        err = TRUE;
    }        
    for (int i = 0; i < commands->size(); i++) {
        if (commands->get(i) == NULL) {
            // TBD
            err = TRUE;
        } 
    }
    if (err == FALSE) {
        this->commands->clear(); 
        this->commands = commands->clone();
    }
}
/**
* Gets the command name property
*
* @return the command name property
*/ 
wchar_t* Atomic::getName() {
    return COMMAND_NAME;
}

ArrayElement* Atomic::clone() {
    Atomic* ret = new Atomic(getCmdID(), getNoResp(), getMeta(), commands);
    return ret;
    
}
