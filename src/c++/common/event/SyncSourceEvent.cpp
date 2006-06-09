

#include "event/SyncSourceEvent.h"
#include "base/util/utils.h"

SyncSourceEvent::SyncSourceEvent(const wchar_t* uri, int mode, int type, unsigned long date) : BaseEvent(type, date) {

    	sourceURI = wstrdup(uri);

	syncMode = mode;
}

SyncSourceEvent::~SyncSourceEvent() {
	
	if(sourceURI) {
		delete [] sourceURI;
	}
}

const wchar_t* SyncSourceEvent::getSourceURI() const{

	return sourceURI;
}

int SyncSourceEvent::getSyncMode() {
	
	return syncMode;
}
