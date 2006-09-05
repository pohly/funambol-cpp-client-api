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


#include "event/SyncStatusEvent.h"
#include "base/util/utils.h"

SyncStatusEvent::SyncStatusEvent(int code, const char* cmd, const wchar_t* key, const char* uri, int type, unsigned long date) : SyncItemEvent( key, uri, type, date) {
    statusCode = code;
    command = stringdup(cmd);
}

SyncStatusEvent::~SyncStatusEvent() {

    if(command) {
        delete [] command;
        command = NULL;
    }
}

int SyncStatusEvent::getStatusCode() {
    return statusCode;
}

const char* SyncStatusEvent::getCommand() const{
    return command;
}
