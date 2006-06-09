

#include "event/TransportListener.h"
#include "event/TransportEvent.h"
#include "event/constants.h"

/* 
	Empty TransportListener methods
	Application developers override the methods corresponding to the
	events they are listening for
*/

TransportListener::TransportListener() : Listener() {
}

void TransportListener::sendDataBegin(TransportEvent &event) {}
                                                                                
void TransportListener::syncDataEnd(TransportEvent &event) {}

void TransportListener::receiveDataBegin(TransportEvent &event) {}

void TransportListener::receivingData(TransportEvent &event) {}

void TransportListener::receiveDataEnd(TransportEvent &event) {}

