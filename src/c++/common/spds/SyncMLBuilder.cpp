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

#include <string.h>

#include "base/fscapi.h"
#include "base/base64.h"
#include "base/util/utils.h"
#include "base/util/StringBuffer.h"
#include "filter/ClauseUtil.h"
#include "spds/constants.h"
#include "spds/DataTransformerFactory.h"
#include "spds/SyncItem.h"
#include "spds/SyncMLBuilder.h"

#include "event/FireEvent.h"


SyncMLBuilder::SyncMLBuilder() {
    initialize();
    set(NULL, NULL);
}

SyncMLBuilder::~SyncMLBuilder() {
    safeDelete(&target  );    
    safeDelete(&device  );     
    safeDelete(&encPassword);
}


SyncMLBuilder::SyncMLBuilder(BCHAR* t, BCHAR* d) {
    initialize();
    set(t, d);
}

void SyncMLBuilder::set(BCHAR* t, BCHAR* d) {    

    target   = stringdup(t);   
    device   = stringdup(d);   
}

void SyncMLBuilder::initialize() {
    sessionID = (unsigned long)time(NULL);
    msgRef    = 0         ;
    msgID     = 0         ;
    cmdID     = 0         ;
    encoding   = PLAIN    ;
    encPassword = NULL    ;
}

void SyncMLBuilder::setEncoding(DataEncoding e) {
    encoding = e;
}

DataEncoding SyncMLBuilder::getEncoding() {
    return encoding;
}

void SyncMLBuilder::resetCommandID() {
    cmdID = 0;
}

void SyncMLBuilder::increaseMsgRef() {
    msgRef++;
}

void SyncMLBuilder::resetMsgRef() {
    msgRef = 0;
}

void SyncMLBuilder::addItemStatus(ArrayList* previousStatus, Status* status) {
    
    if (previousStatus->size() == 0) {
        previousStatus->add(*status);
        return;
    }
    
    BOOL found    = FALSE;    
    Status* s     = NULL;

    if (status == NULL)
        return;
    ArrayList* list = new ArrayList();
    for (int i = 0; i < previousStatus->size(); i++) {
    
        s = (Status*)previousStatus->get(i);
        if ((bstrcmp(s->getCmd(NULL), status->getCmd(NULL)) == 0) &&
            (bstrcmp(s->getData()->getData(NULL), status->getData()->getData(NULL)) == 0) &&  
            (bstrcmp(s->getCmdRef(NULL), status->getCmdRef(NULL)) == 0) ) {                    
                    list = s->getItems();
                    for (int j = 0; j < status->getItems()->size(); j++) {                        
                        list->add(*((Item*)(status->getItems())->get(j)));  
                        found = TRUE;
                    }                    
                }                               
                                                    
    }    
    if (!found)
        previousStatus->add(*status);
    
}



/*
* Return the status of the items sent by server. Used to create the status to respond
* after a add, replace or delete command
*/
Status* SyncMLBuilder::prepareItemStatus(const BCHAR* COMMAND,
                                         const BCHAR* key,
                                         const BCHAR* cmdRef,
                                         int code) {
    /*                                                                                               
    <Status>                                 CmdID*        cmdID     ,                           
        <CmdID>3</CmdID>                     BCHAR*      msgRef    ,                           
        <MsgRef>2</MsgRef>                   BCHAR*      cmdRef    ,                           
        <CmdRef>4</CmdRef>                   BCHAR*      cmd       ,                           
        <Cmd>Replace</Cmd>                   ArrayList*    targetRefs,                           
        <Data>201</Data>                     ArrayList*    sourceRefs,                           
        <Item>                               Cred*         cred      ,                           
            <Source>                         Chal*         chal      ,                           
            <LocURI>item0</LocURI>           Data*         data      ,                           
            </Source>                        ArrayList*    items     );                          
        </Item>                                                                                  
        <Item>                                                                                   
            <Source>                                                                             
            <LocURI>item1</LocURI>                                                               
            </Source>                                                                            
        </Item>                                                                                  
    </Status>                                                                                    
    */                                                                                           

    ++cmdID;
    BCHAR* cmdid = itow(cmdID);
    CmdID* commandID  = new CmdID(cmdid);    
    ArrayList* empty  = new ArrayList();    
    Data*      data   = new Data(code);    
    ArrayList* list   = new ArrayList();                
    Source* sou       = new Source(key);   
    Item* item        = new Item(NULL, sou, NULL, NULL, FALSE);
    list->add(*item);

    BCHAR *mRef = itow(msgRef);
    Status* s = new Status(commandID, mRef, cmdRef, COMMAND, empty, empty, NULL, NULL, data, list);
    delete [] mRef;
    
    safeDelete(&cmdid);
    deleteCmdID(&commandID);
    deleteData(&data);
    deleteSource(&sou);    
    deleteItem(&item);
    deleteArrayList(&empty);
    deleteArrayList(&list);
    if (list)  { delete list; list = NULL; }
    if (empty) { delete empty; empty = NULL; }

    return s;

}

