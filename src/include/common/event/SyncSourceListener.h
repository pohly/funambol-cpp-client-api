

#ifndef INCL_SYNC_SOURCE_LISTENER
    #define INCL_SYNC_SOURCE_LISTENER

#include "event/SyncSourceEvent.h"
#include "event/Listener.h"

/* 
	Set Listeners for each event in SyncSourceEvent
*/

	class SyncSourceListener : public Listener{
	
	  public:
		//Conctructor
		SyncSourceListener();		
	
		// listen for the Sync Begin Event
		virtual void syncSourceBegin(SyncSourceEvent &event);

		// listen for the Sync End Event
                virtual void syncSourceEnd(SyncSourceEvent &event);
		
	};

#endif

