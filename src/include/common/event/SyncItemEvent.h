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
