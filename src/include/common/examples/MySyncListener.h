

#ifndef INCL_MY_LISTENER
#define INCL_MY_LISTENER

#include "event/SyncListener.h"

	class MySyncListener : public SyncListener {

		void syncBegin(SyncEvent &event);
		void syncEnd(SyncEvent &event);
		void sendInitialization(SyncEvent &event);
		void sendModifications(SyncEvent &event);
		void sendFinalization(SyncEvent &event);
		void syncError(SyncEvent &event);
	};

#endif
