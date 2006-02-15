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

#ifndef INCL_SPDM_CONSTANTS
#define INCL_SPDM_CONSTANTS

#define DIM_MANAGEMENT_PATH 512	
#define DIM_PROPERTY_NAME    64

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 512


#define PROPERTY_USERNAME              TEXT("username"         )
#define PROPERTY_PASSWORD              TEXT("password"         )
#define PROPERTY_DEVICE_ID             TEXT("deviceId"         )
#define PROPERTY_FIRST_TIME_SYNC_MODE  TEXT("firstTimeSyncMode")
#define PROPERTY_USE_PROXY             TEXT("useProxy"         )
#define PROPERTY_PROXY_HOST            TEXT("proxyHost"        )
#define PROPERTY_PROXY_PORT            TEXT("proxyPort"        )
#define PROPERTY_SERVER_NAME           TEXT("serverName"       )
#define PROPERTY_SYNC_URL              TEXT("syncUrl"          )
#define PROPERTY_SYNC_BEGIN            TEXT("begin"            )
#define PROPERTY_SYNC_END              TEXT("end"              )
#define PROPERTY_SOURCE_NAME           TEXT("name"             )
#define PROPERTY_SOURCE_URI            TEXT("uri"              )
#define PROPERTY_SOURCE_SYNC_MODES     TEXT("syncModes"        )
#define PROPERTY_SOURCE_TYPE           TEXT("type"             )
#define PROPERTY_SOURCE_SYNC           TEXT("sync"             )
#define PROPERTY_SOURCE_LAST_SYNC      TEXT("last"             )
#define PROPERTY_SERVER_NONCE          TEXT("serverNonce"      ) 
#define PROPERTY_CLIENT_NONCE          TEXT("clientNonce"      )
#define PROPERTY_SERVER_ID             TEXT("serverID"         )
#define PROPERTY_SERVER_PWD            TEXT("serverPWD"        )
#define PROPERTY_CLIENT_AUTH_TYPE      TEXT("clientAuthType"   )
#define PROPERTY_SERVER_AUTH_TYPE      TEXT("serverAuthType"   )
#define PROPERTY_IS_SERVER_REQUIRED    TEXT("isServerAuthRequired")
#define PROPERTY_MAX_MSG_SIZE          TEXT("maxMsgSize"       )
#define PROPERTY_MAX_MOD_PER_MSG       TEXT("maxModPerMsg"     )
#define PROPERTY_SOURCE_DOWNLOAD_AGE   TEXT("downloadAge"      )
#define PROPERTY_SOURCE_BODY_SIZE      TEXT("bodySize"         )
#define PROPERTY_SOURCE_ATTACH_SIZE    TEXT("attachSize"       )
#define PROPERTY_SOURCE_INBOX          TEXT("Inbox"            )
#define PROPERTY_SOURCE_OUTBOX         TEXT("Outbox"           )
#define PROPERTY_SOURCE_DRAFT          TEXT("Draft"            )
#define PROPERTY_SOURCE_TRASH          TEXT("Trash"            )
#define PROPERTY_SOURCE_SENT           TEXT("Sent"             )
#define PROPERTY_ENCRYPTION            TEXT("encryption"       )
#define PROPERTY_SOURCE_ENCODING       TEXT("encoding"         )


        

        
#define DIRTY_USERNAME                 0x00001
#define DIRTY_PASSWORD                 0x00002
#define DIRTY_DEVICE_ID                0x00004
#define DIRTY_FIRST_TIME_SYNC_MODE     0x00008
#define DIRTY_USE_PROXY                0x00010
#define DIRTY_PROXY_HOST               0x00020
#define DIRTY_PROXY_PORT               0x00040
#define DIRTY_SERVER_NAME              0x00080
#define DIRTY_SYNC_URL                 0x00100
#define DIRTY_SYNC_BEGIN               0x00200
#define DIRTY_SYNC_END                 0x00400
#define DIRTY_SYNC_SOURCE              0x00800
#define DIRTY_CLIENT_NONCE             0x01000
#define DIRTY_SERVER_NONCE             0x02000
#define DIRTY_SERVERID                 0x04000
#define DIRTY_SERVERPWD                0x08000 
#define DIRTY_CLIENTAUTHTYPE           0x10000 
#define DIRTY_SERVERAUTH_REQUIRED      0x20000


#endif
