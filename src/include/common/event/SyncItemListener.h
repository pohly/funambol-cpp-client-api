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


#ifndef INCL_SYNC_ITEM_LISTENER
#define INCL_SYNC_ITEM_LISTENER

#include "event/SyncItemEvent.h"
#include "event/Listener.h"


/*
 * Set Listeners for each event in SyncItemEvent
 */

class SyncItemListener : public Listener {

public:
    //Constructor
    SyncItemListener();

    // listen for the Item added by Server Event
    virtual void itemAddedByServer(SyncItemEvent& event);

    // listen for the Item  deleted by Server Event
    virtual void itemDeletedByServer(SyncItemEvent& event);

    // listen for the Item updated by Server Event
    virtual void itemUpdatedByServer(SyncItemEvent& event);

    // listen for the Item added by Client Event
    virtual void itemAddedByClient(SyncItemEvent& event);

    // listen for the Item deleted by Client Event
    virtual void itemDeletedByClient(SyncItemEvent& event);

    // listen for the Item updated by Server Event
    virtual void itemUpdatedByClient(SyncItemEvent& event);
};

#endif

