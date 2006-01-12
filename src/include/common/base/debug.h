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



#ifndef INCL_BASE_DEBUG
#define INCL_BASE_DEBUG

#include "base/fscapi.h"

/**
 * This constants are for debug messages that won't be removed from the code
 * but will be used when the client is in debug mode.
 */
#define DBG_LOADING_CONN_SETTINGS TEXT("Loading connection settings from node %s")
#define DBG_LOADING_MANAGEMENT_OBJECT TEXT("Loading management object %s")
#define DBG_INVALID_ACCESS_SETTINGS TEXT("Access settings invalid record format!")
#define DBG_NODE_NOT_FOUND TEXT("Node not found")
#define DBG_LOADING_SOURCES TEXT("Loading sources subnodes from node %s")
#define DBG_INVALID_SOURCE_SETTINGS TEXT("Source settings invalid record format!")
#define DBG_SOURCE_CONFIGS_NOT_FOUND TEXT("No source configs were found, using default")
#define DBG_COMMITTING_SOURCE TEXT("Committing source %s (next: %s)")
#define DBG_COMMITTING_CHANGE TEXT("Committing %s operation on item %s")
#define DBG_READING_CONFIG_FROM_DM TEXT("Reading configuration settings from the management tree")
#define DBG_WRITING_CONFIG_TO_DM TEXT("Writing configuration settings to the management tree")
#define DBG_DELETING_ITEM TEXT("Deleting item with LUID: %ld - Status: %u")
#define DBG_ADDING_ITEM TEXT("Adding item with GUID: %s - LUID: %lu - Status: %u")
#define DBG_UPDATING_ITEM TEXT("Updating item with LUID: %lu - Status: %u")
#define DBG_ITEM_STATUS TEXT("Item: %s, Cmd: %s, Status code: %u")
#define DBG_NUMBER_OF_RECORD TEXT("The sync source %s contains %d items")
#define DBG_REMOVING_RECORD TEXT("Removing record in position %d (%ld)")
#define DBG_RESETTING_RECORD TEXT("Resetting record in position %d (%ld)")
#define DBG_RECORD_ATTTRS TEXT("Record info of %s")
#define DBG_DELETION_DETECTED TEXT("Detected deleted record: %ld")
#define DBG_CHANGE_DETECTED TEXT("Detected modified record: %ld")
#define DBG_RECORD_NOT_COMMITTED TEXT("Record in position %d (%ld) not committed due to error %d on %s")
#define DBG_READING TEXT("Reading ...")
#define DBG_READING_CONTENT TEXT("Reading content (buffer size: %ld)")
#define DBG_READING_BUFFER TEXT("Read %ld bytes")
#define DBG_READING_DONE TEXT("Reading done")
#define DBG_RECEIVED_CONTENT TEXT("Response:")
#define DBG_HTTP_HEADERS TEXT("Request HTTP headers:")
#endif
