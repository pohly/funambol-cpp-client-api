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


#ifndef INCL_SYNC_STATUS_EVENT
#define INCL_SYNC_STATUS_EVENT

#include "event/SyncItemEvent.h"
#include "event/constants.h"


class SyncStatusEvent : public SyncItemEvent {

    // status code
    int statusCode;

    //command the status relates to
    char* command;

public:

    // Constructor
    SyncStatusEvent(int code, const char* cmd, const WCHAR* key, const char* uri, int type, unsigned long date);

    // Destructor
    ~SyncStatusEvent();

    // get the current status code
    int getStatusCode();

    // get the command the status related to
    const char* getCommand() const;
};
#endif
