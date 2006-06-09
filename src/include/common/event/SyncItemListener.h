

#ifndef INCL_SYNC_ITEM_LISTENER
    #define INCL_SYNC_ITEM_LISTENER

#include "event/SyncItemEvent.h"
#include "event/Listener.h"


/* 
	Set Listeners for each event in SyncItemEvent
*/

	class SyncItemListener : public Listener{
	
	  public:
		//Constructor
		SyncItemListener();
			
		// listen for the Item added by Server Event
		virtual void itemAddedByServer(SyncItemEvent &event);

		// listen for the Item  deleted by Server Event
                virtual void itemDeletedByServer(SyncItemEvent &event);

		// listen for the Item updated by Server Event
                virtual void itemUpdatedByServer(SyncItemEvent &event);

		// listen for the Item added by Client Event
                virtual void itemAddedByClient(SyncItemEvent &event);

		// listen for the Item deleted by Client Event
                virtual void itemDeletedByClient(SyncItemEvent &event);

		// listen for the Item updated by Server Event
                virtual void itemUpdatedByClient(SyncItemEvent &event);		
	};

#endif

