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


#define PROPERTY_USERNAME              T("username"         )
#define PROPERTY_PASSWORD              T("password"         )
#define PROPERTY_FIRST_TIME_SYNC_MODE  T("firstTimeSyncMode")
#define PROPERTY_USE_PROXY             T("useProxy"         )
#define PROPERTY_PROXY_HOST            T("proxyHost"        )
#define PROPERTY_PROXY_PORT            T("proxyPort"        )
#define PROPERTY_PROXY_USERNAME        T("proxyUsername"    )
#define PROPERTY_PROXY_PASSWORD        T("proxyPassword"    )
#define PROPERTY_SERVER_NAME           T("serverName"       )
#define PROPERTY_SYNC_URL              T("syncUrl"          )
#define PROPERTY_SYNC_BEGIN            T("begin"            )
#define PROPERTY_SYNC_END              T("end"              )
#define PROPERTY_SOURCE_NAME           T("name"             )
#define PROPERTY_SOURCE_URI            T("uri"              )
#define PROPERTY_SOURCE_SYNC_MODES     T("syncModes"        )
#define PROPERTY_SOURCE_TYPE           T("type"             )
#define PROPERTY_SOURCE_VERSION        T("version"          )
#define PROPERTY_SOURCE_SYNC           T("sync"             )
#define PROPERTY_SOURCE_LAST_SYNC      T("last"             )
#define PROPERTY_SOURCE_CTCAP          T("ctCap"            )
#define PROPERTY_SERVER_NONCE          T("serverNonce"      ) 
#define PROPERTY_CLIENT_NONCE          T("clientNonce"      )
#define PROPERTY_SERVER_ID             T("serverID"         )
#define PROPERTY_SERVER_PWD            T("serverPWD"        )
#define PROPERTY_CLIENT_AUTH_TYPE      T("clientAuthType"   )
#define PROPERTY_SERVER_AUTH_TYPE      T("serverAuthType"   )
#define PROPERTY_IS_SERVER_REQUIRED    T("isServerAuthRequired")
#define PROPERTY_MAX_MSG_SIZE          T("maxMsgSize"       )
#define PROPERTY_MAX_MOD_PER_MSG       T("maxModPerMsg"     )
#define PROPERTY_SOURCE_DOWNLOAD_AGE   T("downloadAge"      )
#define PROPERTY_SOURCE_BODY_SIZE      T("bodySize"         )
#define PROPERTY_SOURCE_ATTACH_SIZE    T("attachSize"       )
#define PROPERTY_SOURCE_INBOX          T("Inbox"            )
#define PROPERTY_SOURCE_OUTBOX         T("Outbox"           )
#define PROPERTY_SOURCE_DRAFT          T("Draft"            )
#define PROPERTY_SOURCE_TRASH          T("Trash"            )
#define PROPERTY_SOURCE_SENT           T("Sent"             )
#define PROPERTY_SOURCE_ENCODING       T("encoding"         )
#define PROPERTY_SOURCE_SUPP_TYPES     T("supportedTypes"   )
#define PROPERTY_READ_BUFFER_SIZE      T("readBufferSize"   )
#define PROPERTY_USER_AGENT            T("userAgent"        )
#define PROPERTY_CHECK_CONN            T("checkConn"        )
#define PROPERTY_RESPONSE_TIMEOUT      T("responseTimeout"  )
#define PROPERTY_SOURCE_SCHEDULE       T("schedule"         )
#define PROPERTY_SOURCE_ENCRYPTION     T("encryption"       )

// DeviceConfig properties
#define PROPERTY_VER_DTD                    T("verDTD"          )
#define PROPERTY_MANUFACTURER               T("man"             )
#define PROPERTY_MODEL                      T("mod"             )
#define PROPERTY_OEM                        T("oem"             )
#define PROPERTY_FIRMWARE_VERSION           T("fwv"             )
#define PROPERTY_SOFTWARE_VERSION           T("swv"             )
#define PROPERTY_HARDWARE_VERSION           T("hwv"             )
#define PROPERTY_DEVICE_ID                  T("devID"           )
#define PROPERTY_DEVICE_TYPE                T("devType"         )
#define PROPERTY_DS_VERSION                 T("dsV"             )
#define PROPERTY_UTC                        T("utc"             )
#define PROPERTY_LARGE_OBJECT_SUPPORT       T("loSupport"       )
#define PROPERTY_NUMBER_OF_CHANGES_SUPPORT  T("nocSupport"      )
#define PROPERTY_LOG_LEVEL                  T("logLevel"        )
#define PROPERTY_MAX_OBJ_SIZE               T("maxObjSize"      )
#define PROPERTY_DEVINF_HASH                T("devInfHash"      )
        

        
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
#define DIRTY_DEV_INF_HASH             0x40000


#endif