/*
* Return the status of the server authentication process
*/
Status* SyncMLBuilder::prepareSyncHdrStatus(Chal*chal, int d) {
    
    ++cmdID;
    BCHAR* cmdid = itow(cmdID);
    CmdID* commandID         = new CmdID(cmdid);
    ArrayList*    targetRefs = new ArrayList();
    ArrayList*    sourceRefs = new ArrayList();
    TargetRef*    tar        = new TargetRef(target);
    SourceRef*    sou        = new SourceRef(device);   
    Data*      data          = new Data(d);
    targetRefs->add(*tar);
    sourceRefs->add(*sou);

    Status* s = new Status(commandID, itow(msgRef), T("0"), SYNC_HDR, targetRefs, sourceRefs, NULL, chal, data, NULL);

    // Fire Sync Status Event: syncHdr status from client
    fireSyncStatusEvent(SYNC_HDR, s->getStatusCode(), NULL, NULL , CLIENT_STATUS);
    
    safeDelete(&cmdid);
    deleteCmdID(&commandID);
    deleteArrayList(&targetRefs);
    deleteArrayList(&sourceRefs);
    deleteTargetRef(&tar);
    deleteSourceRef(&sou);
    deleteData(&data);

    return s;

}

Status* SyncMLBuilder::prepareSyncStatus(SyncSource& source, Sync* sync) {
    if (sync == NULL)
        return NULL;
    
    ++cmdID;

    CmdID* commandID         = new CmdID(itow(cmdID));
    ArrayList*    targetRefs = new ArrayList();
    ArrayList*    sourceRefs = new ArrayList();
    CmdID* cmdRef            = sync->getCmdID();

    
    TargetRef*    tar        = new TargetRef(source.getConfig().getURI());
    SourceRef*    sou        = new SourceRef(_wcc(source.getName()));
    targetRefs->add(*tar);
    sourceRefs->add(*sou);
    Data* d                  = new Data(200);
    
    Status* s = new Status(commandID, itow(msgRef), cmdRef->getCmdID(), SYNC, targetRefs, sourceRefs, NULL, NULL, d, NULL);

    // Fire Sync Status Event: sync status from client
    fireSyncStatusEvent(SYNC, s->getStatusCode(), source.getConfig().getURI(), NULL, CLIENT_STATUS);

    deleteCmdID(&commandID);
    deleteArrayList(&targetRefs);
    deleteArrayList(&sourceRefs);
    deleteTargetRef(&tar);
    deleteSourceRef(&sou);    
    deleteData(&d);

    return s;

}


