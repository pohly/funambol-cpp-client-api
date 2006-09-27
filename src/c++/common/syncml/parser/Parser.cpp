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
 
SyncML* Parser::getSyncML(char* xml) {
    SyncBody* syncBody = NULL;
    SyncHdr*  syncHdr  = NULL;
    SyncML*   syncML   = NULL; 
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent(xml, SYNC_HDR, &pos);
    syncHdr  = getSyncHdr (t);
    if (t) { delete [] t; t = NULL; }
    t = XMLProcessor::getElementContent(xml, SYNC_BODY, &pos);
    syncBody = getSyncBody(t);
    if (t) { delete [] t; t = NULL; }
    syncML = new SyncML(syncHdr, syncBody);
    
    deleteSyncHdr (&syncHdr);
    deleteSyncBody(&syncBody);
    
    return syncML;
     
}

SyncHdr* Parser::getSyncHdr(char* xml) {
         
    SessionID*   sessionID = NULL;
    VerDTD*      verDTD    = NULL;
    VerProto*    verProto  = NULL;
    Source*      source    = NULL;
    Target*      target    = NULL;
    Cred*        cred      = NULL;
    char*     respURI   = NULL;
    char*     msgID     = NULL;
    BOOL         noResp    = NULL;             
    char*     tmp       = NULL;
    Meta*        meta      = NULL;

    SyncHdr*     ret       = NULL;
    unsigned int pos = 0;
    char* t = NULL;
    
    t = XMLProcessor::getElementContent(xml, SESSION_ID, NULL);
    sessionID = getSessionID(t);
    if (t) { delete [] t; t = NULL; }
    t = XMLProcessor::getElementContent (xml, VER_DTD, NULL);
    verDTD = getVerDTD(t);
    if (t) { delete [] t; t = NULL; }
    t = XMLProcessor::getElementContent (xml, VER_PROTO, NULL);
    verProto = getVerProto(t);
    if (t) { delete [] t; t = NULL; }
    t = XMLProcessor::getElementContent (xml, SOURCE, NULL);
    source = getSource(t);
    if (t) { delete [] t; t = NULL; }
    t = XMLProcessor::getElementContent (xml, TARGET, NULL);
    target = getTarget(t);    
    if (t) { delete [] t; t = NULL; }
    t = XMLProcessor::getElementContent (xml, CRED, NULL);
    cred = getCred(t);
    if (t) { delete [] t; t = NULL; }
    msgID = XMLProcessor::getElementContent(xml, MSG_ID, NULL);
    respURI = XMLProcessor::getElementContent(xml, RESP_URI, NULL);
    t = XMLProcessor::getElementContentLevel(xml, META, NULL);
    meta = getMeta(t);
    if (t) { delete [] t; t = NULL; }
    tmp = XMLProcessor::getElementContent(xml, NO_RESP, NULL);

    if (tmp) {
        wcscmpIgnoreCase(tmp, T("TRUE")) ? noResp = TRUE : noResp = FALSE;            
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

Cred* Parser::getCred(char* xml) {
    Cred* ret              = NULL;
    Authentication* auth   = NULL;    
    
    auth = getAuthentication(xml);
    if (auth) { 
        ret = new Cred(auth); 
    }
    
    deleteAuthentication(&auth);

    return ret;
}

Authentication* Parser::getAuthentication(char* xml) {
    Authentication* ret        = NULL;
    
    char* data       = NULL;    
    Meta*  meta       = NULL;
    
    unsigned int pos = 0;
    char* t = NULL;

    data = XMLProcessor::getElementContent (xml, DATA , NULL);
    t = XMLProcessor::getElementContentLevel (xml, META , NULL);
    meta = getMeta(t);
    if (t) {delete [] t; t = NULL;}
    if (data || meta) {    
        ret = new Authentication(meta, data);
    }
    safeDel(&data);
    deleteMeta(&meta);
    
    return ret;
}

Meta* Parser::getMeta(char* xml) {
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

MetInf* Parser::getMetInf(char* xml) {
    MetInf* ret             = NULL;
    
    char*     format     = NULL;
    char*     type       = NULL;
    char*     mark       = NULL;
    Anchor*      anchor     = NULL;
    char*     version    = NULL;
    NextNonce*   nextNonce  = NULL;
    long         maxMsgSize = 0;
    long         maxObjSize = 0;
    long         size       = 0;
    ArrayList*   emi        = NULL;
    Mem*         mem        = NULL; 
    
    char*         maxMsgSizeW   = NULL;
    char*         maxObjSizeW   = NULL;
    char*         sizeW         = NULL;    
    
    // get all the values

    format       = XMLProcessor::getElementContent (xml, FORMAT   , NULL);
    type         = XMLProcessor::getElementContent (xml, TYPE     , NULL);
    mark         = XMLProcessor::getElementContent (xml, MARK     , NULL);
    
    anchor       = getAnchor(xml);
    version      = XMLProcessor::getElementContent (xml, VERSIONSTR       , NULL);
    nextNonce    = getNextNonce(xml);
    
    maxMsgSizeW  = XMLProcessor::getElementContent (xml, MAX_MESSAGE_SIZE     , NULL);
    maxObjSizeW  = XMLProcessor::getElementContent (xml, MAX_OBJ_SIZE     , NULL);
    sizeW        = XMLProcessor::getElementContent (xml, SIZE             , NULL);
        
    if (maxMsgSizeW) {
        maxMsgSize = strtol(maxMsgSizeW, NULL, 10);
    }
    if (maxObjSizeW) {
        maxObjSize = strtol(maxObjSizeW, NULL, 10);
    }
    if (sizeW) {
        size = strtol(sizeW, NULL, 10);
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


ArrayList* Parser::getSources(char* xml) {
    
    Source* source = NULL; 
    SourceArray* sourceArray = NULL;
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();

    char* t = NULL;
    t = XMLProcessor::getElementContent(&xml[pos], SOURCE, &pos);
    while ((source = getSource(t)) != NULL) {
        if (source) {
            sourceArray = new SourceArray(source);
            list->add(*sourceArray); // in the ArrayList NULL element cannot be inserted
            deleteSource(&source);            
            deleteSourceArray(&sourceArray);            
        }
        pos += previous;
        previous = pos;
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], SOURCE, &pos);
    }
    if (t) { delete [] t; t = NULL;}
    return list;    
}


Source* Parser::getSource(char* xml) {
    Source* ret      = NULL;
    char* locURI  = NULL;
    char* locName = NULL;
    locURI  = XMLProcessor::getElementContent (xml, LOC_URI, NULL);
    locName = XMLProcessor::getElementContent (xml, LOC_NAME, NULL);
    
    if (NotNullCheck(2, locURI, locName)) {
        ret = new Source(locURI, locName);
    }
    
    safeDel(&locURI);
    safeDel(&locName);
    
    return ret;
}

Target* Parser::getTarget(char* xml) {
    Target*  ret      = NULL;
    char* locURI   = NULL;
    char* locName  = NULL;
    
    locURI  = XMLProcessor::getElementContent (xml, LOC_URI, NULL);
    locName = XMLProcessor::getElementContent (xml, LOC_NAME, NULL);
    
    if (NotNullCheck(2, locURI, locName)) {
        ret = new Target(locURI, locName);
    }
    
    safeDel(&locURI);
    safeDel(&locName);

    return ret;
}

Anchor* Parser::getAnchor(char* xml) {
    Anchor*  ret    = NULL;
    char* last   = NULL;
    char* next   = NULL;
    
    last  = XMLProcessor::getElementContent (xml, LAST, NULL);
    next  = XMLProcessor::getElementContent (xml, NEXT, NULL);
    
    if (NotNullCheck(2, last, next)) {
        ret = new Anchor(last, next);
    }
    
    safeDel(&next);
    safeDel(&last);

    return ret;   
}

NextNonce* Parser::getNextNonce(char* xml) {
    NextNonce* ret   = NULL;
    char* value   = NULL;   
    
    value  = XMLProcessor::getElementContent (xml, NEXT_NONCE, NULL);

    if (NotNullCheck(1, value)) {

        ret = new NextNonce(value);        
    }
    
    safeDel(&value);
    return ret;   
}

Mem* Parser::getMem(char* xml) {
    Mem* ret            = NULL;
    char* freeMemW   = NULL;   
    char* sharedMemW = NULL;
    char* freeIDW    = NULL;
        
    BOOL    sharedMem   = NULL;
    long    freeMem     = 0;
    long    freeID      = 0;
    
    BOOL isToCreate = FALSE;

    freeMemW    = XMLProcessor::getElementContent (xml, FREE_MEM,   NULL);
    sharedMemW  = XMLProcessor::getElementContent (xml, SHARED_MEM, NULL);
    freeIDW     = XMLProcessor::getElementContent (xml, FREE_ID,    NULL);
    
    isToCreate = NotNullCheck(3, freeMemW, sharedMemW, freeIDW);

    if (freeMemW != NULL) {
        freeMem = strtol(freeMemW, NULL, 10);
    }
    if (freeIDW != NULL) {
        freeID = strtol(freeIDW, NULL, 10);
    } 
    if (sharedMemW != NULL) {
        sharedMem = strtol(sharedMemW, NULL, 10);
    }
    
    if (isToCreate) {
        ret = new Mem(sharedMem, freeMem, freeID);
    }

    safeDel(&freeMemW);
    safeDel(&freeIDW);
    safeDel(&sharedMemW);

    return ret;   
}


SessionID* Parser::getSessionID(char* content) {
    SessionID* ret = NULL;
    if (content) {
        ret = new SessionID(content);
    }
    return ret;
}

VerDTD* Parser::getVerDTD(char* content) {
    VerDTD* ret = NULL;
    if (content) {
        ret = new VerDTD(content);
    }
    return ret;
}

VerProto* Parser::getVerProto(char* content) {
    VerProto* ret = NULL;
    if (content) {
        ret = new VerProto(content);
    }
    return ret;
}

SyncBody* Parser::getSyncBody(char* xml) {
    
    SyncBody* syncBody   = NULL;
    BOOL finalMsg        = FALSE;
    ArrayList* commands;
    unsigned int pos = 0;
    char* t = NULL;
    commands = getCommands(xml);
    t = XMLProcessor::getElementContent(xml, FINAL_MSG, NULL);
    finalMsg = getFinalMsg(t);
    if (t) {delete [] t; t = NULL;}
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
Sequence* Parser::getSequence(char* xml) {    

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

    char* t = NULL;
    t = XMLProcessor::getElementContent(xml, CMD_ID, NULL);
    cmdID = getCmdID(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml,  META  , NULL);
    meta = getMeta(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp   = getNoResp(t);
    if (t) {delete [] t; t = NULL;}
    // list of commands that must not be leaf of Sync and Atomic
    commands = getCommonCommandList(xml, T("Atomic&Sync"));    

    // Alert
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos);
    while ((alert = getAlert(t)) != NULL) {
        if (alert) {
            list->add(*alert); // in the ArrayList NULL element cannot be inserted
            deleteAlert(&alert);            
        }
        pos += previous;
        previous = pos; 
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos);
    }
    if (t) {delete [] t; t = NULL;}
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Map
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos);
    while ((map = getMap(t)) != NULL) {
        if (map) {
            list->add(*map); // in the ArrayList NULL element cannot be inserted
            deleteMap(&map);            
        }
        pos += previous;
        previous = pos;
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos);
    }
    if (t) {delete [] t; t = NULL;}
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        

    deleteArrayList(&list);
    
    // Get
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], GET, &pos);
    while ((get = getGet(t)) != NULL) {
        if (get) {
            list->add(*get); // in the ArrayList NULL element cannot be inserted
            deleteGet(&get);            
        }
        pos += previous;
        previous = pos;
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], GET, &pos);
    }
    if (t) {delete [] t; t = NULL;}
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);   
   
    // Exec
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos);
    while ((exec = getExec(t)) != NULL) {
        if (exec) {
            list->add(*exec); // in the ArrayList NULL element cannot be inserted
            deleteExec(&exec);            
        }
        pos += previous;
        previous = pos;
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos);
    }
    if(t) {delete [] t; t = NULL;}
    
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    char* element = XMLProcessor::getElementContentLevel (xml,  SYNC, NULL);

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
Atomic* Parser::getAtomic(char* xml) {    

    Atomic* ret             = NULL;

    Meta*   meta            = NULL;
    BOOL    noResp          = NULL;
    CmdID*  cmdID           = NULL;
    ArrayList* commands     = new ArrayList(); 
    Sync* sync              = NULL;
    Sequence* sequence      = NULL;
    char* element = NULL;
    
    Alert* alert            = NULL;
    Map*   map              = NULL;
    Get*   get              = NULL;
    Exec* exec              = NULL;

    ArrayList* list     = new ArrayList(); 
    unsigned int pos = 0, previous = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent(xml, CMD_ID, NULL);
    cmdID    = getCmdID(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel(xml,  META  , NULL);
    meta     = getMeta       (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, NO_RESP, NULL);
    noResp   = getNoResp(t);
    if (t) {delete [] t; t = NULL;}
    // list of commands that must not be leaf of Sync and Atomic
    commands = getCommonCommandList(xml, T("Sync&Sequence"));
    
    // Alert
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos);
    while ((alert = getAlert(t)) != NULL) {
        if (alert) {
            list->add(*alert); // in the ArrayList NULL element cannot be inserted
            deleteAlert(&alert);            
        }
        pos += previous;
        previous = pos;
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos);
    }
    if (t) {delete [] t; t = NULL;}

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Map
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos);
    while ((map = getMap(t)) != NULL) {
        if (map) {
            list->add(*map); // in the ArrayList NULL element cannot be inserted
            deleteMap(&map);            
        }
        pos += previous;
        previous = pos; 
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos);
    }
    if (t) {delete [] t; t = NULL;}

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        

    deleteArrayList(&list);
    
    // Get
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], GET, &pos);
    while ((get = getGet(t)) != NULL) {
        if (get) {
            list->add(*get); // in the ArrayList NULL element cannot be inserted
            deleteGet(&get);            
        }
        pos += previous;
        previous = pos; 
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], GET, &pos);
    }
    if (t) {delete [] t; t = NULL;}

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);   
   
    // Exec
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos);
    while ((exec = getExec(t)) != NULL) {
        if (exec) {
            list->add(*exec); // in the ArrayList NULL element cannot be inserted
            deleteExec(&exec);            
        }
        pos += previous;
        previous = pos;
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos);
    }
    if (t) {delete [] t; t = NULL;}

    if (list && list->size() > 0) {
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

Sync* Parser::getSync(char* xml) {    

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
    long numberOfChanges    = -1;
    char* numberOfChangesW = NULL;

    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent(xml, CMD_ID,  NULL);
    cmdID    = getCmdID      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, TARGET,  NULL);
    target   = getTarget     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, SOURCE,  NULL);
    source   = getSource     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml,  META  ,NULL);
    meta     = getMeta       (t);
    if (t) {delete [] t; t = NULL;}
    numberOfChangesW = XMLProcessor::getElementContent (xml,  NUMBER_OF_CHANGES ,NULL);
    if (numberOfChangesW) {
        numberOfChanges = strtol(numberOfChangesW, NULL, 10);
    }

    t = XMLProcessor::getElementContent (xml, CRED   ,NULL);
    cred     = getCred       (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp   = getNoResp    (t);
    if (t) {delete [] t; t = NULL;}
    commands = getCommonCommandList(xml, T("Atomic&Sequence"));
    
    char* element = XMLProcessor::getElementContentExcept(xml,  SEQUENCE, T("Atomic"), NULL);
    
    if (element) {
        sequence = getSequence(element);
        if (sequence) {
            commands->add(*sequence);
            deleteSequence(&sequence);
        }
        safeDel(&element);
    }

    element = XMLProcessor::getElementContentExcept(xml,  ATOMIC, T("Atomic&Sequence"), NULL);
    
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

ArrayList* Parser::getCommonCommandList(char* xml, char* except) {

    ArrayList* commands = new ArrayList();
    ArrayList* list = new ArrayList();

    //
    //Delete
    //    
    list = getDels(xml, except);   
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);  

    //
    //Add
    //    
    list = getAdds(xml, except);
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);    
    
    //
    //Replace
    //    
    list = getReplaces(xml, except);
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);  
        

    //
    //Copy
    //    
    list = getCopies(xml, except);   
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            commands->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);  

    return commands;
}

