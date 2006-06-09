

#ifndef INCL_MY_TRANSPORT_LISTENER
#define INCL_MY_TRANSPORT_LISTENER

#include "event/TransportListener.h"

	class MyTransportListener : public TransportListener {

		void sendDataBegin(TransportEvent &event);
		//void syncDataEnd(TransportEvent &event);
		void receiveDataBegin(TransportEvent &event);
		void receivingData(TransportEvent &event);
		void receiveDataEnd(TransportEvent &event);
	};

#endif
