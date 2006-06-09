

#include "event/SyncStatusEvent.h"
#include "base/util/utils.h"

SyncStatusEvent::SyncStatusEvent(int code, const char * cmd, const wchar_t* key, const wchar_t* uri, int type, unsigned long date) : SyncItemEvent( key, uri, type, date) {
	statusCode = code;
    command = stringdup(cmd);
}

SyncStatusEvent::~SyncStatusEvent() {
	
	if(command) {
		delete [] command;
	}
}

int SyncStatusEvent::getStatusCode() {
	
	return statusCode;
}

const char * SyncStatusEvent::getCommand() const{
	
	return command;
}
