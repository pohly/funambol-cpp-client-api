

#include "event/SyncEvent.h"
#include "base/util/utils.h"

SyncEvent::SyncEvent(int type, unsigned long date) : BaseEvent(type, date) {

	message = NULL;
}

SyncEvent::~SyncEvent() {
	
	if(message) {
		delete [] message;
	}
}

bool SyncEvent::setMessage(const char * msg) {

	if (message)
        	delete [] message;
    	message = stringdup(msg);
    
	return TRUE;
}

const char * SyncEvent::getMessage() const{

	return message;
}
