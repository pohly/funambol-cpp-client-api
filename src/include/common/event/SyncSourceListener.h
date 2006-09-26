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


#ifndef INCL_SYNC_SOURCE_LISTENER
#define INCL_SYNC_SOURCE_LISTENER

#include "event/SyncSourceEvent.h"
#include "event/Listener.h"

/*
 * Set Listeners for each event in SyncSourceEvent
 */

class SyncSourceListener : public Listener{

public:
    //Conctructor
    SyncSourceListener();

    // listen for the Sync Begin Event
    virtual void syncSourceBegin(SyncSourceEvent& event);

    // listen for the Sync End Event
    virtual void syncSourceEnd(SyncSourceEvent& event);
    
    // listen for the SyncMode requested by the server
    virtual void SyncSourceListener::syncSourceSyncModeRequested  (SyncSourceEvent& event);
};

#endif

