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


#include "event/FireEvent.h"
#include "event/ManageListener.h"
#include "event/constants.h"

#include "base/fscapi.h"

bool fireSyncEvent(const char * msg, int type) {

    ManageListener & manage = ManageListener::getInstance();

    unsigned long timestamp = (unsigned long)time(NULL);
    SyncEvent* event = new SyncEvent(type, timestamp);

    if(event) {
        event->setMessage(msg);
    }
    else {
        return FALSE;
    }

    if(manage.synclistener) {

        switch(type) {
          case SYNC_BEGIN:
              manage.synclistener->syncBegin(*event);
              break;
          case SYNC_END:
              manage.synclistener->syncEnd(*event);
              break;
          case SEND_INITIALIZATION:
              manage.synclistener->sendInitialization(*event);
              break;
          case SEND_MODIFICATION:
              manage.synclistener->sendModifications(*event);
              break;
          case SEND_FINALIZATION:
              manage.synclistener->sendFinalization(*event);
              break;
          case SYNC_ERROR:
              manage.synclistener->syncError(*event);
              break;
          default:
              return FALSE;
        }
    }
    else {
        return FALSE;
    }

    return TRUE;

    deleteFireEvent(event);
}

//Fire a Transport Event
bool fireTransportEvent(unsigned long size, int type) {

    ManageListener & manage = ManageListener::getInstance();

    unsigned long timestamp = (unsigned long)time(NULL);
    TransportEvent * event = new TransportEvent(size, type, timestamp);

    if(manage.transportlistener) {

        switch(type) {
            case SEND_DATA_BEGIN:
              manage.transportlistener->sendDataBegin(*event);
              break;
            case SEND_DATA_END:
              manage.transportlistener->syncDataEnd(*event);
              break;
            case RECEIVE_DATA_BEGIN:
              manage.transportlistener->receiveDataBegin(*event);
              break;
            case RECEIVE_DATA_END:
              manage.transportlistener->receiveDataEnd(*event);
              break;
            case DATA_RECEIVED:
              manage.transportlistener->receivingData(*event);
              break;
            default:
              return FALSE;
        }
    }
    else {
        return FALSE;
    }

    return TRUE;

    deleteFireEvent(event);
}

// Fire a SyncSourceEvent
bool fireSyncSourceEvent(const wchar_t* sourceURI, SyncMode mode, int type)
{
    ManageListener & manage = ManageListener::getInstance();

    unsigned long timestamp = (unsigned long)time(NULL);
    SyncSourceEvent * event = new SyncSourceEvent(sourceURI, mode,type, timestamp);

    if(manage.syncsourcelistener) {

        switch(type) {

          case SYNC_SOURCE_BEGIN:
              manage.syncsourcelistener->syncSourceBegin(*event);
              break;
          case SYNC_SOURCE_END:
              manage.syncsourcelistener->syncSourceEnd(*event);
              break;
          default:
              return FALSE;
        }
    }
    else {
        return FALSE;
    }

    return TRUE;

    deleteFireEvent(event);
}

// Fire a SyncItemEvent
bool fireSyncItemEvent(const wchar_t* sourceURI,const wchar_t* itemKey, int type)
{
    ManageListener & manage = ManageListener::getInstance();

    unsigned long timestamp = (unsigned long)time(NULL);
    SyncItemEvent * event = new SyncItemEvent(itemKey, sourceURI, type, timestamp);

    if(manage.syncitemlistener) {

        switch(type) {
            case ITEM_ADDED_BY_SERVER:
              manage.syncitemlistener->itemAddedByServer(*event);
              break;
            case ITEM_DELETED_BY_SERVER:
              manage.syncitemlistener->itemDeletedByServer(*event);
              break;
            case ITEM_UPDATED_BY_SERVER:
              manage.syncitemlistener->itemUpdatedByServer(*event);
              break;
            case ITEM_ADDED_BY_CLIENT:
              manage.syncitemlistener->itemAddedByClient(*event);
              break;
            case ITEM_DELETED_BY_CLIENT:
              manage.syncitemlistener->itemDeletedByClient(*event);
              break;
            case ITEM_UPDATED_BY_CLIENT:
              manage.syncitemlistener->itemUpdatedByClient(*event);
              break;
            default:
              return FALSE;
        }
    }
    else {
        return FALSE;
    }

    return TRUE;

    deleteFireEvent(event);
}

// Fire a SyncStatusEvent
bool fireSyncStatusEvent(const char* command, int statusCode, const wchar_t* uri, const wchar_t* itemKey, int type)
{
    ManageListener & manage = ManageListener::getInstance();

    unsigned long timestamp = (unsigned long)time(NULL);
    SyncStatusEvent * event = new SyncStatusEvent(statusCode, command, itemKey, uri, type, timestamp);

    if(manage.syncstatuslistener) {
        switch(type) {
            case CLIENT_STATUS:
                manage.syncstatuslistener->statusSending(*event);
                break;
            case SERVER_STATUS:
                manage.syncstatuslistener->statusReceived(*event);
                break;
            default:
                return FALSE;
        }
    }
    else {
        return FALSE;
    }

    return TRUE;

    deleteFireEvent(event);
}

void deleteFireEvent(const BaseEvent* event) {

    if(event)
        delete event;
}
