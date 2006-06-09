

#ifndef INCL_SYNC_LISTENER
    #define INCL_SYNC_LISTENER

#include "event/SyncEvent.h"
#include "event/Listener.h"


/* 
	Set Listeners for each event in SyncEvent
*/

	class SyncListener : public Listener{

	  public:
		
		//Contructor
		SyncListener();

		// listen for the Sync Begin Event
		virtual void syncBegin(SyncEvent &event);

		// listen for the Sync End Event
                virtual void syncEnd(SyncEvent &event);

		// listen for the Send Initialization Event
                virtual void sendInitialization(SyncEvent &event);

		// listen for the Send Modifications Event
                virtual void sendModifications(SyncEvent &event);

		// listen for the Sync Finalization Event
                virtual void sendFinalization(SyncEvent &event);

		// listen for the Sync Error Event
                virtual void syncError(SyncEvent &event);
	
	};

#endif

