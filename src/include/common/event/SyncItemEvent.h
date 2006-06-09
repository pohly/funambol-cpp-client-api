

#ifndef INCL_SYNC_ITEM_EVENT
#define INCL_SYNC_ITEM_EVENT

#include "event/BaseEvent.h"
#include "spds/SyncItem.h"
#include "spds/constants.h"

	class SyncItemEvent : public BaseEvent {

		// Itemkey of the object getting synced 
		wchar_t* itemKey;

		// Source URI
		wchar_t* sourceURI;

	    public:

		// Constructor
		SyncItemEvent(const wchar_t* key, const wchar_t* uri, int type, unsigned long date);
		
		// Destructor
		~SyncItemEvent();

		// get the source uri
		const wchar_t* getSourceURI() const;
		
		// get the sync item key
		const wchar_t* getItemKey() const;
		
	};

#endif
