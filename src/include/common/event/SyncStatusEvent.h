

#ifndef INCL_SYNC_STATUS_EVENT
#define INCL_SYNC_STATUS_EVENT

#include "event/SyncItemEvent.h"
#include "event/constants.h"

	class SyncStatusEvent : public SyncItemEvent {

		// status code
		int statusCode;

		//command the status relates to
		char * command;

	   public:

		// Constructor
		SyncStatusEvent(int code, const char * cmd, const wchar_t* key, const wchar_t* uri, int type, unsigned long date);
		
		// Destructor
		~SyncStatusEvent();

		// get the current status code
		int getStatusCode(); 

		// get the command the status related to
		const char * getCommand() const;
	};
#endif
