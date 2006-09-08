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


#include "examples/MySyncItemListener.h"

void MySyncItemListener::itemAddedByServer(SyncItemEvent &event) {

    printf("SyncItemEvent occurred.\n");
    printf("Sync Item %s from sourceURI %s successfully added from server at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemDeletedByServer(SyncItemEvent &event) {

    printf("SyncItemEvent occurred.\n");
    printf("Sync Item %s from sourceURI %s successfully deleted by server at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemUpdatedByServer(SyncItemEvent &event) {

    printf("SyncItemEvent occurred.\n");
    printf("Sync Item %s from sourceURI %s successfully updated from server at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemAddedByClient(SyncItemEvent &event) {

    printf("SyncItemEvent occurred.\n");
    printf("Sync Item %s from sourceURI %s successfully detected by client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemUpdatedByClient(SyncItemEvent &event) {

    printf("SyncItemEvent occurred.\n");
    printf("Sync Item %s from sourceURI %s updated by client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}

void MySyncItemListener::itemDeletedByClient(SyncItemEvent &event) {

    printf("SyncItemEvent occurred.\n");
    printf("Sync Item %s from sourceURI %s deleted by Client at %ld.\n\n",event.getItemKey(), event.getSourceURI(), event.getDate());
}


