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

#include "base/fscapi.h"
#include "event/FireEvent.h"
#include "event/ManageListener.h"


//
// Fire a SyncEvent
//
bool fireSyncEvent(const char* msg, int type) {

    ManageListener& manage = ManageListener::getInstance();
    if(manage.synclistener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncEvent event(type, timestamp);
    if(msg) {
        event.setMessage(msg);
    }

    switch(type) {
      case SYNC_BEGIN:
          manage.synclistener->syncBegin(event);
          break;
      case SYNC_END:
          manage.synclistener->syncEnd(event);
          break;
      case SEND_INITIALIZATION:
          manage.synclistener->sendInitialization(event);
          break;
      case SEND_MODIFICATION:
          manage.synclistener->sendModifications(event);
          break;
      case SEND_FINALIZATION:
          manage.synclistener->sendFinalization(event);
          break;
      case SYNC_ERROR:
          manage.synclistener->syncError(event);
          break;
      default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a Transport Event
//
bool fireTransportEvent(unsigned long size, int type) {

    ManageListener& manage = ManageListener::getInstance();
    if(manage.transportlistener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    TransportEvent event(size, type, timestamp);

    switch(type) {
        case SEND_DATA_BEGIN:
          manage.transportlistener->sendDataBegin(event);
          break;
        case SEND_DATA_END:
          manage.transportlistener->syncDataEnd(event);
          break;
        case RECEIVE_DATA_BEGIN:
          manage.transportlistener->receiveDataBegin(event);
          break;
        case RECEIVE_DATA_END:
          manage.transportlistener->receiveDataEnd(event);
          break;
        case DATA_RECEIVED:
          manage.transportlistener->receivingData(event);
          break;
        default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a SyncSourceEvent
//
bool fireSyncSourceEvent(const char* sourceURI, SyncMode mode, int type) {

    ManageListener& manage = ManageListener::getInstance();
    if(manage.syncsourcelistener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncSourceEvent event(sourceURI, mode,type, timestamp);

    switch(type) {
      case SYNC_SOURCE_BEGIN:
          manage.syncsourcelistener->syncSourceBegin(event);
          break;
      case SYNC_SOURCE_END:
          manage.syncsourcelistener->syncSourceEnd(event);
          break;
      default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a SyncItemEvent
//
bool fireSyncItemEvent(const char* sourceURI,const WCHAR* itemKey, int type) {

    ManageListener& manage = ManageListener::getInstance();
    if(manage.syncitemlistener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncItemEvent event(itemKey, sourceURI, type, timestamp);

    switch(type) {
        case ITEM_ADDED_BY_SERVER:
          manage.syncitemlistener->itemAddedByServer(event);
          break;
        case ITEM_DELETED_BY_SERVER:
          manage.syncitemlistener->itemDeletedByServer(event);
          break;
        case ITEM_UPDATED_BY_SERVER:
          manage.syncitemlistener->itemUpdatedByServer(event);
          break;
        case ITEM_ADDED_BY_CLIENT:
          manage.syncitemlistener->itemAddedByClient(event);
          break;
        case ITEM_DELETED_BY_CLIENT:
          manage.syncitemlistener->itemDeletedByClient(event);
          break;
        case ITEM_UPDATED_BY_CLIENT:
          manage.syncitemlistener->itemUpdatedByClient(event);
          break;
        default:
          return FALSE;
    }

    return TRUE;
}


//
// Fire a SyncStatusEvent
//
bool fireSyncStatusEvent(const char* command, int statusCode, const char* uri, const WCHAR* itemKey, int type) {

    ManageListener& manage = ManageListener::getInstance();
    if(manage.syncstatuslistener == NULL) {
        return FALSE;
    }

    unsigned long timestamp = (unsigned long)time(NULL);
    // Create event (object alive in the scope of this function)
    SyncStatusEvent event(statusCode, command, itemKey, uri, type, timestamp);

    switch(type) {
        case CLIENT_STATUS:
            manage.syncstatuslistener->statusSending(event);
            break;
        case SERVER_STATUS:
            manage.syncstatuslistener->statusReceived(event);
            break;
        default:
            return FALSE;
    }

    return TRUE;
}

