

#ifndef INCL_SYNC_STATUS_LISTENER
    #define INCL_SYNC_STATUS_LISTENER

#include "event/SyncStatusEvent.h"
#include "event/Listener.h"

/* 
	Set Listeners for each event in SyncStatusEvent
*/

	class SyncStatusListener : public Listener{
	
	  public:
		// Constructor
		SyncStatusListener();	
		
		// listen for the Status Received Event
		virtual void statusReceived(SyncStatusEvent &event);

		// listen for the Status Sending Event
                virtual void statusSending(SyncStatusEvent &event);
		
	};

#endif

