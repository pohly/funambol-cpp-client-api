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


#ifndef INCL_TAG_NAMES
#define INCL_TAG_NAMES

// The class includes the name of the tag that are used into the parser

#include "base/fscapi.h"
#include "syncml/core/core.h"

#define SYNC_ML             T("SyncML"           )
#define SYNC_BODY           T("SyncBody"         )
#define SYNC_HDR            T("SyncHdr"          )
#define SESSION_ID          T("SessionID"        )
#define VER_PROTO           T("VerProto"         )
#define VER_DTD             T("VerDTD"           )
#define MSG_ID              T("MsgID"            )
#define RESP_URI            T("RespURI"          )
#define NO_RESP             T("NoResp"           )
#define NO_RESULTS          T("NoResults"        )
#define ARCHIVE             T("Archive"          )
#define SFT_DEL             T("SftDel"           )
#define SOURCE              T("Source"           )
#define TARGET              T("Target"           )
#define LOC_URI             T("LocURI"           )
#define LOC_NAME            T("LocName"          )
#define ANCHOR              T("Anchor"           )
#define CRED                T("Cred"             )
#define LAST                T("Last"             )
#define NEXT                T("Next"             )
#define NEXT_NONCE          T("NextNonce"        )
#define FREE_MEM            T("FreeMem"          )
#define SHARED_MEM          T("SharedMem"        )
#define FREE_ID             T("FreeID"           )
#define MAX_MEM             T("MaxMem"           )
#define MAX_ID              T("MaxID"            )
#define FORMAT              T("Format"           )
#define TYPE                T("Type"             )
#define MARK                T("Mark"             )
#define VERSIONSTR          T("Version"          )
#define MAX_MESSAGE_SIZE    T("MaxMsgSize"       )
#define MAX_OBJ_SIZE        T("MaxObjSize"       )
#define SIZE                T("Size"             )
#define VERSIONSTR          T("Version"          )
#define DATA                T("Data"             )
#define FINAL_MSG           T("Final"            )
#define NO_RESP             T("NoResp"           )
#define CMD_ID              T("CmdID"            )
#define COMPLEX_DATA        T("Data"             ) 
#define MORE_DATA           T("MoreData"         )
#define META                T("Meta"             )
#define STATUS              T("Status"           )
#define MSG_REF             T("MsgRef"           )
#define CMD_REF             T("CmdRef"           )
#define TARGET_REF          T("TargetRef"        )
#define SOURCE_REF          T("SourceRef"        )
#define CHAL                T("Chal"             )
#define CMD                 T("Cmd"              )
#define ADD                 T("Add"              )
#define SYNC                T("Sync"             )
#define REPLACE             T("Replace"          )
#define DEL                 T("Delete"           )
#define MAP                 T("Map"              )
#define MAP_ITEM            T("MapItem"          )
#define COPY                T("Copy"             )
#define ATOMIC              T("Atomic"           )
#define SEQUENCE            T("Sequence"         )
#define LANG                T("Lang"             )
#define GET                 T("Get"              )
#define PUT                 T("Put"              )
#define SUPPORT_NUMBER_OF_CHANGES T("SupportNumberOfChanges")
#define NUMBER_OF_CHANGES   T("NumberOfChanges")
#define SUPPORT_LARGE_OBJECT T("SupportLargeObjs")
#define UTC                 T("UTC"              )
#define DATA_STORE          T("DataStore"        )
#define MAN                 T("Man"              )
#define MOD                 T("Mod"              )
#define OEM                 T("OEM"              )
#define FWV                 T("FwV"              )
#define SWV                 T("SwV"              )
#define HWV                 T("HwV"              )
#define DEV_ID              T("DevID"            )
#define DEV_TYP             T("DevTyp"           )
#define DEV_INF             T("DevInf"           )
#define DISPLAY_NAME        T("DisplayName"      )
#define MAX_GUID_SIZE       T("MaxGUIDSize"      )
#define RX_PREF             T("Rx-Pref"          )
#define TX_PREF             T("Tx-Pref"          )
#define CT_TYPE             T("CTType"           )
#define VER_CT              T("VerCT"            )
#define DS_MEM              T("DSMem"            )
#define SYNC_CAP            T("SyncCap"          )
#define SYNC_TYPE           T("SyncType"         )
#define RX                  T("Rx"               )
#define TX                  T("Tx"               )
#define CT_CAP              T("CTCap"            )
#define EXT                 T("Ext"              )
#define XNAM                T("XNam"             )
#define XVAL                T("XVal"             )
#define RESULTS             T("Results"          )
#define EXEC                T("Exec"             )
#define SEARCH              T("Search"           )
#define TARGET_PARENT       T("TargetParent"     )
#define SOURCE_PARENT       T("SourceParent"     )

// parameters 
#define METINFO             T("xmlns=\"syncml:metinf\"")
#define DEVINF              T("xmlns=\"syncml:devinf\"")

// special values
#define DEVINF_URI          T("./devinf11")
#define DEVINF_FORMAT       T("application/vnd.syncml-devinf+xml")

// Commands
#define ALERT               T("Alert"            )
#define ITEM                T("Item"             )
#define FILTER              T("Filter"           )
#define RECORD              T("Record"           )
#define FIELD               T("Field"            )
#define PROPERTY            T("Property"         )
#define DISPLAY_NAME        T("DisplayName"      )
#define PROP_NAME           T("PropName"         )
#define DATA_TYPE           T("DataType"         )
#define MAX_OCCUR           T("MaxOccur"         )
#define MAX_SIZE            T("MaxSize"          )
#define NO_TRUNCATE         T("NoTruncate"       )
#define PROP_PARAM          T("PropParam"        )
#define PARAM_NAME          T("ParamName"        )
#define VAL_ENUM            T("ValEnum"          )
#define VAL_ENUM            T("ValEnum"          )
#define FILTER_TYPE         T("FilterType"       )

#endif






