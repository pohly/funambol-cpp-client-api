/*
 * Copyright (C) 2003-2007 Funambol
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


#ifndef INCL_SYNC_ITEM_EVENT
#define INCL_SYNC_ITEM_EVENT

#include "event/BaseEvent.h"
#include "spds/SyncItem.h"
#include "spds/constants.h"


class SyncItemEvent : public BaseEvent {

    // Itemkey of the object getting synced
    WCHAR* itemKey;

    // Source URI
    char* sourceURI;

    // Source name
    char* sourcename;

public:

    // Constructor
    SyncItemEvent(const WCHAR* key, const char* name, const char* uri, int type, unsigned long date);

    // Destructor
    ~SyncItemEvent();

    // get the source uri
    const char* getSourceURI() const;

    // get the source uri
    const char* getSourceName() const;

    // get the sync item key
    const WCHAR* getItemKey() const;

};

#endif
