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
        static SyncML*          getSyncML           (wchar_t* xml);
        static SyncHdr*         getSyncHdr          (wchar_t* xml);
        static SyncBody*        getSyncBody         (wchar_t* xml);        
        static SessionID*       getSessionID        (wchar_t* xml);
        static VerDTD*          getVerDTD           (wchar_t* xml);
        static VerProto*        getVerProto         (wchar_t* xml);
        static Target*          getTarget           (wchar_t* xml);
        static Source*          getSource           (wchar_t* xml);
        static Cred*            getCred             (wchar_t* xml);
        static Anchor*          getAnchor           (wchar_t* xml);
        static NextNonce*       getNextNonce        (wchar_t* xml);        
        static Mem*             getMem              (wchar_t* xml);
        static ArrayList*       getEMI              (wchar_t* xml);
        static Meta*            getMeta             (wchar_t* xml);
        static MetInf*          getMetInf           (wchar_t* xml);
        static Authentication*  getAuthentication   (wchar_t* xml);
        static ArrayList*       getCommands         (wchar_t* xml);
        static Alert*           getAlert            (wchar_t* xml);
        static BOOL             getFinalMsg         (wchar_t* xml);
        static int              getDataCode         (wchar_t* xml);
        static Data*            getData             (wchar_t* xml);
        static BOOL             getNoResp           (wchar_t* xml);
        static BOOL             getNoResults        (wchar_t* xml);
        static CmdID*           getCmdID            (wchar_t* xml);
        static Item*            getItem             (wchar_t* xml);
        static ArrayList*       getItems            (wchar_t* xml);
        static ComplexData*     getComplexData      (wchar_t* xml);
        static BOOL             getMoreData         (wchar_t* xml);
        static Status*          getStatus           (wchar_t* xml);
        static DevInf*          getDevInf           (wchar_t* xml);
        static TargetRef*       getTargetRef        (wchar_t* xml);
        static SourceRef*       getSourceRef        (wchar_t* xml);
        static ArrayList*       getTargetRefs       (wchar_t* xml);
        static ArrayList*       getSourceRefs       (wchar_t* xml);   
        static Chal*            getChal             (wchar_t* xml);   
        static Map*             getMap              (wchar_t* xml);
        static MapItem*         getMapItem          (wchar_t* xml);
        static ArrayList*       getMapItems         (wchar_t* xml);        
        static Add*             getAdd              (wchar_t* xml);
        static Sync*            getSync             (wchar_t* xml);
        static Replace*         getReplace          (wchar_t* xml);
        static Delete*          getDelete           (wchar_t* xml);
        static Copy*            getCopy             (wchar_t* xml);
        static Sequence*        getSequence         (wchar_t* xml);
        static Atomic*          getAtomic           (wchar_t* xml);
        static ArrayList*       getAdds             (wchar_t* xml, wchar_t* except);
        static ArrayList*       getReplaces         (wchar_t* xml, wchar_t* except);
        static ArrayList*       getDels             (wchar_t* xml, wchar_t* except);
        static ArrayList*       getCopies           (wchar_t* xml, wchar_t* except);
        static ArrayList*       getCommonCommandList(wchar_t* xml, wchar_t* except);
        static Get*             getGet              (wchar_t* xml);
        static Put*             getPut              (wchar_t* xml);
        static DataStore*       getDataStore        (wchar_t* xml);
        static ContentTypeInfo* getContentTypeInfo  (wchar_t* xml);
        static DSMem*           getDSMem            (wchar_t* xml);
        static SyncCap*         getSyncCap          (wchar_t* xml);
        static SyncType*        getSyncType         (wchar_t* xml);
        static CTCap*           getCTCap            (wchar_t* xml);
        static Ext*             getExt              (wchar_t* xml);
        static Results*         getResult           (wchar_t* xml);
        static Exec*            getExec             (wchar_t* xml);
        static Search*          getSearch           (wchar_t* xml);
        static ArrayList*       getSources          (wchar_t* xml);
};

#endif




