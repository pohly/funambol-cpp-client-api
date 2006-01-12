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
 
 
#include "syncml/parser/Parser.h"
 
SyncML* Parser::getSyncML(wchar_t* xml) {
    SyncBody* syncBody = NULL;
    SyncHdr*  syncHdr  = NULL;
    SyncML*   syncML   = NULL; 
    unsigned int pos = 0;

    syncHdr  = getSyncHdr (XMLProcessor::getElementContent(xml, SYNC_HDR, &pos));
    syncBody = getSyncBody(XMLProcessor::getElementContent(xml, SYNC_BODY, &pos));
    
    syncML = new SyncML(syncHdr, syncBody);
    
    deleteSyncHdr (&syncHdr);
    deleteSyncBody(&syncBody);
    
    return syncML;
     
}

SyncHdr* Parser::getSyncHdr(wchar_t* xml) {
         
    SessionID*   sessionID = NULL;
    VerDTD*      verDTD    = NULL;
    VerProto*    verProto  = NULL;
    Source*      source    = NULL;
    Target*      target    = NULL;
    Cred*        cred      = NULL;
    wchar_t*     respURI   = NULL;
    wchar_t*     msgID     = NULL;
    BOOL         noResp    = NULL;             
    wchar_t*     tmp       = NULL;
    Meta*        meta      = NULL;

    SyncHdr*     ret       = NULL;
    
    sessionID = getSessionID(XMLProcessor::getElementContent (xml, SESSION_ID, NULL));
    verDTD    = getVerDTD   (XMLProcessor::getElementContent (xml, VER_DTD   , NULL));
    verProto  = getVerProto (XMLProcessor::getElementContent (xml, VER_PROTO , NULL));
    source    = getSource   (XMLProcessor::getElementContent (xml, SOURCE    , NULL));
    target    = getTarget   (XMLProcessor::getElementContent (xml, TARGET    , NULL));    
    cred      = getCred     (XMLProcessor::getElementContent (xml, CRED      , NULL));
    msgID     = XMLProcessor::getElementContent              (xml, MSG_ID    , NULL);
    respURI   = XMLProcessor::getElementContent              (xml, RESP_URI  , NULL);
    meta      = getMeta(XMLProcessor::getElementContentLevel(xml, META, NULL));
    tmp       = XMLProcessor::getElementContent        (xml, NO_RESP, NULL);

    if (tmp) {
        wcscmpIgnoreCase(tmp, TEXT("TRUE")) ? noResp = TRUE : noResp = FALSE;            
    } 
       
    ret = new SyncHdr(verDTD, verProto, sessionID, msgID, target, source, respURI, noResp, cred, meta);                
        
    deleteVerDTD(&verDTD);
    deleteVerProto(&verProto);
    deleteSessionID(&sessionID);
    deleteSource(&source   );
    deleteTarget(&target);
    deleteCred(&cred);
    deleteMeta(&meta);
    safeDel(&respURI);
    safeDel(&msgID);
    safeDel(&tmp);
      
    return ret;    
}

Cred* Parser::getCred(wchar_t* xml) {
    Cred* ret              = NULL;
    Authentication* auth   = NULL;    
    
    auth = getAuthentication(xml);
    if (auth) { 
        ret = new Cred(auth); 
    }
    
    deleteAuthentication(&auth);

    return ret;
}

Authentication* Parser::getAuthentication(wchar_t* xml) {
    Authentication* ret        = NULL;
    
    wchar_t*        data       = NULL;    
    Meta*           meta       = NULL;
    
    data           = XMLProcessor::getElementContent              (xml, DATA   , NULL);
    meta           = getMeta(XMLProcessor::getElementContentLevel      (xml, META   , NULL));
    
    if (data || meta) {    
        ret = new Authentication(meta, data);
    }
    safeDel(&data);
    deleteMeta(&meta);
    
    return ret;
}

Meta* Parser::getMeta(wchar_t* xml) {
    Meta* ret        = NULL;
    MetInf* metInf   = NULL;    
    
    metInf = getMetInf(xml);            
    if (metInf) {
        ret = new Meta();
        ret->setMetInf(metInf);  
    }
    
    deleteMetInf(&metInf);    

    return ret;
}

MetInf* Parser::getMetInf(wchar_t* xml) {
    MetInf* ret             = NULL;
    
    wchar_t*     format     = NULL;
    wchar_t*     type       = NULL;
    wchar_t*     mark       = NULL;
    Anchor*      anchor     = NULL;
    wchar_t*     version    = NULL;
    NextNonce*   nextNonce  = NULL;
    long         maxMsgSize = 0;
    long         maxObjSize = 0;
    long         size       = 0;
    ArrayList*   emi        = NULL;
    Mem*         mem        = NULL; 
    
    wchar_t*         maxMsgSizeW   = NULL;
    wchar_t*         maxObjSizeW   = NULL;
    wchar_t*         sizeW         = NULL;    
    
    // get all the values

    format       = XMLProcessor::getElementContent              (xml, FORMAT   , NULL);
    type         = XMLProcessor::getElementContent              (xml, TYPE     , NULL);
    mark         = XMLProcessor::getElementContent              (xml, MARK     , NULL);
    
    anchor       = getAnchor(xml);
    version      = XMLProcessor::getElementContent              (xml, VERSIONSTR       , NULL);
    nextNonce    = getNextNonce(xml);
    
    maxMsgSizeW  = XMLProcessor::getElementContent              (xml, MAX_MESSAGE_SIZE     , NULL);
    maxObjSizeW  = XMLProcessor::getElementContent              (xml, MAX_OBJ_SIZE     , NULL);
    sizeW        = XMLProcessor::getElementContent              (xml, SIZE             , NULL);
        
    if (maxMsgSizeW) {
        maxMsgSize = wcstol(maxMsgSizeW, NULL, 10);
    }
    if (maxObjSizeW) {
        maxObjSize = wcstol(maxObjSizeW, NULL, 10);
    }
    if (sizeW) {
        size = wcstol(sizeW, NULL, 10);
    }
        
    emi          = getEMI(xml);
    mem          = getMem(xml);    
    
    // check if someting is null, 0 or zero lenght
    BOOL isToCreate = FALSE;
    isToCreate = NotNullCheck(7, format, type, mark, version, maxMsgSizeW, maxObjSizeW, sizeW)
                 || NotZeroArrayLenght(1, emi)
                 || (mem)
                 || (anchor) 
                 || (nextNonce);
                
    if (isToCreate) {
        ret = new MetInf(format, type, mark, size, anchor, version, nextNonce, maxMsgSize,
                         maxObjSize, emi, mem);
    }
    deleteAll(7, &format, &type, &mark, &version, &maxMsgSizeW, &maxObjSizeW, &sizeW);
       
    deleteAnchor(&anchor);
    deleteNextNonce(&nextNonce);
    deleteArrayList(&emi);    
    deleteMem(&mem);
    
    return ret;    
}


ArrayList* Parser::getSources(wchar_t* xml) {
    
    Source* source = NULL; 
    SourceArray* sourceArray = NULL;
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();

    while ((source = getSource(XMLProcessor::getElementContent(&xml[pos], SOURCE, &pos))) != NULL) {
        if (source) {
            sourceArray = new SourceArray(source);
            list->add(*sourceArray); // in the ArrayList NULL element cannot be inserted
            deleteSource(&source);            
            deleteSourceArray(&sourceArray);            
        }
        pos += previous;
        previous = pos;                
    }
    return list;    
}


