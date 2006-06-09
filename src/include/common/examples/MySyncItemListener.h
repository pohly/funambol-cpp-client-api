

#ifndef INCL_MY_ITEM_LISTENER
#define INCL_MY_ITEM_LISTENER

#include "event/SyncItemListener.h"

	class MySyncItemListener : public SyncItemListener {

		void itemAddedByServer(SyncItemEvent &event);
		void itemDeletedByServer(SyncItemEvent &event);
		void itemUpdatedByServer(SyncItemEvent &event);
		void itemAddedByClient(SyncItemEvent &event);
		void itemDeletedByClient(SyncItemEvent &event);
		void itemUpdatedByClient(SyncItemEvent &event);
	};

#endif
