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
        static SyncML*          getSyncML           (BCHAR* xml);
        static SyncHdr*         getSyncHdr          (BCHAR* xml);
        static SyncBody*        getSyncBody         (BCHAR* xml);        
        static SessionID*       getSessionID        (BCHAR* xml);
        static VerDTD*          getVerDTD           (BCHAR* xml);
        static VerProto*        getVerProto         (BCHAR* xml);
        static Target*          getTarget           (BCHAR* xml);
        static Source*          getSource           (BCHAR* xml);
        static Cred*            getCred             (BCHAR* xml);
        static Anchor*          getAnchor           (BCHAR* xml);
        static NextNonce*       getNextNonce        (BCHAR* xml);        
        static Mem*             getMem              (BCHAR* xml);
        static ArrayList*       getEMI              (BCHAR* xml);
        static Meta*            getMeta             (BCHAR* xml);
        static MetInf*          getMetInf           (BCHAR* xml);
        static Authentication*  getAuthentication   (BCHAR* xml);
        static ArrayList*       getCommands         (BCHAR* xml);
        static Alert*           getAlert            (BCHAR* xml);
        static BOOL             getFinalMsg         (BCHAR* xml);
        static int              getDataCode         (BCHAR* xml);
        static Data*            getData             (BCHAR* xml);
        static BOOL             getNoResp           (BCHAR* xml);
        static BOOL             getNoResults        (BCHAR* xml);
        static CmdID*           getCmdID            (BCHAR* xml);
        static Item*            getItem             (BCHAR* xml);
        static ArrayList*       getItems            (BCHAR* xml);
        static ComplexData*     getComplexData      (BCHAR* xml);
        static BOOL             getMoreData         (BCHAR* xml);
        static Status*          getStatus           (BCHAR* xml);
        static DevInf*          getDevInf           (BCHAR* xml);
        static TargetRef*       getTargetRef        (BCHAR* xml);
        static SourceRef*       getSourceRef        (BCHAR* xml);
        static ArrayList*       getTargetRefs       (BCHAR* xml);
        static ArrayList*       getSourceRefs       (BCHAR* xml);   
        static Chal*            getChal             (BCHAR* xml);   
        static Map*             getMap              (BCHAR* xml);
        static MapItem*         getMapItem          (BCHAR* xml);
        static ArrayList*       getMapItems         (BCHAR* xml);        
        static Add*             getAdd              (BCHAR* xml);
        static Sync*            getSync             (BCHAR* xml);
        static Replace*         getReplace          (BCHAR* xml);
        static Delete*          getDelete           (BCHAR* xml);
        static Copy*            getCopy             (BCHAR* xml);
        static Sequence*        getSequence         (BCHAR* xml);
        static Atomic*          getAtomic           (BCHAR* xml);
        static ArrayList*       getAdds             (BCHAR* xml, BCHAR* except);
        static ArrayList*       getReplaces         (BCHAR* xml, BCHAR* except);
        static ArrayList*       getDels             (BCHAR* xml, BCHAR* except);
        static ArrayList*       getCopies           (BCHAR* xml, BCHAR* except);
        static ArrayList*       getCommonCommandList(BCHAR* xml, BCHAR* except);
        static Get*             getGet              (BCHAR* xml);
        static Put*             getPut              (BCHAR* xml);
        static DataStore*       getDataStore        (BCHAR* xml);
        static ContentTypeInfo* getContentTypeInfo  (BCHAR* xml);
        static DSMem*           getDSMem            (BCHAR* xml);
        static SyncCap*         getSyncCap          (BCHAR* xml);
        static SyncType*        getSyncType         (BCHAR* xml);
        static CTCap*           getCTCap            (BCHAR* xml);
        static Ext*             getExt              (BCHAR* xml);
        static Results*         getResult           (BCHAR* xml);
        static Exec*            getExec             (BCHAR* xml);
        static Search*          getSearch           (BCHAR* xml);
        static ArrayList*       getSources          (BCHAR* xml);
};

#endif