Copy* Parser::getCopy(char* xml) {
    Copy* ret = NULL;
        
    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;

    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID   = getCmdID     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml, META,    NULL);
    meta    = getMeta      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred    = getCred      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp  = getNoResp    (t);
    if (t) {delete [] t; t = NULL;}
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


Add* Parser::getAdd(char* xml) {
    Add* ret = NULL;

    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID   = getCmdID     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml, META,    NULL);
    meta    = getMeta      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred    = getCred      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp  = getNoResp    (t);
    if (t) {delete [] t; t = NULL;}
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

Delete* Parser::getDelete(char* xml) {
    Delete* ret = NULL;
       
    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    BOOL        archive = NULL;
    BOOL        sftDel  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;

    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID   = getCmdID     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml, META,    NULL);
    meta    = getMeta      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred    = getCred      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp  = getNoResp    (t);
    if (t) {delete [] t; t = NULL;}
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

Replace* Parser::getReplace(char* xml) {
    Replace* ret = NULL;

    CmdID*      cmdID   = NULL;
    BOOL        noResp  = NULL;
    Cred*       cred    = NULL;
    Meta*       meta    = NULL;
    ArrayList*  items   = NULL;

    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID   = getCmdID     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml, META,    NULL);
    meta    = getMeta      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred    = getCred      (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp  = getNoResp    (t);
    if (t) {delete [] t; t = NULL;}
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

MapItem* Parser::getMapItem(char* xml) {
    MapItem* ret = NULL;
    
    Target*    target = NULL;
    Source*    source = NULL;
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent(xml, TARGET,NULL);
    target   = getTarget(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, SOURCE,NULL);
    source   = getSource(t);
    
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
ArrayList* Parser::getMapItems(char* xml) {
    
    MapItem* mapItem = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = new ArrayList();

    char* t = NULL;
    t = XMLProcessor::getElementContent(&xml[pos], MAP_ITEM, &pos);
    while ((mapItem = getMapItem(t)) != NULL) {
        if (mapItem) {
            list->add(*mapItem); // in the ArrayList NULL element cannot be inserted
            deleteMapItem(&mapItem);            
        }
        pos += previous;
        previous = pos;  
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], MAP_ITEM, &pos);
    }
    if (t) {delete [] t; t = NULL;}
    return list;   
}

