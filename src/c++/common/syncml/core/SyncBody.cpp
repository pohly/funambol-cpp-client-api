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
 
 
#include "syncml/core/SyncBody.h"
 
SyncBody::SyncBody() {
    initialize();
}
SyncBody::~SyncBody() {
    if(commands) {
        commands->clear(); 
    }
    finalMsg = FALSE;
}

/**
* Create a new SyncBody object. The commands in <i>commands</i>
* must be of the allowed types.
*
* @param commands The array elements must be an instance of one of these
*                 classes: {@link Alert},  {@link Atomic}, {@link Copy},
*                 {@link Exec}, {@link Get}, {@link Map}, {@link Put},
*                 {@link Results}, {@link Search}, {@link Sequence},
*                 {@link Status}, {@link Sync}
* @param finalMsg is true if this is the final message that is being sent
*
*/
SyncBody::SyncBody(ArrayList* commands   , // AbstractCommand[]
                   BOOL       finalMsg) {
        
        initialize();
        setCommands(commands);
        setFinalMsg(finalMsg);
}

void SyncBody::initialize() {
    finalMsg = FALSE;
    commands = new ArrayList();
}

/**
*
*  @return the return value is guaranteed to be non-null. Also,
*          the elements of the array are guaranteed to be non-null.
*
*/
ArrayList* SyncBody::getCommands() {
    return commands;
}
    

/**
* Sets the sequenced commands. The given commands must be of the allowed
* types.
*
* @param commands the commands - NOT NULL and o the allowed types
*
*/
void SyncBody::setCommands(ArrayList* commands) {
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
* Sets the message as final
*
* @param finalMsg the Boolean value of finalMsg property
*/
void SyncBody::setFinalMsg(BOOL finalMsg) {
      if ((finalMsg == NULL) || (finalMsg != TRUE && finalMsg != FALSE)) {
        this->finalMsg = NULL;
    } else {
        this->finalMsg = finalMsg;
    }  
}

/**
* Gets the value of finalMsg property
*
* @return true if this is the final message being sent, otherwise false
*
*/
BOOL SyncBody::isFinalMsg() {
    return (finalMsg != NULL);
}

/**
* Gets the value of finalMsg property
*
* @return true if this is the final message being sent, otherwise null
*
*/
BOOL SyncBody::getFinalMsg() {
    return finalMsg;
}

SyncBody* SyncBody::clone() {
    SyncBody* ret = new SyncBody(commands, finalMsg);
    return ret;
}
