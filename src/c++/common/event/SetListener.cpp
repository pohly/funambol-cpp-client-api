

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

