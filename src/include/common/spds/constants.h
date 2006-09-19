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



#ifndef INCL_SPDS_CONSTANTS
#define INCL_SPDS_CONSTANTS

#include "base/fscapi.h"

#define DIM_SOURCE_NAME      128
#define DIM_SOURCE_URI        64
#define DIM_KEY              256
#define DIM_MIME_TYPE         64
#define DIM_USERNAME         100
#define DIM_PASSWORD         100
#define DIM_DEVICE_ID         50
#define DIM_SERVERNAME       100
#define DIM_SYNC_MODES_LIST   64
#define DIM_SYNC_MODE         16
#define DIM_ANCHOR            32

#define ERR_REPRESENTATION  700

typedef enum SyncMode{
        SYNC_NONE                          = 000,
        SYNC_TWO_WAY                       = 200,
        SYNC_SLOW                          = 201,
        SYNC_ONE_WAY_FROM_CLIENT           = 202,
        SYNC_REFRESH_FROM_CLIENT           = 203,
        SYNC_ONE_WAY_FROM_SERVER           = 204,
        SYNC_REFRESH_FROM_SERVER           = 205,
        SYNC_TWO_WAY_BY_SERVER             = 206,
        SYNC_ONE_WAY_FROM_CLIENT_BY_SERVER = 207,
        SYNC_REFRESH_FROM_CLIENT_BY_SERVER = 208,
        SYNC_ONE_WAY_FROM_SERVER_BY_SERVER = 209,
        SYNC_REFRESG_FROM_SERVER_BY_SERVER = 210,
        //---Funambol extension-----------------
        SYNC_ADDR_CHANGE_NOTIFICATION      = 745
} SyncMode;

#define CONTEXT_SPDS_SYNCML  "/spds/syncml"
#define CONTEXT_SPDS_SOURCES "/spds/sources"
#define CONTEXT_CONN         "/Conn"
#define CONTEXT_AUTH         "/Auth"
#define CONTEXT_DEV_INFO     "/DevInfo"
#define CONTEXT_DEV_DETAIL   "/DevDetail"
#define CONTEXT_EXT          "/Ext"

//-------------------------------------------------------
// Status codes

#define IN_PROGRESS                                   101
#define OK                                            200
#define ITEM_ADDED                                    201
#define ACCEPTED_FOR_PROCESSING                       202
#define NONAUTHORITATIVE_RESPONSE                     203
#define NO_CONTENT                                    204
#define RESET_CONTENT                                 205
#define PARTIAL_CONTENT                               206
#define CONFLICT_RESOLVED_WITH_MERGE                  207
#define CONFLICT_RESOLVED_WITH_CLIENT_COMMAND_WINNING 208
#define CONFLICT_RESOLVED_WITH_DUPLICATE              209
#define DELETE_WITHOUT_ARCHIVE                        210
#define ITEM_NOT_DELETED                              211
#define AUTHENTICATION_ACCEPTED                       212
#define CHUNKED_ITEM_ACCEPTED                         213
#define OPERATION_CANCELLED_OK                        214
#define NOT_EXECUTED                                  215
#define ATOMIC_ROLLBACK_OK                            216
#define MULTIPLE_CHOICES                              300
#define MOVED_PERMANENTLY                             301
#define FOUND                                         302
#define SEE_ANOTHER_URI                               303
#define NOT_MODIFIED                                  304
#define USE_PROXY                                     305
#define BAD_REQUEST                                   400
#define INVALID_CREDENTIALS                           401
#define PAYMENT_REQUIRED                              402
#define FORBIDDEN                                     403
#define NOT_FOUND                                     404
#define COMMAND_NOT_ALLOWED                           405
#define OPTIONAL_FEATURE_NOT_SUPPORTED                406
#define MISSING_CREDENTIALS                           407
#define REQUEST_TIMEOUT                               408
#define UPDATE_CONFLICT                               409
#define GONE                                          410
#define SIZE_REQUIRED                                 411
#define INCOMPLETE_COMMAND                            412
#define REQUESTED_ENTITY_TOO_LARGE                    413
#define URI_TOO_LONG                                  414
#define UNSUPPORTED_MEDIA_TYPE                        415
#define REQUESTED_SIZE_TOO_BIG                        416
#define RETRY_LATER                                   417
#define ALREADY_EXISTS                                418
#define CONFLICT_RESOLVED_WITH_SERVER_DATA            419
#define DEVICE_FULL                                   420
#define UNKNOWN_SEARCH_GRAMMAR                        421
#define BAD_CGI_SCRIPT                                422
#define SOFT_DELETE_CONFLICT                          423
#define OBJECT_SIZE_MISMATCH                          424
#define PERMISSION_DENIED                             425
#define COMMAND_FAILED                                500
#define COMMAND_NOT_IMPLEMENTED                       501
#define BAD_GATEWAY                                   502
#define SERVICE_UNAVAILABLE                           503
#define GATEWAY_TIMEOUT                               504
#define VERSION_NOT_SUPPORTED                         505
#define PROCESSING_ERROR                              506
#define ATOMIC_FAILED                                 507
#define REFRESH_REQUIRED                              508
#define RECIPIENT_EXCEPTION_RESERVED1                 509
#define DATASTORE_FAILURE                             510
#define SERVER_FAILURE                                511
#define SYNCHRONIZATION_FAILED                        512
#define PROTOCOL_VERSION_NOT_SUPPORTED                513
#define OPERATION_CANCELLED                           514
#define ATOMIC_ROLLBACK_FAILED                        516
#define ATOMIC_RESPONSE_TOO_LARGE_TO_FIT              517

// ------------------------------------------------------

//-------------------------------------------------------
// Commands

#define COMMAND_ADD     T("Add")
#define COMMAND_DELETE  T("Delete")
#define COMMAND_GET     T("Get")
#define COMMAND_PUT     T("Put")
#define COMMAND_REPLACE T("Replace")
#define COMMAND_RESULTS T("Results")
#define COMMAND_STATUS  T("Status")
#define COMMAND_SYNC    T("Sync")

//-------------------------------------------------------

#ifndef NULL
    #define NULL 0x00
#endif


char*  retSyncMode(SyncMode) EXTRA_SECTION_01;

#endif
