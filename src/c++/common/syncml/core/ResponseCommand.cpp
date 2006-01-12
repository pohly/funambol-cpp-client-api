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
 
 
#include "syncml/core/ResponseCommand.h"
 
ResponseCommand::ResponseCommand() {
    msgRef     = NULL;
    cmdRef     = NULL;
    targetRef  = new ArrayList(); // TargetRef[]
    sourceRef  = new ArrayList(); // SourceRef[]                        

}
ResponseCommand::~ResponseCommand()  {
    if (msgRef)     { delete [] msgRef;     msgRef    = NULL; }
    if (cmdRef)     { delete [] cmdRef;     cmdRef    = NULL; }
    if (targetRef)  { targetRef->clear();                     }
    if (sourceRef)  { sourceRef->clear();                     }
    
}

/**
 * Creates a new ResponseCommand object.
 *
 * @param cmdID the command idendifier  - NOT NULL
 * @param msgRef message reference
 * @param cmdRef command reference - NOT NULL
 * @param targetRefs target references
 * @param sourceRefs source references
 * @param items command items
 * if any of the NOT NULL parameter is null
 */
ResponseCommand::ResponseCommand(CmdID*         cmdID     ,
                                 wchar_t*       msgRef    ,
                                 wchar_t*       cmdRef    ,
                                 ArrayList*     targetRefs,
                                 ArrayList*     sourceRefs,
                                 ArrayList*     items      ) : ItemizedCommand (cmdID, items) {

    
    this->msgRef     = NULL;
    this->cmdRef     = NULL;
    this->targetRef  = new ArrayList(); // TargetRef[]
    this->sourceRef  = new ArrayList(); // SourceRef[]              

    setCmdRef(cmdRef);
    setMsgRef(msgRef);
    setTargetRef(targetRefs);
    setSourceRef(sourceRefs);       
}
    

/**
 * Returns the message reference
 *
 * @return the message reference
 *
 */
wchar_t* ResponseCommand::getMsgRef(wchar_t* retGetMsgRef) {
    if (retGetMsgRef == NULL) {
        return msgRef;
    }    
    return wcscpy(retGetMsgRef, msgRef);
}

/**
 * Sets the message reference
 *
 * @param msgRef message reference
 */
void ResponseCommand::setMsgRef(wchar_t* msgRef) {
     if (this->msgRef) {
        delete [] this->msgRef; this->msgRef = NULL;
    }
    this->msgRef = stringdup(msgRef);
}

/**
 * Returns the command reference
 *
 * @return the command reference
 *
 */
wchar_t* ResponseCommand::getCmdRef(wchar_t* retGetCmdRef) {
    if (retGetCmdRef == NULL) {
        return cmdRef;
    }
    return wcscpy(retGetCmdRef, cmdRef);
}

/**
 * Sets the command reference
 *
 * @param cmdRef commandreference - NOT NULL
 *
 */
void ResponseCommand::setCmdRef(wchar_t* cmdRef) {
     if (cmdRef == NULL) {
            // TBD
     } else {
         if (this->cmdRef) {
             delete [] this->cmdRef; this->cmdRef = NULL;
         }
         this->cmdRef = stringdup(cmdRef);
     }
}

/**
 * Returns the target references
 *
 * @return the target references
 *
 */
ArrayList* ResponseCommand::getTargetRef() {
    return targetRef;
}

/**
 * Sets the target references
 *
 * @param targetRefs target refrences
 */
void ResponseCommand::setTargetRef(ArrayList* targetRefs) {
    if (this->targetRef) {
        this->targetRef->clear(); 
    }
    if (targetRefs != NULL) {                  
        this->targetRef = targetRefs->clone();        
    }
}

/**
 * Returns the source references
 *
 * @return the source references
 *
 */
ArrayList* ResponseCommand::getSourceRef(){
    return sourceRef;
}

/**
 * Sets the source references
 *
 * @param sourceRefs source refrences
 */
void ResponseCommand::setSourceRef(ArrayList* sourceRefs) {
    if (this->sourceRef) {
        this->sourceRef->clear(); 
    }
    if (sourceRefs != NULL) {                  
        this->sourceRef = sourceRefs->clone();        
    }
}

