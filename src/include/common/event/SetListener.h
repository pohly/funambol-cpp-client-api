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

#include "event/SyncListener.h"
#include "event/SyncSourceListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/TransportListener.h"

/* SetListener class contains a set of methods to either set or unset Listeners
	for various events
*/

	class SetListener {

	    public:

		// Set the SyncEvent Listener
		bool setSyncListener(SyncListener *listener);

		// Set the TransportEvent Listener
                bool setTransportListener(TransportListener *listener);

		// Set the SyncSourceEvent Listener
                bool setSyncSourceListener(SyncSourceListener *listener);

		// Set the SyncItemEvent Listener
                bool setSyncItemListener(SyncItemListener *listener);

		// Set the SyncStatusEvent Listener
                bool setSyncStatusListener(SyncStatusListener *listener);

		//removes the SyncEvent Listener
                bool unsetSyncListener();

		// Removes the TransportEvent Listener
                bool unsetTransportListener();

		// Removes the SyncSourceEvent Listener
                bool unsetSyncSourceListener();

		// Removes the SyncItemEvent Listener
                bool unsetSyncItemListener();

		// Removes the SyncStatusEvent Listener
                bool unsetSyncStatusListener();	
	};

#endif