/*
* Return the status against an alert command. It could be releated on the authentication process
* or any other else
*/
Status* SyncMLBuilder::prepareAlertStatus(SyncSource& source, ArrayList* alerts, int authStatusCode) {
    /*
    next = source.getNextAnchor(NULL);
    <Status>\n
        <CmdID>2</CmdID>\n
        <MsgRef>1</MsgRef>
        <CmdRef>1</CmdRef>
        <Cmd>Alert</Cmd>\n
        <TargetRef>
            sb.append(source.getRemoteURI(NULL));
        </TargetRef>\n
        <SourceRef>
            sb.append(source.getName(NULL, -1));
        </SourceRef>\n
        <Data>200</Data>\n
        <Item>\n
            <Data>
                <Anchor xmlns='syncml:metinf'>
                    <Next>
                        sb.append(next);    
                    </Next>
                </Anchor>\n
            </Data>\n
        </Item>\n
    </Status>
    */
    
    if (alerts == NULL || alerts->size() == 0)
        return NULL;
    
    Alert* a = NULL;
    Item* item = NULL;
    ArrayList* list = new ArrayList();
    BOOL found = FALSE;

    for (int i = 0; i < alerts->size(); i++) {
        a = (Alert*)alerts->get(i);
        list = a->getItems();
        if (list->size() == 1) {
            item = (Item*)list->get(0);
            if (bstrcmp(item->getTarget()->getLocURI(), _wcc(source.getName())) == 0) {
                found = TRUE;
            }
        }
    }
    
    //
    // Currently it returns null. It could return a status value if necessary
    //
    if (!found) {
        return NULL;

    }

    ++cmdID;

    CmdID* commandID         = new CmdID(itow(cmdID));
    ArrayList*    targetRefs = new ArrayList();
    ArrayList*    sourceRefs = new ArrayList();
    
    TargetRef*    tar        = new TargetRef(source.getConfig().getURI());
    SourceRef*    sou        = new SourceRef(_wcc(source.getName()));
    targetRefs->add(*tar);
    sourceRefs->add(*sou);
    CmdID* cmdRef            = a->getCmdID();
    BCHAR* next            = NULL;
    int authStatus           = 0;
    
    if (authStatusCode >= 200 && authStatusCode <=299)
        authStatus = 200;
     else
        authStatus = authStatusCode;

    Data* d              = new Data(authStatus);
    ComplexData* data    = NULL;
    ArrayList* items     = new ArrayList();
    Anchor*    anchor    = NULL;

    if (authStatusCode < 400) {
        list = a->getItems();
        if (list->size() > 0) {
            for (int i = 0; i < list->size(); i++) {
                Item* it = (Item*)list->get(i);
                Meta* m = it->getMeta();
                if (m) {
                    anchor = m->getAnchor();
                    next = stringdup(anchor->getNext());
                }

            }

            anchor = new Anchor(NULL, next);
            data = new ComplexData();
            data->setAnchor(anchor);
            item           = new Item(NULL, NULL, NULL, data, FALSE);        
            items->add(*item);
        }
    }
    
    Status* s = new Status(commandID, itow(msgRef), cmdRef->getCmdID(), ALERT, targetRefs, sourceRefs, NULL, NULL, d, items);
		        
    // Fire Sync Status Event: alert status from client
    fireSyncStatusEvent(ALERT, s->getStatusCode(), source.getConfig().getURI(), NULL , CLIENT_STATUS);

    deleteCmdID(&commandID);
    deleteArrayList(&targetRefs);
    deleteArrayList(&sourceRefs);
    deleteTargetRef(&tar);
    deleteSourceRef(&sou);
    deleteItem(&item);
    deleteAnchor(&anchor);
    deleteComplexData(&data);    
    deleteData(&d);
    safeDel(&next);

    return s;
}

/*
* Return the status against an arbitrary command.
*/
Status* SyncMLBuilder::prepareCmdStatus(AbstractCommand &cmd, int status) {
    /*
    <Status>\n
        <CmdID>2</CmdID>\n
        <MsgRef>1</MsgRef>
        <CmdRef>1</CmdRef>
        <Cmd>cmd</Cmd>\n
        <Data>status</Data>\n
    </Status>
    */
    
    ++cmdID;

    CmdID commandID(itow(cmdID));
    Data d(status);
    char *msgRefStr = itow(msgRef);
    ArrayList empty;

    Status* s = new Status(&commandID, msgRefStr, cmd.getCmdID()->getCmdID(), cmd.getName(), &empty, &empty, NULL, NULL, &d, NULL);
    
    // Fire Sync Status Event: status from client
    fireSyncStatusEvent(s->getCmd(), s->getStatusCode(), NULL, NULL , CLIENT_STATUS);

    delete [] msgRefStr;
    return s;
}

