

#ifndef INCL_TRANSPORT_LISTENER
    #define INCL_TRANSPORT_LISTENER

#include "event/TransportEvent.h"
#include "event/Listener.h"


/* 
	Set Listeners for each event in TransportEvent
*/

	class TransportListener : public Listener{
	
	  public:
	
		// Constructor
		TransportListener();
		
		// listen for the Send Data Begin Event
		virtual void sendDataBegin(TransportEvent &event);

		// listen for the Sync Data End Event
                virtual void syncDataEnd(TransportEvent &event);

		// listen for the Receive Data Begin Event
                virtual void receiveDataBegin(TransportEvent &event);

		// listen for the Receiving Data Event
                virtual void receivingData(TransportEvent &event);

		// listen for the Receive Data End Event
                virtual void receiveDataEnd(TransportEvent &event);
	};

#endif

