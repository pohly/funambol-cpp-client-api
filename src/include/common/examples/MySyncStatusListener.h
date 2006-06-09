

#ifndef INCL_MY_STATUS_LISTENER
#define INCL_MY_STATUS_LISTENER

#include "event/SyncStatusListener.h"

	class MySyncStatusListener : public SyncStatusListener {

		//void statusReceived(SyncStatusEvent &event);
		void statusSending(SyncStatusEvent &event);
	};

#endif
