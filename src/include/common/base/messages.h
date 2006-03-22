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

#define MSG_CONFIG_SUCCESSFULLY_READ    T("Configuration successfully read")
#define MSG_INITIALIZATATION_MESSAGE    T("Initialization message:")
#define MSG_UNKNOWN                     T("Unknown")
#define MSG_PREPARING_SYNC              T("Preparing synchronization of %s...")
#define MSG_SYNC_URL                    T("Synchronization URL: %s")
#define MSG_NOT_AUTHORIZED              T("Sorry, you are not authorized to synchronize. Check the username/password settings.")
#define MSG_DB_NOT_FOUND                T("Remote database %s not found. Check the remote settings.")
#define MSG_INVALID_URL                 T("Invalid synchronization URL. Check the connection settings.")
#define MSG_MODIFICATION_MESSAGE        T("Modification message:")
#define MSG_OUT_OF_MEMORY               T("Out of memory!")
#define MSG_APPLYING_MODIFICATIONS      T("Applying server modifications: %ld new item(s), %ld updated item(s), %ld deleted item(s)")
#define MSG_ADDING_ITEMS                T("Adding %ld item(s)")
#define MSG_UPDATING_ITEMS              T("Updating %ld item(s)")
#define MSG_DELETING_ITEMS              T("Deleting %ld item(s)")
#define MSG_SYNCHRONIZING               T("Synchronizing...")
#define MSG_SLOW_SYNC                   T("Performing slow synchronization")
#define MSG_TWOWAY_SYNC                 T("Performing two-way synchronization")
#define MSG_SYNC_FAIL                   T("Synchronization failed")
#define MSG_COMMITTING_CHANGES          T("Committing changes")
#define MSG_SYNC_SUCCESS                T("Synchronization successful")
#define MSG_FULL_DATE_TIME              T("%s, %s, %s")


// messages for the logging
#define INITIALIZING                            T("Initializing")
#define INITIALIZATION_DONE                     T("Initialization done")
#define SERVER_ALERT_CODE                       T("The server alert code for %s is %i")
#define SYNCHRONIZING                           T("Synchronizing %s")
#define PREPARING_FAST_SYNC                     T("Preparing fast sync for %s")
#define PREPARING_SLOW_SYNC                     T("Preparing slow sync for %s")
#define PREPARING_SYNC_REFRESH_FROM_SERVER      T("Preparing refresh from server sync for %s")
#define PREPARING_SYNC_ONE_WAY_FROM_SERVER      T("Preparing one way from server sync for %s")
#define DETECTED_SLOW                           T("Detected %i items")
#define DETECTED_FAST                           T("Detected %i new items, %i updated items, %i deleted items")
#define SENDING_MODIFICATION                    T("Sending modifications")
#define SENDING_ALERT                           T("Sending alert to get server modifications")
#define RETURNED_NUM_ITEMS                      T("Returned %i new items, %i updated items, %i deleted items for %s")
#define MODIFICATION_DONE                       T("Modification done")
#define SENDING_MAPPING                         T("Sending mapping")
#define SYNCHRONIZATION_DONE                    T("Synchronization done")
#define RESPURI                                 T("url from response to inizialization-message: %s")
#define MESSAGE_SENT                            T("Message sent")
#define READING_RESPONSE                        T("Reading response...")   

#endif
