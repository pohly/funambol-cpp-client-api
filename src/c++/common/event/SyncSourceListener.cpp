/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


#include "event/SyncSourceListener.h"
#include "event/SyncSourceEvent.h"
#include "event/constants.h"

/*
 * Empty SyncSourceListener methods.
 * Application developers override the methods corresponding to the
 * events they are listening for.
 */

SyncSourceListener::SyncSourceListener() : Listener() {}

void SyncSourceListener::syncSourceBegin(SyncSourceEvent& event) {}
void SyncSourceListener::syncSourceEnd  (SyncSourceEvent& event) {}
void SyncSourceListener::syncSourceSyncModeRequested  (SyncSourceEvent& event) {}
void SyncSourceListener::syncSourceTotalClientItems  (SyncSourceEvent& event) {}
void SyncSourceListener::syncSourceTotalServerItems  (SyncSourceEvent& event) {}
