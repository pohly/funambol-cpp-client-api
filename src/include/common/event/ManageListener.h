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



#ifndef INCL_MANAGE_LISTENER
#define INCL_MANAGE_LISTENER

#include "event/SyncListener.h"
#include "event/SyncItemListener.h"
#include "event/SyncStatusListener.h"
#include "event/SyncSourceListener.h"
#include "event/TransportListener.h"

   /* This is the ManageListener class - which keeps track of the various registered 
    * listeners. This is implemented as an singleton pattern to make sure only instance is 
    * active at any point of time.
    */

    class ManageListener {
	
	public:
		//get and release singleton instance
		static ManageListener & getInstance();
		static void dispose();		

	        //Registered Listeners : At present only one Listener per event family
		SyncListener   * synclistener;
	 	TransportListener * transportlistener;	
		SyncStatusListener * syncstatuslistener;
		SyncItemListener * syncitemlistener;
		SyncSourceListener * syncsourcelistener;

	private:
		static ManageListener *instance;

		//private constructor & destructor
		ManageListener();
		~ManageListener();

    }; 

#endif