AbstractCommand *SyncMLBuilder::prepareDevInf(AbstractCommand *cmd, DevInf &devInf)
{
    AbstractCommand *res = NULL;
    char *msgRefStr = NULL;

    Source source(DEVINF_URI);
    Meta meta;
    meta.setType(DEVINF_FORMAT);
    // meta.setFormat(T("xml"));
    ComplexData complexData;
    complexData.setDevInf(&devInf);
    Item item(NULL,
              &source,
              NULL,
              &complexData,
              FALSE);
    

    ++cmdID;
    CmdID commandID(itow(cmdID));
    ArrayList items;
    items.add(item);

    if (cmd) {
        /*
          <Result>
          <CmdID>2</CmdID>
          <MsgRef>1</MsgRef>
          <CmdRef>4</CmdRef>
          <Meta><Type xmlns='syncml:metinf'>application/vnd.syncml-devinf+xml</Type></Meta>
          <Item>
            <SourceRef><LocURI>./devinf11</LocURI></SourceRef>
            <Data>
              <DevInf>...</DevInf>
            </Data>
          </Item>
          </Result>
        */

        msgRefStr = itow(msgRef);
        ArrayList empty;
        res = new Results(&commandID,
                          msgRefStr,
                          cmd->getCmdID()->getCmdID(),
                          &meta,
                          &empty,
                          &empty,
                          &items);
    } else {
        /*
          <Put>
            <CmdID>2</CmdID>
            <Meta><Type xmlns='syncml:metinf'>application/vnd.syncml-devinf+xml</Type></Meta>
            <Item>
              <Source><LocURI>./devinf11</LocURI></Source>
              <Data>
                <DevInf xmlns='syncml:devinf'>...</DevInf>
              </Data>
            </Item>
          </Put>
        */

        res = new Put(&commandID,
                      FALSE,
                      NULL,
                      NULL,
                      &meta,
                      &items);
    }
    safeDelete(&msgRefStr);
    return res;
}


Alert* SyncMLBuilder::prepareAlert(SyncSource& s, int code) {
    
    ++cmdID;

    CmdID* commandID     = new CmdID(itow(cmdID));    
    Target* tar          = new Target(s.getConfig().getURI());
    Source* sou          = new Source(_wcc(s.getName()));     
    Item* item           = new Item(tar, sou, NULL, NULL, FALSE);

    ArrayList* list      = new ArrayList();    
    list->add(*item);    
    Alert* alert         = new Alert(commandID, false, NULL, code, list);
    
    deleteTarget(&tar);
    deleteSource(&sou);
    deleteItem(&item);
    deleteCmdID(&commandID);
    deleteArrayList(&list);

    return alert;
}

Alert* SyncMLBuilder::prepareInitAlert(SyncSource& s) {
    
    ++cmdID;

    CmdID* commandID     = new CmdID(itow(cmdID));
    int data             = s.getPreferredSyncMode();
    Target* tar          = new Target(s.getConfig().getURI());
    Source* sou          = new Source(_wcc(s.getName()));

    //
    // Read the clause filter from the source and translate
    // it into a Filter. This must be assigned to the Target
    //
    SourceFilter* sourceFilter = s.getFilter();
    if (sourceFilter) {
        Filter* filter = ClauseUtil::toFilter(*sourceFilter);
        tar->setFilter(filter);
        delete filter; filter = NULL;
    }
    
    Anchor*    anchor    = new Anchor(s.getLastAnchor(), s.getNextAnchor());    
    MetInf* metInf       = new MetInf(NULL, NULL, NULL, NULL, 
                               anchor, NULL, NULL, NULL, NULL, NULL, NULL); 
    Meta* meta           = new Meta();
    meta->setMetInf(metInf);
    Item* item           = new Item(tar, sou, meta, NULL, FALSE);

    ArrayList* list      = new ArrayList();    
    list->add(*item);    
    Alert* alert         = new Alert(commandID, false, NULL, data, list);    
    
    deleteCmdID(&commandID);
    deleteTarget(&tar);
    if(sou)
        delete sou;
    deleteAnchor(&anchor);
    deleteMetInf(&metInf);
    deleteMeta(&meta);
    deleteItem(&item);
    deleteArrayList(&list);
    
    return alert;
}

