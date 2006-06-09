
#ifndef INCL_SET_LISTENER
#define INCL_SET_LISTENER

#include "event/BaseEvent.h"
#include "event/SyncEvent.h"
#include "event/SyncSourceEvent.h"
#include "event/SyncItemEvent.h"
#include "event/SyncStatusEvent.h"
#include "event/TransportEvent.h"
#include "event/constants.h"

/* A set of global functions to fire events from inside the Client API
*/

// Fire a SyncEvent
bool fireSyncEvent(const char *msg, int type);

// Fire a TransportEvent
bool fireTransportEvent(unsigned long size, int type);

// Fire a SyncSourceEvent
bool fireSyncSourceEvent(const wchar_t* sourceURI, SyncMode mode, int type);

// Fire a SyncItemEvent
bool fireSyncItemEvent(const wchar_t* sourceURI, const wchar_t* itemKey, int type);

// Fire a SyncStatusEvent
bool fireSyncStatusEvent(const char* command, int statusCode, const wchar_t* uri, const wchar_t* itemKey, int type);

// Delete Event object inside the firing methods
void deleteFireEvent(const BaseEvent* event);

#endif
