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

#ifndef INCL_BASE_MESSAGES
#define INCL_BASE_MESSAGES

#include "base/fscapi.h"

//
// NOTE: these messages are client specific and targeted to the users (for
// example to being displayed to the user). They may replace corresponding
// ERRMSG_*** messages which are instead log-oriented
//

#define MSG_CONFIG_SUCCESSFULLY_READ    TEXT("Configuration successfully read")
#define MSG_INITIALIZATATION_MESSAGE    TEXT("Initialization message:")
#define MSG_UNKNOWN                     TEXT("Unknown")
#define MSG_PREPARING_SYNC              TEXT("Preparing synchronization of %s...")
#define MSG_SYNC_URL                    TEXT("Synchronization URL: %s")
#define MSG_NOT_AUTHORIZED              TEXT("Sorry, you are not authorized to synchronize. Check the username/password settings.")
#define MSG_DB_NOT_FOUND                TEXT("Remote database %s not found. Check the remote settings.")
#define MSG_INVALID_URL                 TEXT("Invalid synchronization URL. Check the connection settings.")
#define MSG_MODIFICATION_MESSAGE        TEXT("Modification message:")
#define MSG_OUT_OF_MEMORY               TEXT("Out of memory!")
#define MSG_APPLYING_MODIFICATIONS      TEXT("Applying server modifications: %ld new item(s), %ld updated item(s), %ld deleted item(s)")
#define MSG_ADDING_ITEMS                TEXT("Adding %ld item(s)")
#define MSG_UPDATING_ITEMS              TEXT("Updating %ld item(s)")
#define MSG_DELETING_ITEMS              TEXT("Deleting %ld item(s)")
#define MSG_SYNCHRONIZING               TEXT("Synchronizing...")
#define MSG_SLOW_SYNC                   TEXT("Performing slow synchronization")
#define MSG_TWOWAY_SYNC                 TEXT("Performing two-way synchronization")
#define MSG_SYNC_FAIL                   TEXT("Synchronization failed")
#define MSG_COMMITTING_CHANGES          TEXT("Committing changes")
#define MSG_SYNC_SUCCESS                TEXT("Synchronization successful")
#define MSG_FULL_DATE_TIME              TEXT("%s, %s, %s")


// messages for the logging
#define INITIALIZING                            TEXT("Initializing")
#define INITIALIZATION_DONE                     TEXT("Initialization done")
#define SERVER_ALERT_CODE                       TEXT("The server alert code for %s is %i")
#define SYNCHRONIZING                           TEXT("Synchronizing %s")
#define PREPARING_FAST_SYNC                     TEXT("Preparing fast sync for %s")
#define PREPARING_SLOW_SYNC                     TEXT("Preparing slow sync for %s")
#define PREPARING_SYNC_REFRESH_FROM_SERVER      TEXT("Preparing refresh from server sync for %s")
#define PREPARING_SYNC_ONE_WAY_FROM_SERVER      TEXT("Preparing one way from server sync for %s")
#define DETECTED_SLOW                           TEXT("Detected %i items")
#define DETECTED_FAST                           TEXT("Detected %i new items, %i updated items, %i deleted items")
#define SENDING_MODIFICATION                    TEXT("Sending modifications")
#define SENDING_ALERT                           TEXT("Sending alert to get server modifications")
#define RETURNED_NUM_ITEMS                      TEXT("Returned %i new items, %i updated items, %i deleted items for %s")
#define MODIFICATION_DONE                       TEXT("Modification done")
#define SENDING_MAPPING                         TEXT("Sending mapping")
#define SYNCHRONIZATION_DONE                    TEXT("Synchronization done")
#define RESPURI                                 TEXT("url from response to inizialization-message: %s")
#define MESSAGE_SENT                            TEXT("Message sent")
#define READING_RESPONSE                        TEXT("Reading response...")   

#endif
