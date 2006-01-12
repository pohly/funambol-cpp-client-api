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
 
 
#include "syncml/core/CmdID.h"
#include "base/util/utils.h"
 

CmdID::CmdID() {
    this->cmdID = NULL;
}

/**
 * Creates a new CmdID object with the given String cmdID
 *
 * @param cmdID the cmdID of CmdID - NOT NULL
 *
 */
CmdID::CmdID(wchar_t* cmdID) {

    this->cmdID = NULL;
    if ((cmdID == NULL) || (wcslen(cmdID) == 0)) {
        // tbd
    }
    this->cmdID = stringdup(cmdID);
}

CmdID::~CmdID() {
    if (cmdID) {
        delete [] cmdID; cmdID = NULL;    
    }
    
}

/**
 * Creates a new CmdID object with the given numeric cmdID
 *
 * @param cmdID the cmdID of CmdID
 *
 */
CmdID::CmdID(long cmdID) {
    wchar_t t[64];
    wsprintf(t, TEXT("%i"), cmdID);
    this->cmdID = stringdup(t);
}

/**
 * Gets cmdID properties
 *
 * @return cmdID properties
 */
wchar_t* CmdID::getCmdID() {
    return cmdID;
}

CmdID* CmdID::clone() {
    CmdID* ret = new CmdID(cmdID);
    return ret;

}
