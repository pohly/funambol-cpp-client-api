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


#ifndef INCL_PARSER
#define INCL_PARSER

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/util/XMLProcessor.h"
#include "base/util/ArrayList.h"
#include "syncml/core/ObjectDel.h"

class Parser {
     
    // ---------------------------------------------------------- Public data    
    public:
        static SyncML*          getSyncML           (char*  xml);
        static SyncHdr*         getSyncHdr          (char*  xml);
        static SyncBody*        getSyncBody         (char*  xml);        
        static SessionID*       getSessionID        (char*  xml);
        static VerDTD*          getVerDTD           (char*  xml);
        static VerProto*        getVerProto         (char*  xml);
        static Target*          getTarget           (char*  xml);
        static Source*          getSource           (char*  xml);
        static Cred*            getCred             (char*  xml);
        static Anchor*          getAnchor           (char*  xml);
        static NextNonce*       getNextNonce        (char*  xml);        
        static Mem*             getMem              (char*  xml);
        static ArrayList*       getEMI              (char*  xml);
        static Meta*            getMeta             (char*  xml);
        static MetInf*          getMetInf           (char*  xml);
        static Authentication*  getAuthentication   (char*  xml);
        static ArrayList*       getCommands         (char*  xml);
        static Alert*           getAlert            (char*  xml);
        static BOOL             getFinalMsg         (char*  xml);
        static int              getDataCode         (char*  xml);
        static Data*            getData             (char*  xml);
        static BOOL             getNoResp           (char*  xml);
        static BOOL             getNoResults        (char*  xml);
        static CmdID*           getCmdID            (char*  xml);
        static Item*            getItem             (char*  xml, const char* command = NULL);
        static ArrayList*       getItems            (char*  xml, const char* command = NULL);
        static ComplexData*     getComplexData      (char*  xml, const char* command = NULL);
        static BOOL             getMoreData         (char*  xml);
        static Status*          getStatus           (char*  xml);
        static DevInf*          getDevInf           (char*  xml);
        static TargetRef*       getTargetRef        (char*  xml);
        static SourceRef*       getSourceRef        (char*  xml);
        static ArrayList*       getTargetRefs       (char*  xml);
        static ArrayList*       getSourceRefs       (char*  xml);   
        static Chal*            getChal             (char*  xml);   
        static Map*             getMap              (char*  xml);
        static MapItem*         getMapItem          (char*  xml);
        static ArrayList*       getMapItems         (char*  xml);        
        static Add*             getAdd              (char*  xml);
        static Sync*            getSync             (char*  xml);
        static Replace*         getReplace          (char*  xml);
        static Delete*          getDelete           (char*  xml);
        static Copy*            getCopy             (char*  xml);
        static Sequence*        getSequence         (char*  xml);
        static Atomic*          getAtomic           (char*  xml);
        static ArrayList*       getAdds             (char*  xml, char*  except);
        static ArrayList*       getReplaces         (char*  xml, char*  except);
        static ArrayList*       getDels             (char*  xml, char*  except);
        static ArrayList*       getCopies           (char*  xml, char*  except);
        static ArrayList*       getCommonCommandList(char*  xml, char*  except);
        static Get*             getGet              (char*  xml);
        static Put*             getPut              (char*  xml);
        static DataStore*       getDataStore        (char*  xml);
        static ContentTypeInfo* getContentTypeInfo  (char*  xml);
        static DSMem*           getDSMem            (char*  xml);
        static SyncCap*         getSyncCap          (char*  xml);
        static SyncType*        getSyncType         (char*  xml);
        static CTCap*           getCTCap            (char*  xml);
        static Ext*             getExt              (char*  xml);
        static Results*         getResult           (char*  xml);
        static Exec*            getExec             (char*  xml);
        static Search*          getSearch           (char*  xml);
        static ArrayList*       getSources          (char*  xml);
};

#endif