Alert* SyncMLBuilder::prepareAddrChangeAlert(SyncSource& s) {
    
    SyncItem *syncItem;
    ArrayList list;
    for(syncItem = s.getFirstItem(); syncItem; syncItem = s.getNextItem()) {
        int size = syncItem->getDataSize();        
        
        if( syncItem && size ) {  // only valid items
            
            char* syncData = new char[size + 1];
            memset(syncData, 0, size + 1);
            memcpy (syncData, (char*)syncItem->getData(), size);
             
            ComplexData addr( syncData );
            Target target( T("") );
            Source source(_wcc(syncItem->getKey()));
            // Build Item
            Item item(&target, &source, NULL, &addr, FALSE);
            // Add it to the list
            list.add(item);

            delete [] syncData;
        }
    }
    /*
    for(syncItem = s.getFirstItem(); syncItem; syncItem = s.getNextItem()) {
        int size = syncItem->getDataSize()/sizeof(BCHAR);
        BCHAR *syncData = (BCHAR *)syncItem->getData();

        if( syncItem && size ) {  // only valid items

            // Add the syncItem data as zero terminated string
            StringBuffer itemData(syncData, size);
            ComplexData addr( itemData.c_str() );
            Target target( T("") );
            Source source(syncItem->getKey());
            // Build Item
            Item item(&target, &source, NULL, &addr, FALSE);
            // Add it to the list
            list.add(item);
        }
    }
    */
    // If no valid items were provided by the syncsource
    // return null alert
    if(list.isEmpty())
        return NULL;

    // Ok, prepare the message
    cmdID++;

    CmdID commandID(itow(cmdID));
    int data = s.getPreferredSyncMode();

    Alert* alert = new Alert(&commandID, false, NULL, data, &list);
    
    return alert;
}

SyncHdr* SyncMLBuilder::prepareSyncHdr(Cred* cred, unsigned long maxMsgSize, unsigned long maxObjSize) {

    ++msgID;

    VerDTD*    verDTD    = new VerDTD(T("1.1"));
    VerProto*  verProto  = new VerProto(T("SyncML/1.1"));
    SessionID* sessID    = new SessionID(ltow(sessionID));
    BCHAR*   messageID = itow(msgID);
    Target*    tar       = new Target(target);
    Source*    sou       = NULL; 
    Meta* meta           = NULL;

    if (cred && bstrcmp(cred->getType(NULL), AUTH_TYPE_MD5) == 0)      
        sou = new Source(device, cred->getUsername());        
    else
        sou = new Source(device);
    

    if (maxMsgSize > 0 || maxObjSize > 0) {
        MetInf* metInf = new MetInf(NULL, NULL, NULL, NULL, 
                                    NULL, NULL, NULL, maxMsgSize,
                                    maxObjSize, NULL, NULL); 
        meta = new Meta();
        meta->setMetInf(metInf);
        deleteMetInf(&metInf);
    }
    SyncHdr* syncHdr = new SyncHdr(verDTD, verProto, sessID, messageID, 
                                   tar, sou, NULL, false, cred, meta); //respUri e Meta are null

    deleteVerDTD(&verDTD);
    deleteVerProto(&verProto);
    deleteSessionID(&sessID);
    deleteSource(&sou);
    deleteTarget(&tar);    
    safeDel(&messageID);    

    return syncHdr;    
}

