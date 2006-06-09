

#ifndef INCL_MY_SOURCE_LISTENER
#define INCL_MY_SOURCE_LISTENER

#include "event/SyncSourceListener.h"

	class MySyncSourceListener : public SyncSourceListener {

		void syncSourceBegin(SyncSourceEvent &event);
		void syncSourceEnd(SyncSourceEvent &event);
	};

#endif
