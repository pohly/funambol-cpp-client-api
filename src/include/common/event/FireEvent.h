/*
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


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
