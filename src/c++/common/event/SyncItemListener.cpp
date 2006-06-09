

#include "event/SyncItemListener.h"
#include "event/SyncItemEvent.h"
#include "event/constants.h"

/* 
	Empty SyncItemListener methods
	Application developers override the methods corresponding to the
	events they are listening for
*/

SyncItemListener::SyncItemListener() : Listener() {
}

void SyncItemListener::itemAddedByServer(SyncItemEvent &event) {}
                                                                                
void SyncItemListener::itemAddedByClient(SyncItemEvent &event) {}

void SyncItemListener::itemDeletedByServer(SyncItemEvent &event) {}

void SyncItemListener::itemUpdatedByServer(SyncItemEvent &event) {}

void SyncItemListener::itemDeletedByClient(SyncItemEvent &event) {}

void SyncItemListener::itemUpdatedByClient(SyncItemEvent &event) {}
