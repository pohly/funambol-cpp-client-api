

#ifndef INCL_MANAGE_LISTENER
#define INCL_MANAGE_LISTENER

#include "event/SyncListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/SyncSourceListener.h"
#include "event/TransportListener.h"

   /* This is the ManageListener class - which keeps track of the various registered 
    * listeners. This is implemented as an singleton pattern to make sure only instance is 
    * active at any point of time.
    */

    class ManageListener {
	
	public:
		//get and release singleton instance
		static ManageListener & getInstance();
		static void dispose();		

	        //Registered Listeners : At present only one Listener per event family
		SyncListener   * synclistener;
	 	TransportListener * transportlistener;	
		SyncStatusListener * syncstatuslistener;
		SyncItemListener * syncitemlistener;
		SyncSourceListener * syncsourcelistener;

	private:
		static ManageListener *instance;

		//private constructor & destructor
		ManageListener();
		~ManageListener();

    }; 

#endif
