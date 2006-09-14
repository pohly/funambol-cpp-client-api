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



#include "examples/MySyncListener.h"

void MySyncListener::syncBegin(SyncEvent &event) {

	printf("SyncEvent occurred.\n");
	printf("Syncing successfully began at %ld.\n\n", event.getDate());
}

void MySyncListener::syncEnd(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Syncing successfully ended at %ld.\n\n", event.getDate());
}

void MySyncListener::sendInitialization( SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Initializations done at %ld.\n\n", event.getDate());
}

void MySyncListener::sendModifications(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Modifications processed at %ld.\n\n", event.getDate());
}

void MySyncListener::sendFinalization(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("final package set & processed at %ld.\n\n", event.getDate());
}

void MySyncListener::syncError(SyncEvent &event) {
                                                                                
        printf("SyncEvent occurred.\n");
        printf("Sync Error %s occured began at %ld.\n\n", event.getMessage(), event.getDate());
}


