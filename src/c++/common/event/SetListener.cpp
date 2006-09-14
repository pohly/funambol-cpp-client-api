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

bool SetListener::setSyncListener(SyncListener * listener) {
		
	ManageListener & manage = ManageListener::getInstance();
	
	manage.synclistener = listener;
	
	return TRUE;
}

bool SetListener::setTransportListener(TransportListener * listener) {
                                                                                
	ManageListener & manage = ManageListener::getInstance();

        manage.transportlistener = listener;

	return TRUE;
}
bool SetListener::setSyncSourceListener(SyncSourceListener * listener) {
                                                                                
	ManageListener & manage = ManageListener::getInstance();
	
        manage.syncsourcelistener = listener;

	return TRUE;
}
bool SetListener::setSyncItemListener(SyncItemListener * listener) {
                                                                                
	ManageListener & manage = ManageListener::getInstance();	
	
        manage.syncitemlistener = listener;

	return TRUE;
}
bool SetListener::setSyncStatusListener(SyncStatusListener * listener) {
                                                                                
	ManageListener & manage = ManageListener::getInstance();

        manage.syncstatuslistener = listener;
	
	return TRUE;
}

bool SetListener::unsetSyncListener() {

	ManageListener & manage = ManageListener::getInstance();	
	
	if(manage.synclistener) {
		delete manage.synclistener;
	}
	
	return TRUE;	
}

bool SetListener::unsetTransportListener() {
                                                                                
        ManageListener & manage = ManageListener::getInstance();
                                                                                
        if(manage.transportlistener) {
                delete manage.transportlistener;
        }
                                                                                
        return TRUE;
}
bool SetListener::unsetSyncSourceListener() {
                                                                                
        ManageListener & manage = ManageListener::getInstance();
                                                                                
        if(manage.syncsourcelistener) {
                delete manage.syncsourcelistener;
        }
                                                                                
        return TRUE;
}
bool SetListener::unsetSyncItemListener() {
                                                                                
        ManageListener & manage = ManageListener::getInstance();
                                                                                
        if(manage.syncitemlistener) {
                delete manage.syncitemlistener;
        }
                                                                                
        return TRUE;
}

bool SetListener::unsetSyncStatusListener() {
                                                                                                 
        ManageListener & manage = ManageListener::getInstance();
                                                                                                 
        if(manage.syncstatuslistener) {
                delete manage.syncstatuslistener;
        }
                                                                                                 
        return TRUE;
}

