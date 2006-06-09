
#include "event/SyncItemEvent.h"
#include "base/util/utils.h"

SyncItemEvent::SyncItemEvent(const wchar_t* key, const wchar_t* uri, int type, unsigned long date) : BaseEvent(type, date) {

    itemKey = wstrdup(key);
   	sourceURI = wstrdup(uri);
}

SyncItemEvent::~SyncItemEvent() {

	if(itemKey) {
		delete [] itemKey;
	}

	if(sourceURI) {
		delete [] sourceURI;
	}
}

const wchar_t* SyncItemEvent::getSourceURI() const{
	
	return sourceURI;
}

const wchar_t* SyncItemEvent::getItemKey() const{
	return itemKey;
}