Map* Parser::getMap(char* xml) {
    Map* ret = NULL;

    CmdID*     cmdID  = NULL;
    Cred*      cred   = NULL;
    Meta*      meta   = NULL;
    
    Target*    target = NULL;
    Source*    source = NULL;
    ArrayList* mapItems; //MapItem[]
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID   = getCmdID(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml, META,    NULL);
    meta    = getMeta(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred    = getCred(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, TARGET,NULL);
    target   = getTarget(t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, SOURCE,  NULL);
    source   = getSource(t);
    if (t) {delete [] t; t = NULL;}
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
ArrayList* Parser::getCopies(char* xml, char* except) {
    
    Copy* copy = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = NULL;
    
   /*
    * except is set to SYNC if we are looking for Copy commands external from <sync> tag
    */
    char* t = NULL;
    t = XMLProcessor::getElementContentExcept(&xml[pos], COPY, except, &pos);
    while ((copy = getCopy(t)) != NULL) {
        if (copy) {
            if (!list)
                list = new ArrayList();
            list->add(*copy); // in the ArrayList NULL element cannot be inserted
            deleteCopy(&copy);            
        }
        pos += previous;
        previous = pos; 
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentExcept(&xml[pos], COPY, except, &pos);
    }  
    if (t) {delete [] t; t = NULL;}
    return list;   
}

/*
* Returns an ArrayList of add command
*/
ArrayList* Parser::getAdds(char* xml, char* except) {
    
    Add* add         = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list  = NULL;
    char* t         = NULL;
   /*
    * except is set to SYNC if we are looking for Add commands external from <sync> tag
    */
    t = XMLProcessor::getElementContentExcept(&xml[pos], ADD, except, &pos);
    while ((add = getAdd(t)) != NULL) {
        if (add) {
            if (!list)
                list = new ArrayList();
            list->add(*add); // in the ArrayList NULL element cannot be inserted
            deleteAdd(&add);            
        }
        pos += previous;
        previous = pos;                
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentExcept(&xml[pos], ADD, except, &pos);
    }   
    if (t) { delete [] t; t = NULL; }
    return list;   
}

/*
* Returns an ArrayList of Replace commands
*/
ArrayList* Parser::getReplaces(char* xml, char* except) {
    
    Replace* replace = NULL;    
    unsigned int pos = 0, previous = 0;
    ArrayList* list = NULL;
    char* t        = NULL;

    t = XMLProcessor::getElementContentExcept(&xml[pos], REPLACE, except, &pos);
    while ((replace = getReplace(t)) != NULL) {
        if (replace) {
            if (!list)
                list = new ArrayList();
            list->add(*replace); // in the ArrayList NULL element cannot be inserted
            deleteReplace(&replace);            
        }
        pos += previous;
        previous = pos;                
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentExcept(&xml[pos], REPLACE, except, &pos);
    }
    if (t) { delete [] t; t = NULL; }
    return list;   
}

/*
* Returns an ArrayList of Dels command
*/
ArrayList* Parser::getDels(char* xml, char* except) {
    
    Delete* del        = NULL;    
    unsigned int pos   = 0, previous = 0;
    ArrayList* list    = NULL;
    char* t           = NULL;

    t = XMLProcessor::getElementContentExcept(&xml[pos], DEL, except, &pos);
    while ((del = getDelete(t)) != NULL) {
        if (del) {
            if (!list)
                list = new ArrayList();
            list->add(*del); // in the ArrayList NULL element cannot be inserted
            deleteDelete(&del);            
        }
        pos += previous;
        previous = pos;                
        if (t) { delete [] t; t = NULL;}
        t = XMLProcessor::getElementContentExcept(&xml[pos], DEL, except, &pos);
    }
    if (t) { delete [] t; t = NULL; }
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
ArrayList* Parser::getCommands(char* xml) {    
    ArrayList* ret    = new ArrayList();
    
    ArrayList* list     = NULL;
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
    char* t            = NULL;


    // Status
    t = XMLProcessor::getElementContent(&xml[pos], STATUS, &pos);
    while ((status = getStatus(t)) != NULL) {
        if (status) {
            if (!list)
                list = new ArrayList();
            list->add(*status); // in the ArrayList NULL element cannot be inserted
            deleteStatus(&status);            
        }
        pos += previous;
        previous = pos;                
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], STATUS, &pos);
    }
    if (t) { delete [] t; t = NULL; }
   
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);    
        
    // Alert: use the getElementContentLevel because Alert could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos);
    while ((alert = getAlert(t)) != NULL) {
        if (alert) {
            if (!list)
                list = new ArrayList();
            list->add(*alert); // in the ArrayList NULL element cannot be inserted
            deleteAlert(&alert);            
        }
        pos += previous;
        previous = pos;       
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], ALERT, &pos);
    }
    if (t) { delete [] t; t = NULL; }

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Map: use the getElementContentLevel because Map could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos);
    while ((map = getMap(t)) != NULL) {
        if (map) {
            if (!list)
                list = new ArrayList();
            list->add(*map); // in the ArrayList NULL element cannot be inserted
            deleteMap(&map);            
        }
        pos += previous;
        previous = pos;   
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], MAP, &pos);
    }
    if (t) { delete [] t; t = NULL; }
    
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        

    deleteArrayList(&list);
    
    // Get: use the getElementContentLevel because Get could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContent(&xml[pos], GET, &pos);
    while ((get = getGet(t)) != NULL) {
        if (get) {
            if (!list)
                list = new ArrayList();
            list->add(*get); // in the ArrayList NULL element cannot be inserted
            deleteGet(&get);            
        }
        pos += previous;
        previous = pos;   
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], GET, &pos);
    }
    if (t) { delete [] t; t = NULL; }

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Put
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContent(&xml[pos], PUT, &pos);
    while ((put = getPut(t)) != NULL) {
        if (put) {
            if (!list)
                list = new ArrayList();
            list->add(*put); // in the ArrayList NULL element cannot be inserted
            deletePut(&put);            
        }
        pos += previous;
        previous = pos;   
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], PUT, &pos);
    }
    if (t) { delete [] t; t = NULL; }
    
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Results
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContent(&xml[pos], RESULTS, &pos);
    while ((result = getResult(t)) != NULL) {
        if (result) {
            if (!list)
                list = new ArrayList();
            list->add(*result); // in the ArrayList NULL element cannot be inserted
            deleteResults(&result);            
        }
        pos += previous;
        previous = pos;         
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], RESULTS, &pos);
    }
    if (t) { delete [] t; t = NULL; }

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Exec: use the getElementContentLevel because Exec could be also in Atomic and Sequence commands
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos);
    while ((exec = getExec(t)) != NULL) {
        if (exec) {
            if (!list)
                list = new ArrayList();
            list->add(*exec); // in the ArrayList NULL element cannot be inserted
            deleteExec(&exec);            
        }
        pos += previous;
        previous = pos;     
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentLevel(&xml[pos], EXEC, &pos);
    }
    if (t) { delete [] t; t = NULL; }
    
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // Search
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContent(&xml[pos], SEARCH, &pos);
    while ((search = getSearch(t)) != NULL) {
        if (search) {
            if (!list)
                list = new ArrayList();
            list->add(*search); // in the ArrayList NULL element cannot be inserted
            deleteSearch(&search);            
        }
        pos += previous;
        previous = pos;              
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], SEARCH, &pos);
    }
    if (t) { delete [] t; t = NULL; }

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);

    // get the Sync commands. not belonging to Atomic and Sequence
    //sync = getSync(XMLProcessor::getElementContentExcept (xml, SYNC, T("Atomic&Sequence"), NULL));
    
    //if (sync) {
    //    ret->add(*sync);
    //    deleteSync(&sync);
    //}
    
    // Sync
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContentExcept(&xml[pos], SYNC, T("Atomic&Sequence"), &pos);
    while ((sync = getSync(t)) != NULL) {
        if (sync) {
            if (!list)
                list = new ArrayList();
            list->add(*sync); // in the ArrayList NULL element cannot be inserted
            deleteSync(&sync);
        }
        pos += previous;
        previous = pos;     
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContentExcept(&xml[pos], SYNC, T("Atomic&Sequence"), &pos);
    }
    if (t) { delete [] t; t = NULL; }
    
    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }        
    deleteArrayList(&list);



    // get the Sequence commands. Not belonging to Atomic and Sync
    t = XMLProcessor::getElementContentExcept(xml, SEQUENCE, T("Atomic&Sync"), &pos);
    sequence = getSequence(t);
    if (t) {delete [] t; t = NULL;}
    
    if (sequence) {
        ret->add(*sequence);
        deleteSequence(&sequence);
    }
    
    // get the Sequence commands. Not belonging to Sequence and Sync and Atomic
    t = XMLProcessor::getElementContentExcept(xml, ATOMIC, T("Atomic&Sync&Sequence"), &pos);
    atomic = getAtomic(t);   
    if (t) {delete [] t; t = NULL;}
    
    if (atomic) {
        ret->add(*atomic);
        deleteAtomic(&atomic);
    }
    
    
    list = getCommonCommandList(xml, T("Atomic&Sync&Sequence"));

    if (list && list->size() > 0) {
        for (int i = 0; i < list->size(); i++) {
            ret->add(*list->get(i));
        }
    }
    deleteArrayList(&list);
    
    return ret;
}

