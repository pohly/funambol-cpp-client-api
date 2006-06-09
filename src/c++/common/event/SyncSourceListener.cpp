

#include "event/SyncSourceListener.h"
#include "event/SyncSourceEvent.h"
#include "event/constants.h"

/* 
	Empty SyncSourceListener methods
	Application developers override the methods corresponding to the
	events they are listening for
*/

SyncSourceListener::SyncSourceListener() : Listener() {
}

void SyncSourceListener::syncSourceBegin(SyncSourceEvent &event) {}
                                                                                
void SyncSourceListener::syncSourceEnd(SyncSourceEvent &event) {}

