
#ifndef INCL_SET_LISTENER
#define INCL_SET_LISTENER

#include "event/SyncListener.h"
#include "event/SyncSourceListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/TransportListener.h"

/* SetListener class contains a set of methods to either set or unset Listeners
	for various events
*/

	class SetListener {

	    public:

		// Set the SyncEvent Listener
		bool setSyncListener(SyncListener *listener);

		// Set the TransportEvent Listener
                bool setTransportListener(TransportListener *listener);

		// Set the SyncSourceEvent Listener
                bool setSyncSourceListener(SyncSourceListener *listener);

		// Set the SyncItemEvent Listener
                bool setSyncItemListener(SyncItemListener *listener);

		// Set the SyncStatusEvent Listener
                bool setSyncStatusListener(SyncStatusListener *listener);

		//removes the SyncEvent Listener
                bool unsetSyncListener();

		// Removes the TransportEvent Listener
                bool unsetTransportListener();

		// Removes the SyncSourceEvent Listener
                bool unsetSyncSourceListener();

		// Removes the SyncItemEvent Listener
                bool unsetSyncItemListener();

		// Removes the SyncStatusEvent Listener
                bool unsetSyncStatusListener();	
	};

#endif
