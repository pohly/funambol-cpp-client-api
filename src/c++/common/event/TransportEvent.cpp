

#include "event/TransportEvent.h"


TransportEvent::TransportEvent(unsigned long size, int type, unsigned long date) : BaseEvent(type, date) {

	dataSize = size;
}

unsigned long TransportEvent::getDataSize() {
	
	return dataSize;
}
