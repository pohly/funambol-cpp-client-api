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


#include "spds/SyncItemStatus.h"

/*
 * Default constructor
 */
SyncItemStatus::SyncItemStatus() {
	key = NULL;
}

/*
 * Constructs a new SyncItemStatus identified by the given key. The key must
 * not be longer than DIM_KEY_SYNC_ITEM_STATUS (see SPDS Constants).
 *
 * @param key - the key
 */
SyncItemStatus::SyncItemStatus(wchar_t* itemStatusKey){
    
    key = new wchar_t[wcslen(itemStatusKey)+1];
    wcscpy(key, itemStatusKey);
    
    data    = 0;
    cmdID   = 0;
    msgRef  = 0;
    cmdRef  = 0;
    
    cmd = NULL;

}

SyncItemStatus::~SyncItemStatus() {
	if (key) {
		delete [] key;
	}
	
	if (cmd) {
		delete [] cmd;
	}
		
}

/*
 * Returns the SyncItemStatus's key. If key is NULL, the internal buffer is
 * returned; if key is not NULL, the value is copied in the caller
 * allocated buffer and the given buffer pointer is returned.
 *
 * @param key - buffer where the key will be stored
 */
wchar_t* SyncItemStatus::getKey(wchar_t* itemStatusKey) {
    if (itemStatusKey == NULL) {
        return key;
    }

    return wcscpy(itemStatusKey, key);
}

/*
 * Changes the SyncItemStatus key. The key must not be longer than DIM_KEY_SYNC_ITEM_STATUS
 * (see SPDS Constants).
 *
 * @param key - the key
 */
void SyncItemStatus::setKey(wchar_t* itemStatusKey) {
	if (key) {
		delete [] key;
	}
	key = new wchar_t[wcslen(itemStatusKey)+1];
    wcscpy(key, itemStatusKey);
}

 /*
 * Returns the SyncItemStatus's command name. If cmd is NULL, the internal buffer is
 * returned; if cmd is not NULL, the value is copied in the caller
 * allocated buffer and the given buffer pointer is returned.
 *
 * @param itemStatusCmd - buffer where the itemStatusCmd will be stored
 */
wchar_t* SyncItemStatus::getCmd(wchar_t* itemStatusCmd) {
    if (itemStatusCmd == NULL) {
        return cmd;
    }

    return wcscpy(itemStatusCmd, cmd);
}


/*
 * Changes the SyncItemStatus cmd. The cmd must not be longer than DIM_COMMAND_SYNC_ITEM_STATUS
 * (see SPDS Constants).
 *
 * @param itemStatusCmd - the itemStatusCmd
 */
void SyncItemStatus::setCmd(wchar_t* itemStatusCmd) {
    if (cmd) {
		delete [] cmd;
	}
	cmd = new wchar_t[wcslen(itemStatusCmd)+1];
    wcscpy(cmd, itemStatusCmd);

}


/*
 * Sets the SyncItemStatus data. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setData(int itemStatusData) {
    data = itemStatusData;
}

/*
 * Returns the SyncItemStatus data variable.
 */
int SyncItemStatus::getData() {
    return data;
}


/*
 * Sets the SyncItemStatus command ID. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setCmdID(int itemStatusCmdID) {
    cmdID = itemStatusCmdID;
}

/*
 * Returns the SyncItemStatus command ID variable.
 */
int SyncItemStatus::getCmdID() {
    return cmdID;
}

/*
 * Sets the SyncItemStatus message referring. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setMsgRef(int itemStatusMsgRef) {
    msgRef = itemStatusMsgRef;
}

/*
 * Returns the SyncItemStatus message referring variable.
 */
int SyncItemStatus::getMsgRef() {
    return msgRef;
}

/*
 * Sets the SyncItemStatus command referring. The passed data are copied into an
 * internal variable.
 */
void SyncItemStatus::setCmdRef(int itemStatusCmdRef) {
    cmdRef = itemStatusCmdRef;
}

/*
 * Returns the SyncItemStatus command referring variable.
 */
int SyncItemStatus::getCmdRef() {
    return cmdRef;
}

ArrayElement* SyncItemStatus::clone() {
	SyncItemStatus* ret = new SyncItemStatus(key);
	
	ret->setCmd(cmd);
	ret->setData(data);
	ret->setCmdRef(cmdRef);
	ret->setMsgRef(msgRef);
	ret->setCmdID(cmdID);
	
	return ret;
}
