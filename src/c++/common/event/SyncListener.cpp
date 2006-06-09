

#include "event/SyncListener.h"
#include "event/SyncEvent.h"
#include "event/constants.h"

/* 
	Empty SyncListener methods
	Application developers override the methods corresponding to the
	events they are listening for
*/

SyncListener::SyncListener() : Listener() {
}

void SyncListener::syncBegin(SyncEvent &event) {}
                                                                                
void SyncListener::syncEnd(SyncEvent &event) { }

void SyncListener::syncError(SyncEvent &event) { }                                                                                 
void SyncListener::sendInitialization(SyncEvent &event) { }

void SyncListener::sendModifications(SyncEvent &event) { }                                                                               
void SyncListener::sendFinalization(SyncEvent &event) { }
