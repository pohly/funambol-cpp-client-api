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


#include "event/ManageListener.h"

/* Static Variables */

ManageListener * ManageListener::instance = 0;

// Private Methods
//Contructor and Destructor

ManageListener::ManageListener() {
	
	synclistener = NULL;
	transportlistener = NULL;
	syncitemlistener = NULL;
	syncstatuslistener = NULL;
	syncsourcelistener = NULL;
}

ManageListener::~ManageListener() {
		
	if(synclistener) {
		delete synclistener;
	}
	
	if(transportlistener) {
		delete transportlistener;
	}

	if(syncitemlistener) {

		delete syncitemlistener;
	}

	if(syncsourcelistener) {
		delete syncsourcelistener;
	}

	if(syncstatuslistener) {
		delete syncstatuslistener;
	}
}


//Public Methods

/*
 * Get, or create, ManageListener instance
 */
ManageListener& ManageListener::getInstance() {

	if(instance == NULL) {
		instance = new ManageListener();
	}

	return *instance;
}

void ManageListener::dispose() {

	if(instance) {
		delete instance;
	}

	instance = NULL;
}