SyncML* SyncMLBuilder::prepareInitObject(Cred* cred, ArrayList* alerts, ArrayList* commands, unsigned long maxMsgSize, unsigned long maxObjSize) {

    SyncHdr* syncHdr     = prepareSyncHdr(cred, maxMsgSize, maxObjSize);
    SyncML*  syncml      = NULL;               
    ArrayList* list      = new ArrayList();
    SyncBody* syncBody   = NULL;

    if (!commands->isEmpty()) {
        list = commands->clone();
    }    
    if (alerts && alerts->size() > 0) {
        for (int k = 0; k < alerts->size(); k++)
            list->add(*(Alert*)alerts->get(k));        
    }
        
    
    syncBody   = new SyncBody(list, TRUE);
    deleteArrayList(&list);
    syncml       = new SyncML(syncHdr, syncBody);

    deleteSyncHdr(&syncHdr);
    deleteSyncBody(&syncBody);   
    
    return syncml;
}

BCHAR* SyncMLBuilder::prepareMsg(SyncML* syncml) {        
    StringBuffer *s = Formatter::getSyncML(syncml);
    BCHAR* str = stringdup(s->c_str());
    delete s;
    return str;
}


SyncML* SyncMLBuilder::prepareSyncML(ArrayList* commands, BOOL final) {    
    
    SyncHdr* syncHdr = prepareSyncHdr(NULL);       
    SyncBody* syncBody   = new SyncBody(commands, final);       
    SyncML* syncml = new SyncML(syncHdr, syncBody);
    deleteSyncHdr(&syncHdr);    
    deleteSyncBody(&syncBody);

    return syncml;
}

ComplexData* SyncMLBuilder::getComplexData(SyncItem* syncItem,
                                           long &syncItemOffset, long maxBytes, long &sentBytes) {
    
        BCHAR* t   = NULL;
        TransformationInfo info;
        ComplexData* data = NULL;

        info.size = syncItem->getDataSize() - syncItemOffset;
        if (info.size && info.size > maxBytes) {
            info.size = maxBytes;
            if (info.size < 1) {
                // ensure that at least one byte is sent even if maxBytes is 0 or negative,
                // otherwise no progress is made
                info.size = 1;
            }
        }
        sentBytes = info.size;
        info.password = encPassword;
        if (encoding == DESB64) {
            char* tt = new char[info.size + 1];
            memset(tt, 0, info.size + 1);            
            memcpy(tt, (char *)syncItem->getData() + syncItemOffset, sentBytes);                        
            t = encodeDESB64(tt, info);
            delete [] tt;
        } else if (encoding == B64) {
            char* tt = new char[info.size + 1];
            memset(tt, 0, info.size + 1);            
            memcpy(tt, (char *)syncItem->getData() + syncItemOffset, sentBytes);            
            t = encodeB64(tt, info);
            delete [] tt;
        } else {
            t = new BCHAR[info.size + 1];
            memset(t, 0, info.size + 1);            
            memcpy(t, (char *)syncItem->getData() + syncItemOffset, sentBytes);
        }
        syncItemOffset += sentBytes;
                           
        data = new ComplexData(t);
        
        if (t)
            {delete [] t; t = NULL; }
        
        return data;
}


ArrayList* SyncMLBuilder::prepareItem(SyncItem* syncItem,
                                      long &syncItemOffset, long maxBytes, long &sentBytes,
                                      const BCHAR* type, BCHAR* COMMAND) {
    ArrayList* list = new ArrayList();            
    
    Source* sou = new Source(_wcc(syncItem->getKey()));
    ComplexData* data = NULL;
    Meta m;
    BOOL hasMoreData = FALSE;
    BOOL isFirstChunk = !syncItemOffset;
    if (bstrcmp(DELETE_COMMAND_NAME, COMMAND) != 0) {       
        if (encoding == DESB64) {
            m.setFormat(T("des;b64"));         
        } else if (encoding == B64) {
            m.setFormat(T("b64"));            
        } 
        data = getComplexData(syncItem, syncItemOffset, maxBytes, sentBytes);
        hasMoreData = syncItemOffset < syncItem->getDataSize();
        if (isFirstChunk && hasMoreData) {
            // must send size, but only in first chunk of this item
            m.setSize(syncItem->getDataSize());
        }
    } else {
        // skip all item data for deleted items
        syncItemOffset = syncItem->getDataSize();
        sentBytes = 0;
    }
    
    BCHAR *tparent = toMultibyte(syncItem->getTargetParent());
    BCHAR *sparent = toMultibyte(syncItem->getSourceParent());

    Item* item = new Item(NULL, sou, tparent, sparent, &m, data, hasMoreData);
    list->add(*item);

    delete [] tparent;
    delete [] sparent;

    deleteSource(&sou);
    deleteComplexData(&data);
    deleteItem(&item);

    return list;   
}