Source* Parser::getSource(wchar_t* xml) {
    Source* ret      = NULL;
    wchar_t* locURI  = NULL;
    wchar_t* locName = NULL;
    locURI  = XMLProcessor::getElementContent (xml, LOC_URI, NULL);
    locName = XMLProcessor::getElementContent (xml, LOC_NAME, NULL);
    
    if (NotNullCheck(2, locURI, locName)) {
        ret = new Source(locURI, locName);
    }
    
    safeDel(&locURI);
    safeDel(&locName);
    
    return ret;
}

Target* Parser::getTarget(wchar_t* xml) {
    Target*  ret      = NULL;
    wchar_t* locURI   = NULL;
    wchar_t* locName  = NULL;
    
    locURI  = XMLProcessor::getElementContent (xml, LOC_URI, NULL);
    locName = XMLProcessor::getElementContent (xml, LOC_NAME, NULL);
    
    if (NotNullCheck(2, locURI, locName)) {
        ret = new Target(locURI, locName);
    }
    
    safeDel(&locURI);
    safeDel(&locName);

    return ret;
}

Anchor* Parser::getAnchor(wchar_t* xml) {
    Anchor*  ret    = NULL;
    wchar_t* last   = NULL;
    wchar_t* next   = NULL;
    
    last  = XMLProcessor::getElementContent (xml, LAST, NULL);
    next  = XMLProcessor::getElementContent (xml, NEXT, NULL);
    
    if (NotNullCheck(2, last, next)) {
        ret = new Anchor(last, next);
    }
    
    safeDel(&next);
    safeDel(&last);

    return ret;   
}

NextNonce* Parser::getNextNonce(wchar_t* xml) {
    NextNonce* ret   = NULL;
    wchar_t* value   = NULL;   
    
    value  = XMLProcessor::getElementContent (xml, NEXT_NONCE, NULL);

    if (NotNullCheck(1, value)) {

        ret = new NextNonce(value);        
    }
    
    safeDel(&value);
    return ret;   
}

Mem* Parser::getMem(wchar_t* xml) {
    Mem* ret            = NULL;
    wchar_t* freeMemW   = NULL;   
    wchar_t* sharedMemW = NULL;
    wchar_t* freeIDW    = NULL;
        
    BOOL    sharedMem   = NULL;
    long    freeMem     = 0;
    long    freeID      = 0;
    
    BOOL isToCreate = FALSE;

    freeMemW    = XMLProcessor::getElementContent (xml, FREE_MEM,   NULL);
    sharedMemW  = XMLProcessor::getElementContent (xml, SHARED_MEM, NULL);
    freeIDW     = XMLProcessor::getElementContent (xml, FREE_ID,    NULL);
    
    isToCreate = NotNullCheck(3, freeMemW, sharedMemW, freeIDW);

    if (freeMemW != NULL) {
        freeMem = wcstol(freeMemW, NULL, 10);
    }
    if (freeIDW != NULL) {
        freeID = wcstol(freeIDW, NULL, 10);
    } 
    if (sharedMemW != NULL) {
        sharedMem = wcstol(sharedMemW, NULL, 10);
    }
    
    if (isToCreate) {
        ret = new Mem(sharedMem, freeMem, freeID);
    }

    safeDel(&freeMemW);
    safeDel(&freeIDW);
    safeDel(&sharedMemW);

    return ret;   
}


SessionID* Parser::getSessionID(wchar_t* content) {
    SessionID* ret = NULL;
    if (content) {
        ret = new SessionID(content);
    }
    return ret;
}

VerDTD* Parser::getVerDTD(wchar_t* content) {
    VerDTD* ret = NULL;
    if (content) {
        ret = new VerDTD(content);
    }
    return ret;
}

VerProto* Parser::getVerProto(wchar_t* content) {
    VerProto* ret = NULL;
    if (content) {
        ret = new VerProto(content);
    }
    return ret;
}

SyncBody* Parser::getSyncBody(wchar_t* xml) {
    
    SyncBody* syncBody   = NULL;
    BOOL finalMsg        = FALSE;
    ArrayList* commands;
    
    commands = getCommands(xml);
    finalMsg = getFinalMsg(XMLProcessor::getElementContent(xml, FINAL_MSG, NULL));
    syncBody = new SyncBody(commands, finalMsg);
    
    deleteArrayList(&commands);    

    return syncBody;
}

