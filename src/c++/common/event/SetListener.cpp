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


#include "event/SetListener.h"
#include "event/ManageListener.h"

//
// Set listeners:
//
void setSyncListener(SyncListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.synclistener = listener;
}

void setTransportListener(TransportListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.transportlistener = listener;
}

void setSyncSourceListener(SyncSourceListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.syncsourcelistener = listener;
}

void setSyncItemListener(SyncItemListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.syncitemlistener = listener;
}

void setSyncStatusListener(SyncStatusListener* listener) {
    ManageListener& manage = ManageListener::getInstance();
    manage.syncstatuslistener = listener;
}

//
// Unset listeners:
//
void unsetSyncListener() {
    ManageListener& manage = ManageListener::getInstance();
    if(manage.synclistener) {
        delete manage.synclistener;
        manage.synclistener = NULL;
    }
}

void unsetTransportListener() {
    ManageListener& manage = ManageListener::getInstance();
    if(manage.transportlistener) {
        delete manage.transportlistener;
        manage.transportlistener = NULL;
    }
}

void unsetSyncSourceListener() {
    ManageListener& manage = ManageListener::getInstance();
    if(manage.syncsourcelistener) {
        delete manage.syncsourcelistener;
        manage.syncsourcelistener = NULL;
    }
}

void unsetSyncItemListener() {
    ManageListener& manage = ManageListener::getInstance();
    if(manage.syncitemlistener) {
        delete manage.syncitemlistener;
        manage.syncitemlistener = NULL;
    }
}

void unsetSyncStatusListener() {
    ManageListener& manage = ManageListener::getInstance();
    if(manage.syncstatuslistener) {
        delete manage.syncstatuslistener;
        manage.syncstatuslistener = NULL;
    }
}