long SyncMLBuilder::addItem(ModificationCommand* &modificationCommand,
                            long &syncItemOffset, long maxBytes,
                            BCHAR* COMMAND, SyncItem* syncItem, const BCHAR* defaultType) {           
    if (syncItem == NULL) {
         return 0;
    }

    // The item should determine its type itself.
    // Only fallback to the default type configured for its
    // source if (broken?) SyncSources do not set a in their
    // items.
    const BCHAR *type = _wcc(syncItem->getDataType());
    if (!type || !type[0]) {
        type = defaultType;
    }

    if (!modificationCommand) {
        ++cmdID;
        CmdID commandID(itow(cmdID));
        ModificationCommand* ret = NULL;
        MetInf metInf(NULL, (BCHAR*)type, NULL, NULL, 
                      NULL, NULL, NULL, NULL, NULL, NULL, NULL); 
        Meta meta;
        
        meta.setMetInf(&metInf);
    
        if (bstrcmp(ADD_COMMAND_NAME, COMMAND) == 0)
            modificationCommand = new Add(&commandID, FALSE, NULL, &meta, NULL);
        else if (bstrcmp(REPLACE_COMMAND_NAME, COMMAND) == 0){
            modificationCommand = new Replace(&commandID, FALSE, NULL, &meta, NULL);
        } else if (bstrcmp(DELETE_COMMAND_NAME, COMMAND) == 0) {
            modificationCommand = new Delete(&commandID, FALSE, FALSE, FALSE, NULL, &meta, NULL);
        }
    }

    ArrayList* list = modificationCommand->getItems();
    // assert(syncItemOffset >= 0);
    // assert(syncItemOffset <= syncItem->getDataSize();
    long sentBytes = 0;
    ArrayList* tmpList = prepareItem(syncItem, syncItemOffset, maxBytes, sentBytes, type, COMMAND);
    // assert(syncItemOffset >= 0);
    // assert(syncItemOffset <= syncItem->getDataSize();
    list->add(tmpList);
    deleteArrayList(&tmpList);

    return sentBytes;
}

Sync* SyncMLBuilder::prepareSyncCommand(SyncSource& source) {            

    ++cmdID;

    CmdID* commandID     = new CmdID(itow(cmdID));
    Target* tar          = new Target(source.getConfig().getURI());
    Source* sou          = new Source(_wcc(source.getName())); 
    ArrayList* list      = new ArrayList();  
    Sync* sync           = NULL;
         
    sync = new Sync(commandID, FALSE, NULL, tar, sou, NULL, 0,  list);

    deleteCmdID(&commandID);
    deleteTarget(&tar);
    deleteSource(&sou);
    deleteArrayList(&list);

    return sync;  
    
}


Map* SyncMLBuilder::prepareMapCommand(SyncSource& s) {
    /*
    CmdID* cmdID,
    Target* target,
    Source* source,
    Cred* cred,
    Meta* meta,
    ArrayList* mapItems);
    */
    ++cmdID;
    CmdID* commandID     = new CmdID(itow(cmdID));    
    Target* tar          = new Target(s.getConfig().getURI());
    Source* sou          = new Source(_wcc(s.getName()));
    ArrayList* mapItems  = new ArrayList();
    Map* map = new Map(commandID, tar, sou, NULL, NULL, mapItems);

    deleteCmdID(&commandID);
    deleteTarget(&tar);
    deleteSource(&sou);
    deleteArrayList(&mapItems);

    return map;

}