/*
* The sequence tag can contains the common commands (Add, Replace, Delete, Copy) and
* Alert
* Exec
* Get
* Map
*
* Atomic
* Sync
*/
Sequence* Parser::getSequence(wchar_t* xml) {    

    Sequence* ret           = NULL;
        
    Meta*   meta            = NULL;
    BOOL    noResp          = NULL;
    CmdID*  cmdID           = NULL;
    ArrayList* commands     = new ArrayList(); 
    Sync* sync              = NULL;
    Atomic* atomic          = NULL;
    
    Alert* alert            = NULL;
    Map*   map              = NULL;
    Get*   get              = NULL;
    Exec* exec              = NULL;

    ArrayList* list     = new ArrayList(); 
    unsigned int pos = 0, previous = 0;

    cmdID    = getCmdID      (XMLProcessor::getElementContent(xml, CMD_ID,         NULL));
    meta     = getMeta       (XMLProcessor::getElementContentLevel (xml,  META  , NULL));
    noResp   = getNoResp     (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    // list of commands that must not be leaf of Sync and Atomic
    commands = getCommonCommandList(xml, TEXT("Atomic&Sync"));    

    // Alert
    pos = 0, previous = 0;
    while ((alert = getAlert(XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos))) != NULL) {
        if (alert) {
            list->add(*alert); // in the ArrayList NULL element cannot be inserted
            deleteAlert(&alert);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Map
    pos = 0, previous = 0;
    while ((map = getMap(XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos))) != NULL) {
        if (map) {
            list->add(*map); // in the ArrayList NULL element cannot be inserted
            deleteMap(&map);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        

    deleteArrayList(&list);
    
    // Get
    pos = 0, previous = 0;
    while ((get = getGet(XMLProcessor::getElementContentLevel(&xml[pos], GET, &pos))) != NULL) {
        if (get) {
            list->add(*get); // in the ArrayList NULL element cannot be inserted
            deleteGet(&get);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);   
   
    // Exec
    pos = 0, previous = 0;
    while ((exec = getExec(XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos))) != NULL) {
        if (exec) {
            list->add(*exec); // in the ArrayList NULL element cannot be inserted
            deleteExec(&exec);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    wchar_t* element = XMLProcessor::getElementContentLevel (xml,  SYNC, NULL);

    if (element) {       
        sync = getSync(element);
        if (sync) {
            commands->add(*sync);
            deleteSync(&sync);
        }
        safeDel(&element);
    } 

    element = XMLProcessor::getElementContentLevel (xml,  ATOMIC, NULL);

    if (element) {    
        atomic = getAtomic(element);
        if (atomic) {
            commands->add(*atomic);
            deleteAtomic(&atomic);
        }
        safeDel(&element);
    } 


    if ((cmdID)   || 
        (meta)    ||
        NotZeroArrayLenght(1, commands)) {
        
        ret = new Sequence(cmdID, noResp, meta, commands);
    }

    deleteMeta(&meta);    
    deleteCmdID(&cmdID);
    deleteArrayList(&commands);

    return ret;
}

/*
* The Atomic tag can contains the common commands (Add, Replace, Delete, Copy) and
* Alert
* Exec
* Get
* Map
*
* Atomic
* Sync
* Sequence
*/
Atomic* Parser::getAtomic(wchar_t* xml) {    

    Atomic* ret             = NULL;

    Meta*   meta            = NULL;
    BOOL    noResp          = NULL;
    CmdID*  cmdID           = NULL;
    ArrayList* commands     = new ArrayList(); 
    Sync* sync              = NULL;
    Sequence* sequence      = NULL;
    wchar_t* element = NULL;
    
    Alert* alert            = NULL;
    Map*   map              = NULL;
    Get*   get              = NULL;
    Exec* exec              = NULL;

    ArrayList* list     = new ArrayList(); 
    unsigned int pos = 0, previous = 0;

    cmdID    = getCmdID      (XMLProcessor::getElementContent(xml, CMD_ID,         NULL));
    meta     = getMeta       (XMLProcessor::getElementContentLevel (xml,  META  , NULL));
    noResp   = getNoResp    (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    // list of commands that must not be leaf of Sync and Atomic
    commands = getCommonCommandList(xml, TEXT("Sync&Sequence"));
    
    // Alert
    pos = 0, previous = 0;
    while ((alert = getAlert(XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos))) != NULL) {
        if (alert) {
            list->add(*alert); // in the ArrayList NULL element cannot be inserted
            deleteAlert(&alert);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Map
    pos = 0, previous = 0;
    while ((map = getMap(XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos))) != NULL) {
        if (map) {
            list->add(*map); // in the ArrayList NULL element cannot be inserted
            deleteMap(&map);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        

    deleteArrayList(&list);
    
    // Get
    pos = 0, previous = 0;
    while ((get = getGet(XMLProcessor::getElementContentLevel(&xml[pos], GET, &pos))) != NULL) {
        if (get) {
            list->add(*get); // in the ArrayList NULL element cannot be inserted
            deleteGet(&get);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);   
   
    // Exec
    pos = 0, previous = 0;
    while ((exec = getExec(XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos))) != NULL) {
        if (exec) {
            list->add(*exec); // in the ArrayList NULL element cannot be inserted
            deleteExec(&exec);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    element = XMLProcessor::getElementContentLevel (xml,  SYNC , NULL);
    
    if (element) {
        sync = getSync(element);
        if (sync) {
            commands->add(*sync);
            deleteSync(&sync);
        }
        safeDel(&element);
    }
    
    element = XMLProcessor::getElementContentLevel (xml,  SEQUENCE, NULL);    
    
    if (element) {
        sequence = getSequence(element);
        if (sequence) {
            commands->add(*sequence);
            deleteSequence(&sequence);
        }
        safeDel(&element);
    }    

    //
    // TBD: Atomic
    //


    if ((cmdID)   || 
        (meta)    ||
        NotZeroArrayLenght(1, commands)) {
        
        ret = new Atomic(cmdID, noResp, meta, commands);
    }

    deleteMeta(&meta);    
    deleteCmdID(&cmdID);
    deleteArrayList(&commands);

    return ret;
}

/*
* Contains the commands that the <sync> tag can have.
*    Add    
*    Replace
*    Delete 
*    Copy   
*    Atomic 
*    Map    
*    Sync   
*/

Sync* Parser::getSync(wchar_t* xml) {    

    Sync* ret               = NULL;
    Sequence* sequence      = NULL;
    Atomic* atomic          = NULL;
    Cred*   cred            = NULL;
    Meta*   meta            = NULL;
    BOOL    noResp          = NULL;
    CmdID*  cmdID           = NULL;
    Target* target          = NULL;
    Source* source          = NULL;
    ArrayList* commands     = new ArrayList();   
    long numberOfChanges    = 0;
    wchar_t* numberOfChangesW = NULL;

    cmdID    = getCmdID      (XMLProcessor::getElementContent(xml, CMD_ID,  NULL));
    target   = getTarget     (XMLProcessor::getElementContent(xml, TARGET,  NULL));
    source   = getSource     (XMLProcessor::getElementContent(xml, SOURCE,  NULL));
    meta     = getMeta       (XMLProcessor::getElementContentLevel (xml,  META  ,NULL));
    numberOfChangesW = XMLProcessor::getElementContent (xml,  NUMBER_OF_CHANGES ,NULL);
    if (numberOfChangesW) {
        numberOfChanges = wcstol(numberOfChangesW, NULL, 10);
    }

    cred     = getCred       (XMLProcessor::getElementContent (xml, CRED   ,NULL));
    noResp   = getNoResp    (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    commands = getCommonCommandList(xml, TEXT("Atomic&Sequence"));
    
    wchar_t* element = XMLProcessor::getElementContentExcept(xml,  SEQUENCE, TEXT("Atomic"), NULL);
    
    if (element) {
        sequence = getSequence(element);
        if (sequence) {
            commands->add(*sequence);
            deleteSequence(&sequence);
        }
        safeDel(&element);
    }

    element = XMLProcessor::getElementContentExcept(xml,  ATOMIC, TEXT("Atomic&Sequence"), NULL);
    
    if (element) {
        atomic = getAtomic(element);
        if (atomic) {
            commands->add(*atomic);
            deleteAtomic(&atomic);
        }
        safeDel(&element);
    }
    
    if ((cmdID)   || 
        (cred)    || 
        (target)  ||
        (source)  ||
        (meta)    ||
        NotZeroArrayLenght(1, commands)) {
        
        ret = new Sync(cmdID, noResp, cred, target, source, meta, numberOfChanges, commands);
    }

    deleteCred(&cred);
    deleteMeta(&meta);    
    deleteCmdID(&cmdID);
    deleteTarget(&target);
    deleteSource(&source);
    deleteArrayList(&commands);

    return ret;
}

ArrayList* Parser::getCommonCommandList(wchar_t* xml, wchar_t* except) {

    ArrayList* commands = new ArrayList();
    ArrayList* list = new ArrayList();

    //
    //Add
    //    
    list = getAdds(xml, except);
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);    
    
    //
    //Replace
    //    
    list = getReplaces(xml, except);
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);  
    
    //
    //Delete
    //    
    list = getDels(xml, except);   
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);  

    //
    //Copy
    //    
    list = getCopies(xml, except);   
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);  

    return commands;
}

Copy* Parser::getCopy(wchar_t* xml) {
    Copy* ret = NULL;
        
    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;

    cmdID   = getCmdID     (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    meta    = getMeta      (XMLProcessor::getElementContentLevel (xml, META,    NULL));
    cred    = getCred      (XMLProcessor::getElementContent (xml, CRED   , NULL));
    noResp  = getNoResp    (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    items = getItems(xml);
    
    if ((cmdID) || 
        (cred)  || 
        NotZeroArrayLenght(1, items)                                        
        )  {

        ret = new Copy(cmdID, noResp, cred, meta, items);
    }
    
    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);
    deleteArrayList(&items);    

    return ret;
}


Add* Parser::getAdd(wchar_t* xml) {
    Add* ret = NULL;

    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;

    cmdID   = getCmdID     (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    meta    = getMeta      (XMLProcessor::getElementContentLevel(xml,  META,    NULL));
    cred    = getCred      (XMLProcessor::getElementContent (xml, CRED   , NULL));
    noResp  = getNoResp    (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    items = getItems(xml);
    
    if ((cmdID) || 
        (cred)  || 
        NotZeroArrayLenght(1, items)                                        
        )  {

        ret = new Add(cmdID, noResp, cred, meta, items);
    }
    
    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);
    deleteArrayList(&items);    

    return ret;
}

Delete* Parser::getDelete(wchar_t* xml) {
    Delete* ret = NULL;
       
    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    BOOL        archive = NULL;
    BOOL        sftDel  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;

    cmdID   = getCmdID     (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    meta    = getMeta      (XMLProcessor::getElementContentLevel (xml,  META  , NULL));
    cred    = getCred      (XMLProcessor::getElementContent (xml, CRED   , NULL));
    noResp  = getNoResp    (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    items = getItems(xml);
    
    if ((cmdID) || 
        (cred)  || 
        NotZeroArrayLenght(1, items)                                        
        )  {

        ret = new Delete(cmdID, noResp, archive, sftDel, cred, meta, items);
    }
        
    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);
    deleteArrayList(&items);    

    return ret;
}

Replace* Parser::getReplace(wchar_t* xml) {
    Replace* ret = NULL;

    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;

    cmdID   = getCmdID     (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    meta    = getMeta      (XMLProcessor::getElementContentLevel(xml,  META,    NULL));
    cred    = getCred      (XMLProcessor::getElementContent (xml, CRED   , NULL));
    noResp  = getNoResp    (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    items = getItems(xml);
    
    if ((cmdID) || 
        (cred)  || 
        NotZeroArrayLenght(1, items)                                        
        )  {

        ret = new Replace(cmdID, noResp, cred, meta, items);
    }
        
    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);
    deleteArrayList(&items);    

    return ret;
}

MapItem* Parser::getMapItem(wchar_t* xml) {
    MapItem* ret = NULL;
    
    Target*    target = NULL;
    Source*    source = NULL;
    
    target   = getTarget     (XMLProcessor::getElementContent(xml, TARGET,         NULL));
    source   = getSource     (XMLProcessor::getElementContent(xml, SOURCE,         NULL));
    
    if ((target)|| 
        (source)) {

        ret = new MapItem(target, source);
    }
    
    deleteTarget(&target);
    deleteSource(&source);
    
    return ret;
}

/*
* Returns an ArrayList of mapItem command
*/
ArrayList* Parser::getMapItems(wchar_t* xml) {
    
    MapItem* mapItem = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();

    while ((mapItem = getMapItem(XMLProcessor::getElementContent(&xml[pos], MAP_ITEM, &pos))) != NULL) {
        if (mapItem) {
            list->add(*mapItem); // in the ArrayList NULL element cannot be inserted
            deleteMapItem(&mapItem);            
        }
        pos += previous;
        previous = pos;                
    }
    return list;   
}

Map* Parser::getMap(wchar_t* xml) {
    Map* ret = NULL;

    CmdID*     cmdID  = NULL;
    Cred*      cred   = NULL;
    Meta*      meta   = NULL;
    
    Target*    target = NULL;
    Source*    source = NULL;
    ArrayList* mapItems; //MapItem[]
    
    cmdID    = getCmdID      (XMLProcessor::getElementContent(xml, CMD_ID,  NULL));
    meta     = getMeta       (XMLProcessor::getElementContentLevel(xml, META,    NULL));
    cred     = getCred       (XMLProcessor::getElementContent(xml, CRED,    NULL));    
    target   = getTarget     (XMLProcessor::getElementContent(xml, TARGET,  NULL));
    source   = getSource     (XMLProcessor::getElementContent(xml, SOURCE,  NULL));
    mapItems = getMapItems(xml);
    
    if ((cmdID) ||
        (meta)  ||
        (cred)  ||
        (target)|| 
        (source)|| 
        NotZeroArrayLenght(1, mapItems)                                        
        )  {

        ret = new Map(cmdID, target, source, cred, meta, mapItems);
    }
    
    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    deleteCred(&cred);
    deleteTarget(&target);
    deleteSource(&source);
    deleteArrayList(&mapItems);    

    return ret;
}


/*
* Returns an ArrayList of copy command
*/
ArrayList* Parser::getCopies(wchar_t* xml, wchar_t* except) {
    
    Copy* copy = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();
    
   /*
    * except is set to SYNC if we are looking for Copy commands external from <sync> tag
    */
    while ((copy = getCopy(XMLProcessor::getElementContentExcept(&xml[pos], COPY, except, &pos))) != NULL) {
        if (copy) {
            list->add(*copy); // in the ArrayList NULL element cannot be inserted
            deleteCopy(&copy);            
        }
        pos += previous;
        previous = pos;                
    }   
    return list;   
}

/*
* Returns an ArrayList of add command
*/
ArrayList* Parser::getAdds(wchar_t* xml, wchar_t* except) {
    
    Add* add = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();
    
   /*
    * except is set to SYNC if we are looking for Add commands external from <sync> tag
    */
    while ((add = getAdd(XMLProcessor::getElementContentExcept(&xml[pos], ADD, except, &pos))) != NULL) {
        if (add) {
            list->add(*add); // in the ArrayList NULL element cannot be inserted
            deleteAdd(&add);            
        }
        pos += previous;
        previous = pos;                
    }   
    return list;   
}

/*
* Returns an ArrayList of Replace commands
*/
ArrayList* Parser::getReplaces(wchar_t* xml, wchar_t* except) {
    
    Replace* replace = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();

    while ((replace = getReplace(XMLProcessor::getElementContentExcept(&xml[pos], REPLACE, except, &pos))) != NULL) {
        if (replace) {
            list->add(*replace); // in the ArrayList NULL element cannot be inserted
            deleteReplace(&replace);            
        }
        pos += previous;
        previous = pos;                
    }
    return list;   
}

/*
* Returns an ArrayList of Dels command
*/
ArrayList* Parser::getDels(wchar_t* xml, wchar_t* except) {
    
    Delete* del = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();

    while ((del = getDelete(XMLProcessor::getElementContentExcept(&xml[pos], DEL, except, &pos))) != NULL) {
        if (del) {
            list->add(*del); // in the ArrayList NULL element cannot be inserted
            deleteDelete(&del);            
        }
        pos += previous;
        previous = pos;                
    }
    return list;   
}

/*
Commands of SyncBody tag
    Alert
    Add     
    Atomic
    Copy    
    Delete
    Exec    
    Get
    Map     
    Put     
    Replace
    Results 
    Search  
    Sequence
    Status  
    Sync
*/
ArrayList* Parser::getCommands(wchar_t* xml) {    
    ArrayList* ret    = new ArrayList();
    
    ArrayList* list = new ArrayList();
    Alert* alert        = NULL;
    Map*   map          = NULL;
    Get*   get          = NULL;
    Put*   put          = NULL;
    Status* status      = NULL;
    Results* result     = NULL;
    Exec* exec          = NULL;
    Search* search      = NULL;

    Sequence* sequence  = NULL;
    Atomic* atomic      = NULL;
    Sync* sync          = NULL;
    unsigned int pos = 0, previous = 0;
        
    // Status
    while ((status = getStatus(XMLProcessor::getElementContent(&xml[pos], STATUS, &pos))) != NULL) {
        if (status) {
            list->add(*status); // in the ArrayList NULL element cannot be inserted
            deleteStatus(&status);            
        }
        pos += previous;
        previous = pos;                
    }
   
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);    
        
    // Alert: use the getElementContentLevel because Alert could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    while ((alert = getAlert(XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos))) != NULL) {
        if (alert) {
            list->add(*alert); // in the ArrayList NULL element cannot be inserted
            deleteAlert(&alert);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Map: use the getElementContentLevel because Map could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    while ((map = getMap(XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos))) != NULL) {
        if (map) {
            list->add(*map); // in the ArrayList NULL element cannot be inserted
            deleteMap(&map);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        

    deleteArrayList(&list);
    
    // Get: use the getElementContentLevel because Get could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    while ((get = getGet(XMLProcessor::getElementContent(&xml[pos], GET, &pos))) != NULL) {
        if (get) {
            list->add(*get); // in the ArrayList NULL element cannot be inserted
            deleteGet(&get);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Put
    pos = 0, previous = 0;
    while ((put = getPut(XMLProcessor::getElementContent(&xml[pos], PUT, &pos))) != NULL) {
        if (put) {
            list->add(*put); // in the ArrayList NULL element cannot be inserted
            deletePut(&put);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Results
    pos = 0, previous = 0;
    while ((result = getResult(XMLProcessor::getElementContent(&xml[pos], RESULTS, &pos))) != NULL) {
        if (result) {
            list->add(*result); // in the ArrayList NULL element cannot be inserted
            deleteResults(&result);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Exec: use the getElementContentLevel because Exec could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    while ((exec = getExec(XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos))) != NULL) {
        if (exec) {
            list->add(*exec); // in the ArrayList NULL element cannot be inserted
            deleteExec(&exec);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Search
    pos = 0, previous = 0;
    while ((search = getSearch(XMLProcessor::getElementContent(&xml[pos], SEARCH, &pos))) != NULL) {
        if (search) {
            list->add(*search); // in the ArrayList NULL element cannot be inserted
            deleteSearch(&search);            
        }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // get the Sync commands. not belonging to Atomic and Sequence
    //sync = getSync(XMLProcessor::getElementContentExcept (xml, SYNC, TEXT("Atomic&Sequence"), NULL));
    
    //if (sync) {
    //    ret->add(*sync);
    //    deleteSync(&sync);
    //}
    
    // Sync
    pos = 0, previous = 0;
    while ((sync = getSync(XMLProcessor::getElementContentExcept(&xml[pos], SYNC, TEXT("Atomic&Sequence"), &pos))) != NULL) {
    if (sync) {
            list->add(*sync); // in the ArrayList NULL element cannot be inserted
        deleteSync(&sync);
    }
        pos += previous;
        previous = pos;                
    }
    
    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);



    // get the Sequence commands. Not belonging to Atomic and Sync
    sequence = getSequence(XMLProcessor::getElementContentExcept(xml, SEQUENCE, TEXT("Atomic&Sync"), NULL));    
    
    if (sequence) {
        ret->add(*sequence);
        deleteSequence(&sequence);
    }
    
    // get the Sequence commands. Not belonging to Sequence and Sync and Atomic
    atomic = getAtomic(XMLProcessor::getElementContentExcept(xml, ATOMIC, TEXT("Atomic&Sync&Sequence"), NULL));   
    
    if (atomic) {
        ret->add(*atomic);
        deleteAtomic(&atomic);
    }
    
    
    list = getCommonCommandList(xml, TEXT("Atomic&Sync&Sequence"));

    if (list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }
    deleteArrayList(&list);
    
    return ret;
}

Status* Parser::getStatus(wchar_t* xml) {
    
    if (!xml)
        return NULL;
    
    Status*     ret         = NULL;
    
    CmdID*      cmdID       = NULL;
    wchar_t*    msgRef      = NULL;
    wchar_t*    cmdRef      = NULL;
    wchar_t*    cmd         = NULL;
    ArrayList*  targetRefs  = new ArrayList();
    ArrayList*  sourceRefs  = new ArrayList();
    Cred*       cred        = NULL;
    Chal*       chal        = NULL;
    Data*       data        = NULL;
    ArrayList*  items       = new ArrayList();

    cmdID           = getCmdID     (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    msgRef          = XMLProcessor::getElementContent (xml, MSG_REF, NULL);
    cmdRef          = XMLProcessor::getElementContent (xml, CMD_REF, NULL);
    cmd             = XMLProcessor::getElementContent (xml, CMD,     NULL);
    cred            = getCred      (XMLProcessor::getElementContent (xml, CRED   , NULL));
    // get Data <Data>200</Data>
    data            = getData      (XMLProcessor::getElementContent (xml, DATA   , NULL));
    items           = getItems     (xml);
    targetRefs      = getTargetRefs(xml);
    sourceRefs      = getSourceRefs(xml);    
    chal            = getChal      (XMLProcessor::getElementContent (xml, CHAL   , NULL));    
    
    if (NotNullCheck(2, msgRef, cmdRef) || (cred) 
                                        || (data)
                                        || (cmdID) 
                                        || (chal) 
                                        || NotZeroArrayLenght(3, items, targetRefs, sourceRefs)                                        
                                        )  {

        ret = new Status(cmdID, msgRef, cmdRef, cmd, targetRefs, sourceRefs, cred, chal, data, items);
    }
    deleteArrayList(&sourceRefs);
    deleteArrayList(&targetRefs);
    deleteArrayList(&items);
    deleteAll(3, &msgRef, &cmdRef, &cmd);
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteData(&data);
    deleteChal(&chal);

    return ret;   
}

Chal* Parser::getChal(wchar_t* xml) {
    
    Chal* ret  = NULL;
    Meta* meta = NULL;

    meta     = getMeta (xml);    

    if (meta) {
        ret = new Chal(meta);
        deleteMeta(&meta); 
    }

    return ret;
}

ArrayList* Parser::getTargetRefs(wchar_t* xml) {
    ArrayList* list = new ArrayList();
    TargetRef* targetRef = NULL;
    unsigned int pos = 0, previous = 0;

    while ((targetRef = getTargetRef(XMLProcessor::getElementContent(&xml[pos], TARGET_REF, &pos))) != NULL) {
        if (targetRef) {
            list->add(*targetRef); // in the ArrayList NULL element cannot be inserted
            deleteTargetRef(&targetRef);            
        }
        pos += previous;
        previous = pos;                
    }
    
    return list;
}

ArrayList* Parser::getSourceRefs(wchar_t* xml) {
    ArrayList* list = new ArrayList();
    SourceRef* sourceRef = NULL;
    unsigned int pos = 0, previous = 0;

    while ((sourceRef = getSourceRef(XMLProcessor::getElementContent(&xml[pos], SOURCE_REF, &pos))) != NULL) {
        if (sourceRef) {
            list->add(*sourceRef); // in the ArrayList NULL element cannot be inserted
            deleteSourceRef(&sourceRef);            
        }
        pos += previous;
        previous = pos;                
    }
    
    return list;
}

SourceRef* Parser::getSourceRef(wchar_t* xml) {    
    SourceRef* ret = NULL;        
    Source* source;   
    
    source = getSource(xml);    
    if (source) {
        ret = new SourceRef(source);
    } else if (xml) {
        ret = new SourceRef(xml);
    }
    
    return ret;
}

TargetRef* Parser::getTargetRef(wchar_t* xml) {    
    TargetRef* ret = NULL;        
    Target* target;   
    
    target = getTarget(xml);    
    if (target) {
        ret = new TargetRef(target);
    } else if (xml) {
        ret = new TargetRef(xml);
    }
    
    return ret;
}

Alert* Parser::getAlert(wchar_t* xml) {
    
    Alert* ret = NULL;
    ArrayList* items = new ArrayList();    
    
    CmdID* cmdID     = getCmdID   (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    Cred*  cred      = getCred    (XMLProcessor::getElementContent (xml, CRED   , NULL));
    int    data      = getDataCode(XMLProcessor::getElementContent (xml, DATA   , NULL));
    BOOL   noResp    = getNoResp  (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    
    items = getItems(xml);
    if (items->size() > 0) {
        ret = new Alert(cmdID, noResp, cred, data, items); //Item[]
    }
    
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteArrayList(&items);
    
    return ret;
}

Exec* Parser::getExec(wchar_t* xml) {
    
    Exec* ret = NULL;
    
    CmdID* cmdID        = NULL;
    BOOL   noResp       = FALSE;
    Cred*  cred         = NULL;
    ArrayList*  items   = new ArrayList();

    cmdID     = getCmdID   (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    cred      = getCred    (XMLProcessor::getElementContent (xml, CRED   , NULL));
    noResp    = getNoResp  (XMLProcessor::getElementContent (xml, NO_RESP, NULL));    
    items = getItems(xml);

    if (cmdID || NotZeroArrayLenght(1, items) || (cred)) {
        ret = new Exec(cmdID, noResp, cred, items); 
    }
    
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteArrayList(&items);
    
    return ret;
}

Get* Parser::getGet(wchar_t* xml) {
       
    Get* ret = NULL;    
    ArrayList* items = NULL;    
    
    CmdID* cmdID     = getCmdID   (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    Cred*  cred      = getCred    (XMLProcessor::getElementContent (xml, CRED   , NULL));
    BOOL   noResp    = getNoResp  (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    Meta*  meta      = getMeta    (XMLProcessor::getElementContentLevel (xml, META ,   NULL));
    wchar_t* lang    = XMLProcessor::getElementContent        (xml, LANG, NULL);
    items = getItems(xml);

    if (NotNullCheck(1, lang)  || (cred) 
                               || (cmdID) 
                               || (meta) 
                               || NotZeroArrayLenght(1, items))  {
    
        ret = new Get(cmdID, noResp, lang, cred, meta, items); //Item[]
    }
       
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteMeta(&meta);
    safeDel(&lang);
    deleteArrayList(&items);
    
    return ret;
}

Put* Parser::getPut(wchar_t* xml) {
    
    Put* ret = NULL;    
    ArrayList* items = NULL;    
    
    CmdID* cmdID     = getCmdID   (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    Cred*  cred      = getCred    (XMLProcessor::getElementContent (xml, CRED   , NULL));
    BOOL   noResp    = getNoResp  (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    Meta*  meta      = getMeta    (XMLProcessor::getElementContentLevel (xml, META ,   NULL));
    wchar_t* lang    = XMLProcessor::getElementContent        (xml, LANG, NULL);        
    items = getItems(xml);

    if (NotNullCheck(1, lang)  || (cred) 
                               || (cmdID) 
                               || (meta) 
                               || NotZeroArrayLenght(1, items))  {
    
        ret = new Put(cmdID, noResp, lang, cred, meta, items); //Item[]
    }
       
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteMeta(&meta);
    safeDel(&lang);
    deleteArrayList(&items);
    
    return ret;
}

Search* Parser::getSearch(wchar_t* xml) {    
    
    Search*     ret      = NULL;
    CmdID*      cmdID    = NULL;
    BOOL        noResp   = NULL;
    BOOL        noResults= NULL;
    Cred*       cred     = NULL;
    Target*     target   = NULL;
    ArrayList*  sources  = new ArrayList();  // an Array of SourceArray object
    wchar_t*    lang     = NULL;
    Meta*       meta     = NULL;
    Data*       data     = NULL;
    
    cmdID     = getCmdID   (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    cred      = getCred    (XMLProcessor::getElementContent (xml, CRED   , NULL));
    noResp    = getNoResp  (XMLProcessor::getElementContent (xml, NO_RESP, NULL));
    noResults = getNoResults(XMLProcessor::getElementContent(xml, NO_RESULTS, NULL));
    target    = getTarget  (XMLProcessor::getElementContent (xml, TARGET,         NULL));
    lang      = XMLProcessor::getElementContent             (xml, LANG, NULL);            
    meta      = getMeta    (XMLProcessor::getElementContentLevel (xml, META , NULL));
    data      = getData    (XMLProcessor::getElementContent (xml, DATA ,   NULL));    
    sources   = getSources (xml);

    if (NotNullCheck(1, lang) || (cmdID) || (cred) 
                              || (meta)  || (target) 
                              || (data)  || NotZeroArrayLenght(1, sources))  {
                
        ret = new Search(cmdID, noResp, noResults, cred, target, sources, lang, meta, data);
    }

    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteTarget(&target);
    safeDel(&lang);
    deleteData(&data);
    deleteMeta(&meta);
    deleteArrayList(&sources);
    
  return ret;
}

Results* Parser::getResult(wchar_t* xml) {
    
    if (!xml)
        return NULL;
    
    Results*    ret         = NULL;        
    CmdID*      cmdID       = NULL;
    wchar_t*    msgRef      = NULL;
    wchar_t*    cmdRef      = NULL;
    Meta*       meta        = NULL;   
    ArrayList*  targetRefs  = new ArrayList(); // it could be an array with only a value
    ArrayList*  sourceRefs  = new ArrayList();
    ArrayList*  items       = new ArrayList();

    cmdID           = getCmdID     (XMLProcessor::getElementContent (xml, CMD_ID , NULL));
    msgRef          = XMLProcessor::getElementContent (xml, MSG_REF, NULL);
    cmdRef          = XMLProcessor::getElementContent (xml, CMD_REF, NULL);
    meta            = getMeta      (XMLProcessor::getElementContentLevel(xml, META, NULL));
    targetRefs      = getTargetRefs(xml);
    sourceRefs      = getSourceRefs(xml);    
    items           = getItems     (xml);
    
    if (NotNullCheck(2, msgRef, cmdRef) || (cmdID) || (meta) 
                                        || NotZeroArrayLenght(3, items, targetRefs, sourceRefs)                                        
                                        )  {

        ret = new Results(cmdID, msgRef, cmdRef, meta, targetRefs, sourceRefs, items);
    }
    deleteArrayList(&sourceRefs);
    deleteArrayList(&targetRefs);
    deleteArrayList(&items);
    deleteAll(2, &msgRef, &cmdRef);
    deleteCmdID(&cmdID);
    deleteMeta(&meta);
    
    return ret;   
}


//
// return and array list of items
//
ArrayList* Parser::getItems(wchar_t* xml) {

    Item* item = NULL;
    ArrayList* items = new ArrayList();
    unsigned int pos = 0, previous = 0;

    while ((item = getItem(XMLProcessor::getElementContent(&xml[pos], ITEM, &pos))) != NULL) {
        if (item) {
            items->add(*item);    // in the ArrayList NULL element cannot be inserted
            deleteItem(&item);    
        }
        pos += previous;
        previous = pos;                
    }

    return items;    
}

Item* Parser::getItem(wchar_t* xml) {    
    Item*   ret       = NULL;
    Target* target    = NULL;
    Source* source    = NULL;
    Meta*   meta      = NULL;
    ComplexData* data = NULL;
    BOOL moreData     = FALSE;  
    wchar_t* targetParent = NULL;
    wchar_t* sourceParent = NULL;

    
    target   = getTarget     (XMLProcessor::getElementContent(xml, TARGET,         NULL));
    source   = getSource     (XMLProcessor::getElementContent(xml, SOURCE,         NULL));
    meta     = getMeta       (XMLProcessor::getElementContentLevel(xml, META,           NULL));
    data     = getComplexData(XMLProcessor::getElementContent(xml, COMPLEX_DATA,   NULL));
    moreData = getMoreData   (XMLProcessor::getElementContent(xml, MORE_DATA,      NULL));
    targetParent = XMLProcessor::getElementContent(xml, TARGET_PARENT,      NULL);
    sourceParent = XMLProcessor::getElementContent(xml, SOURCE_PARENT,      NULL);

    if ((target)     || 
            (source) || 
            (meta)   ||
            (data))  {
        // ret = new Item(target, source, meta, data, moreData);
        ret = new Item(target, source, targetParent, sourceParent, meta, data, moreData);

    }
    
    deleteTarget     (&target);
    deleteSource     (&source);
    deleteMeta       (&meta);
    deleteComplexData(&data);
    deleteAll(2, &targetParent, &sourceParent);
    
    return ret;
}

int Parser::getDataCode(wchar_t* content) {    
   int ret = 0;
   if (content) {
        ret = wcstol(content, NULL, 10);
   }
   return ret;
}

Data* Parser::getData(wchar_t* content) {    
   Data* ret = 0;
   if (content) {
        ret = new Data(content);
   }
   return ret;
}

BOOL Parser::getFinalMsg(wchar_t* content) {    
    BOOL ret = FALSE;
    if (content) {
        ret = TRUE;
    }
    return ret;
}

CmdID* Parser::getCmdID(wchar_t* content) {
    CmdID* ret = NULL;
    if (content) {
        ret = new CmdID(content);
    }
    return ret;
}

ComplexData* Parser::getComplexData(wchar_t* xml) {
    
    ComplexData* ret = NULL;
    Anchor* anchor   = NULL;
    DevInf* devInf   = NULL;
    
    anchor = getAnchor(xml);
    devInf = getDevInf(xml);    
    
    if (anchor || devInf) {
        ret = new ComplexData(NULL); 
    
        if (anchor) 
            ret->setAnchor(anchor);
        if (devInf)
            ret->setDevInf(devInf);
    }
    else if (xml) {
        ret = new ComplexData(xml);
    }    

    return ret;
}

DevInf* Parser::getDevInf(wchar_t* xml) {
    DevInf* ret             = NULL;
    DataStore* dataStore    = NULL;
    CTCap* ctCap            = NULL;
    Ext* ext                = NULL;

    VerDTD* verDTD          = NULL;
    wchar_t* man            = NULL;
    wchar_t* mod            = NULL;
    wchar_t* oem            = NULL;
    wchar_t* fwV            = NULL;
    wchar_t* swV            = NULL;
    wchar_t* hwV            = NULL;
    wchar_t* devId          = NULL;
    wchar_t* devTyp         = NULL;
    ArrayList* dataStores   = new ArrayList();  //DataStore[]
    ArrayList* ctCaps       = new ArrayList();       // CTCap[]
    ArrayList* exts         = new ArrayList();         // Ext[]
    BOOL utc                = NULL;         // if present they Support UTC
    BOOL supportLargeObjs   = NULL;         // if present they Support largeObject
    BOOL supportNumberOfChanges = NULL;     // if present they Support NumberOfChanges
    SyncCap* syncCap        = NULL;

    wchar_t* value          = NULL;
    
    verDTD = getVerDTD(XMLProcessor::getElementContent(xml, VER_DTD,           NULL));
    man = XMLProcessor::getElementContent(xml, MAN,           NULL);
    mod = XMLProcessor::getElementContent(xml, MOD,           NULL);
    oem = XMLProcessor::getElementContent(xml, OEM,           NULL);
    fwV = XMLProcessor::getElementContent(xml, FWV,           NULL);
    swV = XMLProcessor::getElementContent(xml, SWV,           NULL);
    hwV = XMLProcessor::getElementContent(xml, HWV,           NULL);
    devId = XMLProcessor::getElementContent(xml, DEV_ID,           NULL);
    devTyp = XMLProcessor::getElementContent(xml, DEV_TYP,           NULL);

    syncCap = getSyncCap(XMLProcessor::getElementContent(xml, SYNC_CAP,           NULL));
    
    unsigned int pos = 0, previous = 0;
        
    // DataStore
    while ((dataStore = getDataStore(XMLProcessor::getElementContent(&xml[pos], DATA_STORE, &pos))) != NULL) {
        if (dataStore) {
            dataStores->add(*dataStore); // in the ArrayList NULL element cannot be inserted
            deleteDataStore(&dataStore);            
        }
        pos += previous;
        previous = pos;                
    }
    
        
    // ctCap
    pos = 0; previous = 0;
    while ((ctCap = getCTCap(XMLProcessor::getElementContent(&xml[pos], CT_CAP, &pos))) != NULL) {
        if (ctCap) {
            ctCaps->add(*ctCap); // in the ArrayList NULL element cannot be inserted
            deleteCTCap(&ctCap);            
        }
        pos += previous;
        previous = pos;                
    }

    // ext
    pos = 0; previous = 0;
    while ((ext = getExt(XMLProcessor::getElementContent(&xml[pos], EXT, &pos))) != NULL) {
        if (ext) {
            exts->add(*ext); // in the ArrayList NULL element cannot be inserted
            deleteExt(&ext);            
        }
        pos += previous;
        previous = pos;                
    }

    //
    // The large object value depends on SUPPORT_LARGE_OBJECT tag.
    //
    if ((value = XMLProcessor::getElementContent(xml, SUPPORT_LARGE_OBJECT, NULL)) != NULL) {
        if (wcscmpIgnoreCase(value, TEXT("TRUE"))) {
            supportLargeObjs = TRUE;
        }
        safeDel(&value);
    }

    //
    // The large object value depends on SUPPORT_NUMBER_OF_CHANGES tag. 
    //    
    if ((value = XMLProcessor::getElementContent(xml, SUPPORT_NUMBER_OF_CHANGES, NULL)) != NULL) {
        if (wcscmpIgnoreCase(value, TEXT("TRUE"))) {
            supportNumberOfChanges = TRUE;
        }
        safeDel(&value);
    }

    //
    // The large object value depends on UTC tag.
    //
    if ((value = XMLProcessor::getElementContent(xml, UTC, NULL)) != NULL) {
        if (wcscmpIgnoreCase(value, TEXT("TRUE"))) {
            utc = TRUE;
        }
        safeDel(&value);
    }    
    
    if (NotNullCheck(8, man, mod, oem, fwV, swV, hwV, devId, devTyp) ||
                                     (verDTD)      ||
                                     (syncCap)     ||
                                      NotZeroArrayLenght(3, dataStores, ctCaps, exts) ) {

        ret = new DevInf(verDTD, man, mod, oem,  fwV, swV, hwV, devId, devTyp,
                                dataStores, ctCaps, exts,
                                utc, supportLargeObjs, supportNumberOfChanges,
                                syncCap);

    }
    deleteAll(8, &man, &mod, &oem, &fwV, &swV, &hwV, &devId, &devTyp);
    deleteArrayList(&dataStores);
    deleteArrayList(&ctCaps);
    deleteArrayList(&exts);
    deleteVerDTD(&verDTD);
    deleteSyncCap(&syncCap);
    return ret;
}


/*
* TBD. There is to use the getNextTag method in xmlProcessor.
* This CTCap is no nested as a usual XML. See syncml_devinf_v11_20020215.pdf
*
*/
Ext* Parser::getExt(wchar_t* xml) {
    Ext* ret = NULL;
    wchar_t* XNam       = NULL;
    wchar_t* value      = NULL;
    ArrayList* list     = new ArrayList();
    StringElement* s      = NULL;
    unsigned int pos = 0, previous = 0;

    XNam = XMLProcessor::getElementContent(xml, XNAM, NULL);

    // XVal
    while ((value = XMLProcessor::getElementContent(&xml[pos], XVAL, &pos)) != NULL) {
        if (value) {
            s = new StringElement(value);
            list->add(*s); 
            deleteStringElement(&s);            
            safeDel(&value);
        }
        pos += previous;
        previous = pos;                
    }
       
    if ( XNam || NotZeroArrayLenght(1, list) ) {
        ret = new Ext(XNam, list);
    }
    
    safeDel(&XNam);
    deleteArrayList(&list);

    return ret;
}

DataStore* Parser::getDataStore(wchar_t* xml) {
    DataStore* ret = NULL;
    
    SourceRef*       sourceRef      = NULL;
    wchar_t*         displayName    = NULL;
    long             maxGUIDSize    = 0;
    wchar_t*         maxGUIDSizeW   = NULL;
    ContentTypeInfo* rxPref         = NULL;
    ArrayList*       rx             = new ArrayList(); // ContentTypeInfo[]
    ContentTypeInfo* txPref         = NULL;;
    ArrayList*       tx             = new ArrayList(); // ContentTypeInfo[]
    DSMem*           dsMem          = NULL;
    SyncCap*         syncCap        = NULL;  

    ContentTypeInfo* x              = NULL;
    
    sourceRef   = getSourceRef(XMLProcessor::getElementContent(xml, SOURCE_REF,  NULL));
    displayName = XMLProcessor::getElementContent(xml, DISPLAY_NAME,             NULL);
    maxGUIDSizeW = XMLProcessor::getElementContent(xml, MAX_GUID_SIZE,           NULL);
    if (maxGUIDSizeW) {
        maxGUIDSize = wcstol(maxGUIDSizeW, NULL, 10);
    }
    rxPref = getContentTypeInfo(XMLProcessor::getElementContent(xml, RX_PREF,  NULL));
    txPref = getContentTypeInfo(XMLProcessor::getElementContent(xml, TX_PREF,  NULL));
    
    unsigned int pos = 0, previous = 0;
        
    // Rx 
    while ((x = getContentTypeInfo(XMLProcessor::getElementContent(&xml[pos], RX, &pos))) != NULL) {
        if (x) {
            rx->add(*x); // in the ArrayList NULL element cannot be inserted
            deleteContentTypeInfo(&x);            
        }
        pos += previous;
        previous = pos;                
    }

    // Tx 
    pos = 0, previous = 0;
    while ((x = getContentTypeInfo(XMLProcessor::getElementContent(&xml[pos], TX, &pos))) != NULL) {
        if (x) {
            tx->add(*x); // in the ArrayList NULL element cannot be inserted
            deleteContentTypeInfo(&x);            
        }
        pos += previous;
        previous = pos;                
    }
    
    dsMem = getDSMem(XMLProcessor::getElementContent(xml, DS_MEM,  NULL));    
    syncCap = getSyncCap(XMLProcessor::getElementContent(xml, SYNC_CAP,  NULL));  
        
    if (NotNullCheck(2, displayName, maxGUIDSizeW) ||
                                     (sourceRef)   ||
                                     (rxPref)      ||
                                     (txPref)      ||
                                     (dsMem)       ||
                                     (syncCap)     ||
                                     NotZeroArrayLenght(2, rx, tx) ) {
        ret = new DataStore(sourceRef, displayName, maxGUIDSize, rxPref, rx, txPref, tx, dsMem, syncCap);
    }
    
    safeDel(&displayName);
    safeDel(&maxGUIDSizeW);
    deleteContentTypeInfo(&rxPref);
    deleteContentTypeInfo(&txPref);
    deleteArrayList(&tx);
    deleteArrayList(&rx);
    deleteSyncCap(&syncCap);
    deleteDSMem(&dsMem);


    return ret;
}


SyncCap* Parser::getSyncCap(wchar_t* xml) {
    
    SyncCap* ret            = NULL;
    SyncType* syncType      = NULL;
    ArrayList* list         = new ArrayList();

    unsigned int pos = 0, previous = 0;
    while ((syncType = getSyncType(XMLProcessor::getElementContent(&xml[pos], SYNC_TYPE, &pos))) != NULL) {
        if (syncType) {
            list->add(*syncType); // in the ArrayList NULL element cannot be inserted
            deleteSyncType(&syncType);            
        }
        pos += previous;
        previous = pos;                
    }
        
    if (NotZeroArrayLenght(1, list)) {
        ret = new SyncCap(list);
    }

    deleteArrayList(&list);

    return ret;
}

SyncType* Parser::getSyncType(wchar_t* content) {
    
    SyncType* ret            = NULL;    
    int value                = 0;
  
    if (content) {
         value = wcstol(content, NULL, 10);
         if (value >= 1 && value <= 7) {
             ret = new SyncType(value);
         }
    }    
    
    return ret;
}


ContentTypeInfo* Parser::getContentTypeInfo(wchar_t* xml) {
    
    ContentTypeInfo* ret = NULL;
    wchar_t* ctType      = NULL;
    wchar_t* verCT       = NULL;     

    ctType = XMLProcessor::getElementContent(xml, CT_TYPE,             NULL);
    verCT  = XMLProcessor::getElementContent(xml, VER_CT,             NULL);
    
    if (NotNullCheck(2, ctType, verCT)) {
        ret = new ContentTypeInfo(ctType, verCT);
    }
    
    safeDel(&ctType);
    safeDel(&verCT);
    return ret;
}

DSMem* Parser::getDSMem(wchar_t* xml) {    
    
    DSMem* ret          = NULL;
    wchar_t* maxMemW   = NULL;   
    wchar_t* sharedMemW = NULL;
    wchar_t* maxIDW    = NULL;
        
    BOOL    sharedMem   = NULL;
    long    maxMem     = 0;
    long    maxID      = 0;
    
    BOOL isToCreate = FALSE;

    maxMemW     = XMLProcessor::getElementContent (xml, MAX_MEM,   NULL);
    sharedMemW  = XMLProcessor::getElementContent (xml, SHARED_MEM, NULL);
    maxIDW      = XMLProcessor::getElementContent (xml, MAX_ID,    NULL);
    
    isToCreate = NotNullCheck(3, maxMemW, sharedMemW, maxIDW);

    if (maxMemW != NULL) {
        maxMem = wcstol(maxMemW, NULL, 10);
    }
    if (maxIDW != NULL) {
        maxID = wcstol(maxIDW, NULL, 10);
    } 
    if (sharedMemW != NULL) {
        sharedMem = wcstol(sharedMemW, NULL, 10);
    }
    
    if (isToCreate) {
        ret = new DSMem(sharedMem, maxMem, maxID);
    }

    safeDel(&maxMemW);
    safeDel(&maxIDW);
    safeDel(&sharedMemW);

    return ret;   

}

BOOL Parser::getNoResp(wchar_t* content) {    
  if (content)
      return TRUE;
  else
      return FALSE;
}

BOOL Parser::getNoResults(wchar_t* content) {    
    
    if (content)
        return TRUE;
    else    
        return FALSE;
}

BOOL Parser::getMoreData(wchar_t* content) {
     if (content)
        return TRUE;
    else    
        return FALSE;
}

/*
* TBD. There is to use the getNextTag method in xmlProcessor.
* This CTCap is no nested as a usual XML. See syncml_devinf_v11_20020215.pdf
* TBD
*
*/
CTCap* Parser::getCTCap(wchar_t* xml) {
    CTCap* ret = NULL;
    CTTypeSupported* ctTypeSupported = NULL;

    // ArrayList* ctTypes = new ArrayList();       
       
    return ret;
}

//
// TBD 
//
ArrayList* Parser::getEMI(wchar_t* content) {
    ArrayList* ret = new ArrayList();    
    return ret;
}
