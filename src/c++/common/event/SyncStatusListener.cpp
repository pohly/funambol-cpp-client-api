

#include "event/SyncStatusListener.h"
#include "event/SyncStatusEvent.h"
#include "event/constants.h"

/* 
	Empty SyncStatusListener methods
	Application developers override the methods corresponding to the
	events they are listening for
*/

SyncStatusListener::SyncStatusListener() : Listener() {
}

void SyncStatusListener::statusReceived(SyncStatusEvent &event) {}
                                                                                
void SyncStatusListener::statusSending(SyncStatusEvent &event) {}

