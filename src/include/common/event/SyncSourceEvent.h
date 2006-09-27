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


#ifndef INCL_SYNC_SOURCE_EVENT
#define INCL_SYNC_SOURCE_EVENT

#include "event/BaseEvent.h"
#include "event/constants.h"


class SyncSourceEvent : public BaseEvent {

    // source URI
    char* sourceURI;

    // Sync Mode
    int syncMode;

    // source name
    char* name;

    // data
    int data;


public:

    // Constructor
    SyncSourceEvent(const char* uri, const char* sourcename, int mode, int data, int type, unsigned long date);

    // Destructor
    ~SyncSourceEvent();

    // get the source URI
    const char* getSourceURI() const;

    // get the sync mode
    int getSyncMode();

    // get the source name
    const char* getSourceName() const;

    // get the data
    int getData();
};
#endif
