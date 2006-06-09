

#ifndef INCL_SYNC_SOURCE_EVENT
#define INCL_SYNC_SOURCE_EVENT

#include "event/BaseEvent.h"
#include "event/constants.h"

	class SyncSourceEvent : public BaseEvent {

		// source URI
		wchar_t* sourceURI;
		 
		// Sync Mode
		int syncMode;

	   public:
		
		// Constructor
		SyncSourceEvent(const wchar_t*, int, int type, unsigned long date);

		// Destructor
		~SyncSourceEvent();
		
		// get the source URI
		const wchar_t* getSourceURI() const;

		// get the sync mode
		int getSyncMode();
	};
#endif
