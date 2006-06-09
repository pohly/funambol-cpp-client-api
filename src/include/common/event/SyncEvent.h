
#ifndef INCL_SYNC_EVENT
	#define INCL_SYNC_EVENT

#include "event/BaseEvent.h"
#include "event/constants.h"
#include "base/util/utils.h"

	class SyncEvent : public BaseEvent {

		// error that occured during this event
		char * message;
		
	    public:

		// Constructor
		SyncEvent(int type, unsigned long date);
		
		// Destructor
		~SyncEvent();

		// set error message
		bool setMessage(const char * message); 

		// get the error messages
		const char* getMessage() const;

	};

#endif
