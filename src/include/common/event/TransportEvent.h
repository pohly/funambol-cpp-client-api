
#ifndef INCL_TRANSPORT_EVENT
#define INCL_TRANSPORT_EVENT

#include "event/BaseEvent.h"

	class TransportEvent : public BaseEvent {

		// amount of data received
		unsigned long dataSize;

	   public:

		// Constructor
		TransportEvent(unsigned long size, int type, unsigned long date);

		// get the amount of data received
		unsigned long getDataSize();
	};


#endif