Status* Parser::getStatus(char* xml) {
    
    if (!xml)
        return NULL;
    
    Status*     ret         = NULL;
    
    CmdID*      cmdID       = NULL;
    char*    msgRef      = NULL;
    char*    cmdRef      = NULL;
    char*    cmd         = NULL;
    ArrayList*  targetRefs  = new ArrayList();
    ArrayList*  sourceRefs  = new ArrayList();
    Cred*       cred        = NULL;
    Chal*       chal        = NULL;
    Data*       data        = NULL;
    ArrayList*  items       = new ArrayList();

    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID = getCmdID(t);
    if (t) {delete [] t; t = NULL;}
    
    msgRef          = XMLProcessor::getElementContent (xml, MSG_REF, NULL);
    cmdRef          = XMLProcessor::getElementContent (xml, CMD_REF, NULL);
    cmd             = XMLProcessor::getElementContent (xml, CMD,     NULL);
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred            = getCred      (t);
    if (t) {delete [] t; t = NULL;}
    // get Data <Data>200</Data>
    t = XMLProcessor::getElementContent (xml, DATA   , NULL);
    data            = getData      (t);
    if (t) {delete [] t; t = NULL;}
    items           = getItems     (xml);
    targetRefs      = getTargetRefs(xml);
    sourceRefs      = getSourceRefs(xml);
    t = XMLProcessor::getElementContent (xml, CHAL   , NULL);
    chal            = getChal      (t);    
    if (t) {delete [] t; t = NULL;}
    
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

Chal* Parser::getChal(char* xml) {
    
    Chal* ret  = NULL;
    Meta* meta = NULL;

    meta     = getMeta (xml);    

    if (meta) {
        ret = new Chal(meta);
        deleteMeta(&meta); 
    }

    return ret;
}

ArrayList* Parser::getTargetRefs(char* xml) {
    ArrayList* list = new ArrayList();
    TargetRef* targetRef = NULL;
    unsigned int pos = 0, previous = 0;

    char* t = NULL;
    t = XMLProcessor::getElementContent(&xml[pos], TARGET_REF, &pos);
    while ((targetRef = getTargetRef(t)) != NULL) {
        if (targetRef) {
            list->add(*targetRef); // in the ArrayList NULL element cannot be inserted
            deleteTargetRef(&targetRef);            
        }
        pos += previous;
        previous = pos;               
        if (t) {delete [] t; t = NULL;}
        t = XMLProcessor::getElementContent(&xml[pos], TARGET_REF, &pos);
    }
    if (t) {delete [] t; t = NULL;}
    
    return list;
}

ArrayList* Parser::getSourceRefs(char* xml) {
    ArrayList* list = new ArrayList();
    SourceRef* sourceRef = NULL;
    unsigned int pos = 0, previous = 0;

    char* t = NULL;
    t = XMLProcessor::getElementContent(&xml[pos], SOURCE_REF, &pos);
    while ((sourceRef = getSourceRef(t)) != NULL) {
        if (sourceRef) {
            list->add(*sourceRef); // in the ArrayList NULL element cannot be inserted
            deleteSourceRef(&sourceRef);            
        }
        pos += previous;
        previous = pos;               
        if (t) {delete [] t; t = NULL;}
        t = XMLProcessor::getElementContent(&xml[pos], SOURCE_REF, &pos);
    }
    if (t) {delete [] t; t = NULL;}
    return list;
}

SourceRef* Parser::getSourceRef(char* xml) {    
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

TargetRef* Parser::getTargetRef(char* xml) {    
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

Alert* Parser::getAlert(char* xml) {
    
    Alert* ret = NULL;
    ArrayList* items = new ArrayList();    
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    CmdID* cmdID     = getCmdID   (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    Cred*  cred      = getCred    (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, DATA   , NULL);
    int    data      = getDataCode(t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    BOOL   noResp    = getNoResp  (t);
    if(t) {delete [] t; t = NULL;}
    
    items = getItems(xml);
    if (items && items->size() > 0) {
        ret = new Alert(cmdID, noResp, cred, data, items); //Item[]
    }
    
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteArrayList(&items);
    
    return ret;
}

Exec* Parser::getExec(char* xml) {
    
    Exec* ret = NULL;
    
    CmdID* cmdID        = NULL;
    BOOL   noResp       = FALSE;
    Cred*  cred         = NULL;
    ArrayList*  items   = new ArrayList();

    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID     = getCmdID   (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred      = getCred    (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp    = getNoResp  (t);    
    if(t) {delete [] t; t = NULL;}
    items = getItems(xml);

    if (cmdID || NotZeroArrayLenght(1, items) || (cred)) {
        ret = new Exec(cmdID, noResp, cred, items); 
    }
    
    deleteCmdID(&cmdID);
    deleteCred(&cred);
    deleteArrayList(&items);
    
    return ret;
}

Get* Parser::getGet(char* xml) {
       
    Get* ret = NULL;    
    ArrayList* items = NULL;    
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    CmdID* cmdID     = getCmdID   (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    Cred*  cred      = getCred    (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    BOOL   noResp    = getNoResp  (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml, META ,   NULL);
    Meta*  meta      = getMeta    (t);
    if(t) {delete [] t; t = NULL;}    
    char* lang    = XMLProcessor::getElementContent        (xml, LANG, NULL);
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

Put* Parser::getPut(char* xml) {
    
    Put* ret = NULL;    
    ArrayList* items = NULL;    
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    CmdID* cmdID     = getCmdID   (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    Cred*  cred      = getCred    (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    BOOL   noResp    = getNoResp  (t);
    if(t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel (xml, META ,   NULL);
    Meta*  meta      = getMeta    (t);
    if(t) {delete [] t; t = NULL;}
    char* lang    = XMLProcessor::getElementContent        (xml, LANG, NULL);        
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

Search* Parser::getSearch(char* xml) {    
    
    Search*     ret      = NULL;
    CmdID*      cmdID    = NULL;
    BOOL        noResp   = NULL;
    BOOL        noResults= NULL;
    Cred*       cred     = NULL;
    Target*     target   = NULL;
    ArrayList*  sources  = new ArrayList();  // an Array of SourceArray object
    char*    lang     = NULL;
    Meta*       meta     = NULL;
    Data*       data     = NULL;
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID     = getCmdID   (t);
    if (t) { delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, CRED   , NULL);
    cred      = getCred    (t);
    if (t) { delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, NO_RESP, NULL);
    noResp    = getNoResp  (t);
    if (t) { delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, NO_RESULTS, NULL);
    noResults = getNoResults(t);
    if (t) { delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent (xml, TARGET,NULL);
    target    = getTarget  (t);
    if (t) { delete [] t; t = NULL;}    
    lang      = XMLProcessor::getElementContent             (xml, LANG, NULL);            
    t = XMLProcessor::getElementContentLevel (xml, META , NULL);
    meta      = getMeta    (t);
    if (t) { delete [] t; t = NULL;}    
    t = XMLProcessor::getElementContent (xml, DATA ,   NULL);
    data      = getData    (t);    
    if (t) { delete [] t; t = NULL;}    
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

Results* Parser::getResult(char* xml) {
    
    if (!xml)
        return NULL;
    
    Results*    ret         = NULL;        
    CmdID*      cmdID       = NULL;
    char*    msgRef      = NULL;
    char*    cmdRef      = NULL;
    Meta*       meta        = NULL;   
    ArrayList*  targetRefs  = new ArrayList(); // it could be an array with only a value
    ArrayList*  sourceRefs  = new ArrayList();
    ArrayList*  items       = new ArrayList();

    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent (xml, CMD_ID , NULL);
    cmdID           = getCmdID     (t);
    if (t) {delete [] t; t = NULL;}    
    msgRef          = XMLProcessor::getElementContent (xml, MSG_REF, NULL);
    cmdRef          = XMLProcessor::getElementContent (xml, CMD_REF, NULL);
    t = XMLProcessor::getElementContentLevel(xml, META, NULL);
    meta            = getMeta      (t);
    if (t) {delete [] t; t = NULL;}
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
ArrayList* Parser::getItems(char* xml) {

    Item* item = NULL;
    ArrayList* items = NULL;
    unsigned int pos = 0, previous = 0;
    char*      t       = NULL;
    
    t = XMLProcessor::getElementContent(&xml[pos], ITEM, &pos);    
    while ((item = getItem(t)) != NULL) {
        if (item) {
            if (!items)
                items = new ArrayList();
            items->add(*item);    // in the ArrayList NULL element cannot be inserted
            deleteItem(&item);    
        }
        pos += previous;
        previous = pos;                
        if (t) { delete [] t; t = NULL; }
        t = XMLProcessor::getElementContent(&xml[pos], ITEM, &pos);
    }
    if (t) { delete [] t; t = NULL; }

    return items;    
}

Item* Parser::getItem(char* xml) {    
    Item*   ret       = NULL;
    Target* target    = NULL;
    Source* source    = NULL;
    Meta*   meta      = NULL;
    ComplexData* data = NULL;
    BOOL moreData     = FALSE;  
    char* targetParent = NULL;
    char* sourceParent = NULL;
    char*      t       = NULL;   
    unsigned int pos = 0;
    t = XMLProcessor::getElementContent(xml, TARGET,NULL);
    target   = getTarget     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContent(xml, SOURCE,NULL);
    source   = getSource     (t);
    if (t) {delete [] t; t = NULL;}
    t = XMLProcessor::getElementContentLevel(xml, META,NULL);
    meta     = getMeta       (t);
    if (t) {delete [] t; t = NULL;}
    
    t = XMLProcessor::getElementContent(xml, COMPLEX_DATA,NULL);
    data     = getComplexData(t);
    if (t) { delete [] t; t = NULL; } 
    
    t = XMLProcessor::getElementContent(xml, MORE_DATA,NULL);
    moreData = getMoreData   (t);
    if (t) {delete [] t; t = NULL;}
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

int Parser::getDataCode(char* content) {    
   int ret = 0;
   if (content) {
        ret = strtol(content, NULL, 10);
   }
   return ret;
}

Data* Parser::getData(char* content) {    
   Data* ret = 0;
   if (content) {
        ret = new Data(content);
   }
   return ret;
}

BOOL Parser::getFinalMsg(char* content) {    
    BOOL ret = FALSE;
    if (content) {
        ret = TRUE;
    }
    return ret;
}

CmdID* Parser::getCmdID(char* content) {
    CmdID* ret = NULL;
    if (content) {
        ret = new CmdID(content);
    }
    return ret;
}

ComplexData* Parser::getComplexData(char* xml) {
    
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

DevInf* Parser::getDevInf(char* xml) {
    DevInf* ret             = NULL;
    DataStore* dataStore    = NULL;
    CTCap* ctCap            = NULL;
    Ext* ext                = NULL;

    VerDTD* verDTD          = NULL;
    char* man            = NULL;
    char* mod            = NULL;
    char* oem            = NULL;
    char* fwV            = NULL;
    char* swV            = NULL;
    char* hwV            = NULL;
    char* devId          = NULL;
    char* devTyp         = NULL;
    ArrayList* dataStores   = NULL;       // DataStore[]
    ArrayList* ctCaps       = NULL;       // CTCap[]
    ArrayList* exts         = NULL;       // Ext[]
    BOOL utc                = NULL;         // if present they Support UTC
    BOOL supportLargeObjs   = NULL;         // if present they Support largeObject
    BOOL supportNumberOfChanges = NULL;     // if present they Support NumberOfChanges
    SyncCap* syncCap        = NULL;
    
    char* value          = NULL;
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent(xml, VER_DTD,NULL);
    verDTD = getVerDTD(t);
    if (t) {delete [] t; t = NULL;}
    man     = XMLProcessor::getElementContent(xml, MAN,           NULL);
    mod     = XMLProcessor::getElementContent(xml, MOD,           NULL);
    oem     = XMLProcessor::getElementContent(xml, OEM,           NULL);
    fwV     = XMLProcessor::getElementContent(xml, FWV,           NULL);
    swV     = XMLProcessor::getElementContent(xml, SWV,           NULL);
    hwV     = XMLProcessor::getElementContent(xml, HWV,           NULL);
    devId   = XMLProcessor::getElementContent(xml, DEV_ID,        NULL);
    devTyp  = XMLProcessor::getElementContent(xml, DEV_TYP,       NULL);

    t = XMLProcessor::getElementContent(xml, SYNC_CAP,NULL);
    syncCap = getSyncCap(t);
    if (t) {delete [] t; t = NULL;}
    
    unsigned int previous = 0;
    pos = 0; 
        
    // DataStore
    t = XMLProcessor::getElementContent(&xml[pos], DATA_STORE, &pos);
    while ((dataStore = getDataStore(t)) != NULL) {
        if (dataStore) {
            if (!dataStores) {
                dataStores = new ArrayList();                
            }
            dataStores->add(*dataStore); // in the ArrayList NULL element cannot be inserted
            deleteDataStore(&dataStore);            
        }
        pos += previous;
        previous = pos;            
        if (t) {delete [] t; t = NULL;}
        t = XMLProcessor::getElementContent(&xml[pos], DATA_STORE, &pos);
    }
    if (t) {delete [] t; t = NULL;}
        
    // ctCap
    pos = 0; previous = 0;
    t = XMLProcessor::getElementContent(&xml[pos], CT_CAP, &pos);
    while ((ctCap = getCTCap(t)) != NULL) {
        if (ctCap) {
            if (!ctCaps) {
                ctCaps = new ArrayList();                
            }
            ctCaps->add(*ctCap); // in the ArrayList NULL element cannot be inserted
            deleteCTCap(&ctCap);            
        }
        pos += previous;
        previous = pos;       
        if (t) {delete [] t; t = NULL;}
        t = XMLProcessor::getElementContent(&xml[pos], CT_CAP, &pos);
    }
    if (t) {delete [] t; t = NULL;}

    // ext
    pos = 0; previous = 0;
    t = XMLProcessor::getElementContent(&xml[pos], EXT, &pos);
    while ((ext = getExt(t)) != NULL) {
        if (ext) {
            if (!exts) {
                exts = new ArrayList();                
            }
            exts->add(*ext); // in the ArrayList NULL element cannot be inserted
            deleteExt(&ext);            
        }
        pos += previous;
        previous = pos;   
        if (t) {delete [] t; t = NULL;}
        t = XMLProcessor::getElementContent(&xml[pos], EXT, &pos);
    }
    if (t) {delete [] t; t = NULL;}

    //
    // The large object value depends on SUPPORT_LARGE_OBJECT tag.
    //
    if ((value = XMLProcessor::getElementContent(xml, SUPPORT_LARGE_OBJECT, NULL)) != NULL) {
        if (wcscmpIgnoreCase(value, T("TRUE"))) {
            supportLargeObjs = TRUE;
        }
        safeDel(&value);
    }

    //
    // The large object value depends on SUPPORT_NUMBER_OF_CHANGES tag. 
    //    
    if ((value = XMLProcessor::getElementContent(xml, SUPPORT_NUMBER_OF_CHANGES, NULL)) != NULL) {
        if (wcscmpIgnoreCase(value, T("TRUE"))) {
            supportNumberOfChanges = TRUE;
        }
        safeDel(&value);
    }

    //
    // The large object value depends on UTC tag.
    //
    if ((value = XMLProcessor::getElementContent(xml, UTC, NULL)) != NULL) {
        if (wcscmpIgnoreCase(value, T("TRUE"))) {
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
Ext* Parser::getExt(char* xml) {
    Ext* ret = NULL;
    char* XNam         = NULL;
    char* value        = NULL;
    ArrayList* list     = NULL;
    StringElement* s    = NULL;
    unsigned int pos = 0, previous = 0;

    XNam = XMLProcessor::getElementContent(xml, XNAM, NULL);

    // XVal
    while ((value = XMLProcessor::getElementContent(&xml[pos], XVAL, &pos)) != NULL) {
        if (value) {
            if (!list)
                list = new ArrayList();
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

DataStore* Parser::getDataStore(char* xml) {
    DataStore* ret = NULL;
    
    SourceRef*       sourceRef      = NULL;
    char*           displayName    = NULL;
    long             maxGUIDSize    = 0;
    char*           maxGUIDSizeW   = NULL;
    ContentTypeInfo* rxPref         = NULL;
    ArrayList*       rx             = NULL; // ContentTypeInfo[]
    ContentTypeInfo* txPref         = NULL;;
    ArrayList*       tx             = NULL; // ContentTypeInfo[]
    DSMem*           dsMem          = NULL;
    SyncCap*         syncCap        = NULL;  

    ContentTypeInfo* x              = NULL;
    
    unsigned int pos = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent(xml, SOURCE_REF,  NULL);
    sourceRef   = getSourceRef(t);
    if (t) {delete [] t; t = NULL;}    
    displayName = XMLProcessor::getElementContent(xml, DISPLAY_NAME,             NULL);
    maxGUIDSizeW = XMLProcessor::getElementContent(xml, MAX_GUID_SIZE,           NULL);
    if (maxGUIDSizeW) {
        maxGUIDSize = strtol(maxGUIDSizeW, NULL, 10);
    }
    t = XMLProcessor::getElementContent(xml, RX_PREF,  NULL);
    rxPref = getContentTypeInfo(t);
    if (t) {delete [] t; t = NULL;}    
    t = XMLProcessor::getElementContent(xml, TX_PREF,  NULL);
    txPref = getContentTypeInfo(t);
    if (t) {delete [] t; t = NULL;}    

    unsigned int previous = 0;
    pos = 0;
        
    // Rx 
    t = XMLProcessor::getElementContent(&xml[pos], RX, &pos);
    while ((x = getContentTypeInfo(t)) != NULL) {
        if (x) {
            if (!rx)
                rx = new ArrayList();
            rx->add(*x); // in the ArrayList NULL element cannot be inserted
            deleteContentTypeInfo(&x);            
        }
        pos += previous;
        previous = pos;             
        if (t) {delete [] t; t = NULL;}    
        t = XMLProcessor::getElementContent(&xml[pos], RX, &pos);
    }
    if (t) {delete [] t; t = NULL;}    

    // Tx 
    pos = 0, previous = 0;
    t = XMLProcessor::getElementContent(&xml[pos], TX, &pos);
    while ((x = getContentTypeInfo(t)) != NULL) {
        if (x) {
            if (!tx)
                tx = new ArrayList();
            tx->add(*x); // in the ArrayList NULL element cannot be inserted
            deleteContentTypeInfo(&x);            
        }
        pos += previous;
        previous = pos;             
        if (t) {delete [] t; t = NULL;}    
        t = XMLProcessor::getElementContent(&xml[pos], TX, &pos);
    }
    if (t) {delete [] t; t = NULL;}    
    
    t = XMLProcessor::getElementContent(xml, DS_MEM,  NULL);
    dsMem = getDSMem(t);    
    if (t) {delete [] t; t = NULL;}    
    t = XMLProcessor::getElementContent(xml, SYNC_CAP,  NULL);
    syncCap = getSyncCap(t);  
    if (t) {delete [] t; t = NULL;}    
        
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


SyncCap* Parser::getSyncCap(char* xml) {
    
    SyncCap* ret            = NULL;
    SyncType* syncType      = NULL;
    ArrayList* list         = NULL;

    unsigned int pos = 0, previous = 0;
    char* t = NULL;
    t = XMLProcessor::getElementContent(&xml[pos], SYNC_TYPE, &pos);
    while ((syncType = getSyncType(t)) != NULL) {
        if (syncType) {
            if (!list)
                list = new ArrayList();
            list->add(*syncType); // in the ArrayList NULL element cannot be inserted
            deleteSyncType(&syncType);            
        }
        pos += previous;
        previous = pos;             
        if (t) {delete [] t; t = NULL;}
        t = XMLProcessor::getElementContent(&xml[pos], SYNC_TYPE, &pos);
    }
    if (t) {delete [] t; t = NULL;}
        
    if (NotZeroArrayLenght(1, list)) {
        ret = new SyncCap(list);
    }

    deleteArrayList(&list);

    return ret;
}

SyncType* Parser::getSyncType(char* content) {
    
    SyncType* ret            = NULL;    
    int value                = 0;
  
    if (content) {
         value = strtol(content, NULL, 10);
         if (value >= 1 && value <= 7) {
             ret = new SyncType(value);
         }
    }    
    
    return ret;
}


ContentTypeInfo* Parser::getContentTypeInfo(char* xml) {
    
    ContentTypeInfo* ret = NULL;
    char* ctType      = NULL;
    char* verCT       = NULL;     

    ctType = XMLProcessor::getElementContent(xml, CT_TYPE,             NULL);
    verCT  = XMLProcessor::getElementContent(xml, VER_CT,             NULL);
    
    if (NotNullCheck(2, ctType, verCT)) {
        ret = new ContentTypeInfo(ctType, verCT);
    }
    
    safeDel(&ctType);
    safeDel(&verCT);
    return ret;
}

DSMem* Parser::getDSMem(char* xml) {    
    
    DSMem* ret          = NULL;
    char* maxMemW   = NULL;   
    char* sharedMemW = NULL;
    char* maxIDW    = NULL;
        
    BOOL    sharedMem   = NULL;
    long    maxMem     = 0;
    long    maxID      = 0;
    
    BOOL isToCreate = FALSE;

    maxMemW     = XMLProcessor::getElementContent (xml, MAX_MEM,   NULL);
    sharedMemW  = XMLProcessor::getElementContent (xml, SHARED_MEM, NULL);
    maxIDW      = XMLProcessor::getElementContent (xml, MAX_ID,    NULL);
    
    isToCreate = NotNullCheck(3, maxMemW, sharedMemW, maxIDW);

    if (maxMemW != NULL) {
        maxMem = strtol(maxMemW, NULL, 10);
    }
    if (maxIDW != NULL) {
        maxID = strtol(maxIDW, NULL, 10);
    } 
    if (sharedMemW != NULL) {
        sharedMem = strtol(sharedMemW, NULL, 10);
    }
    
    if (isToCreate) {
        ret = new DSMem(sharedMem, maxMem, maxID);
    }

    safeDel(&maxMemW);
    safeDel(&maxIDW);
    safeDel(&sharedMemW);

    return ret;   

}

BOOL Parser::getNoResp(char* content) {    
  if (content)
      return TRUE;
  else
      return FALSE;
}

BOOL Parser::getNoResults(char* content) {    
    
    if (content)
        return TRUE;
    else    
        return FALSE;
}

BOOL Parser::getMoreData(char* content) {
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
CTCap* Parser::getCTCap(char* xml) {
    CTCap* ret = NULL;
    CTTypeSupported* ctTypeSupported = NULL;

    // ArrayList* ctTypes = new ArrayList();       
       
    return ret;
}

//
// TBD 
//
ArrayList* Parser::getEMI(char* content) {
    ArrayList* ret = NULL;    
    return ret;
}