MapItem* SyncMLBuilder::prepareMapItem(SyncMap* syncMap) {
      
    Target* tar          = new Target(syncMap->getGUID());
    Source* sou          = new Source(syncMap->getLUID());    
    MapItem* m = new MapItem(tar, sou);
    deleteTarget(&tar);
    deleteSource(&sou);
    
    return m;
}

void SyncMLBuilder::addMapItem(Map* map, MapItem* mapItem){
    if (mapItem == NULL || map == NULL)
        return;
    ArrayList* list = map->getMapItems();
    list->add(*mapItem);

}

void SyncMLBuilder::resetMessageID() {
    msgID = 0;
}

void SyncMLBuilder::setEncPassword(const BCHAR* pwd) {
    if (encPassword) {
        safeDelete(&encPassword);
    }

    if (pwd) {
        encPassword = stringdup(pwd);
    }
}

BCHAR* SyncMLBuilder::encodeDESB64(char* data, TransformationInfo& info) {
    char*    des = NULL;
    char*    b64 = NULL;

    DataTransformer* dtdes = DataTransformerFactory::getEncoder(T("des"));
    DataTransformer* dtb64 = DataTransformerFactory::getEncoder(T("b64"));

    if ((dtdes == NULL) || (dtb64 == NULL)) {
        goto exit;
    }

    des = dtdes->transform(data, info);

    if (lastErrorCode != ERR_NONE) {
        goto exit;
    }

    b64 = dtb64->transform(des, info);

    if (lastErrorCode != ERR_NONE) {
        goto exit;
    }

    ++info.size;
    // info.size = (bstrlen(ret)+1)*sizeof(BCHAR);


exit:

    if (des) {
        delete [] des; des = NULL;
    }

    if (dtdes) {
        delete dtdes;
    }

    if (dtb64) {
        delete dtb64;
    }

    return b64;
}

BCHAR* SyncMLBuilder::encodeB64(char* data, TransformationInfo& info) {
    char*    b64 = NULL;
    DataTransformer* dtb64 = DataTransformerFactory::getEncoder(T("b64"));

    if (dtb64 == NULL) {
        goto exit;
    }

    b64 = dtb64->transform((char*)data, info);

    ++info.size;
    // info.size = (bstrlen(ret)+1)*sizeof(BCHAR);

exit:
    if (dtb64) {
        delete dtb64;
    }

    return b64;
}

/*
ComplexData* SyncMLBuilder::getComplexData(SyncItem* syncItem) {
    
        BCHAR* t   = NULL;
        char*    tmp = NULL;
        TransformationInfo info;
        ComplexData* data = NULL;

        info.size = syncItem->getDataSize() / sizeof(BCHAR);
        info.password = encPassword;
        if (encoding == DESB64) {
            BCHAR* tt = new BCHAR[info.size + 1];
            wmemset(tt, 0, info.size + 1);            
            memcpy(tt, syncItem->getData(), syncItem->getDataSize());            
            tmp = wc2utf8(tt);            
            t = encodeDESB64(tmp, info);
            delete [] tt;
        } else if (encoding == B64) {
            BCHAR* tt = new BCHAR[info.size + 1];
            wmemset(tt, 0, info.size + 1);            
            memcpy(tt, syncItem->getData(), syncItem->getDataSize());
            tmp = wc2utf8(tt);                                          
            t = encodeB64(tmp, info);
            delete [] tt;
        } else {
            t = new BCHAR[info.size + 1];
            wmemset(t, 0, info.size + 1);            
            memcpy(t, syncItem->getData(), syncItem->getDataSize());
        }
                           
        data = new ComplexData(t);
        
        if (t) {delete [] t; t = NULL; }
        if (tmp) {delete [] tmp; tmp = NULL; }

        return data;

}
*/
