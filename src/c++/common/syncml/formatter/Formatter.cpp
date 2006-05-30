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
 
#include "syncml/formatter/Formatter.h"
#include "base/Log.h"

#define EMPTY_VALUE  T("__EMPTY__")

/*
* Returns a StringBuffer giving the tag and the value as long. To use for generic simple value
*/
StringBuffer* Formatter::getValue(BCHAR* tagName, long value) {    
    if (!value)
        return NULL;

    BCHAR* t1 = new BCHAR[bstrlen(tagName) + 3]; // <  >  0
    BCHAR* t2 = new BCHAR[bstrlen(tagName) + 5]; // </ > \n 0
    bsprintf(t1, T("<%s>"), tagName);
    bsprintf(t2, T("</%s>\n"), tagName);

    StringBuffer* s = new StringBuffer();
    s->append(t1);       
    s->append(value);
    s->append(t2);    
    safeDel(&t1);
    safeDel(&t2);

    return s;    
}

/*
* Returns a StringBuffer giving the tag and the value as BOOL. If true return only the tag, nothing otherwise
*/
StringBuffer* Formatter::getValue(BCHAR* tagName, BOOL value) {    
    if (!value)
        return NULL;

    BCHAR* t1 = new BCHAR[bstrlen(tagName) + 3]; // <  >  0
    BCHAR* t2 = new BCHAR[bstrlen(tagName) + 5]; // </ > \n 0
    bsprintf(t1, T("<%s>"), tagName);
    bsprintf(t2, T("</%s>\n"), tagName);

    StringBuffer* s = new StringBuffer();
    s->append(t1);        
    s->append(t2);    
    safeDel(&t1);
    safeDel(&t2);

    return s;    
}


/*
* Returns a StringBuffer giving the tag and the value as wchar. To use for generic simple value
*/
StringBuffer* Formatter::getValue(BCHAR* tagName, const BCHAR* value) {    
    return getValue(tagName, (BCHAR*) value);
}


/*
* Returns a StringBuffer giving the tag and the value as wchar.
* To use for generic simple value
*/
StringBuffer* Formatter::getValue(BCHAR* tagName, BCHAR* value) {    
    if (!value)
        return NULL;

    BCHAR* t1 = new BCHAR[bstrlen(tagName) + 3]; // <  >  0
    BCHAR* t2 = new BCHAR[bstrlen(tagName) + 5]; // </ > \n 0
    bsprintf(t1, T("<%s>"), tagName);
    bsprintf(t2, T("</%s>\n"), tagName);

    StringBuffer* s = new StringBuffer(t1);

    if (bstrcmp(value, EMPTY_VALUE) != 0)
        s->append(value);

    s->append(t2);
    
    safeDel(&t1);
    safeDel(&t2);

    return s;    
}

/*
* Returns a StringBuffer giving the tag and the value as StringBuffer. To use to include other stuffs
*/
StringBuffer* Formatter::getValue(BCHAR* tagName, StringBuffer* value) {    
    if (!value)
        return NULL;

    BCHAR* t1 = new BCHAR[bstrlen(tagName) + 3]; // <  >  0
    BCHAR* t2 = new BCHAR[bstrlen(tagName) + 5]; // </ > \n 0
    bsprintf(t1, T("<%s>"), tagName);
    bsprintf(t2, T("</%s>\n"), tagName);

    StringBuffer* s = new StringBuffer();
    s->append(t1);
    s->append(value);
    s->append(t2);
    
    safeDel(&t1);
    safeDel(&t2);

    return s;    
}


StringBuffer* Formatter::getSyncML(SyncML* syncML) {
        
    StringBuffer*   sBody = NULL;
    StringBuffer*   sHdr  = NULL;
    StringBuffer*   sML   = NULL; 
    
    if (syncML == NULL) {
        return NULL;
    }

    sHdr  = getSyncHdr (syncML->getSyncHdr ());
    sBody = getSyncBody(syncML->getSyncBody());
    
    sML = new StringBuffer(T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
    sML->append(T("<SyncML>\n"));    
    sML->append(sHdr);
    sML->append(sBody);
    sML->append(T("</SyncML>"));
    
    deleteAllStringBuffer(2,&sHdr, &sBody);

    return sML;     
}

StringBuffer* Formatter::getSyncHdr(SyncHdr* syncHdr) {
    
    StringBuffer*   ret = NULL;    
    StringBuffer*   s = NULL;    
           
    StringBuffer* sessionID = NULL;
    StringBuffer* verDTD    = NULL;
    StringBuffer* verProto  = NULL;
    StringBuffer* source    = NULL;
    StringBuffer* target    = NULL;    
    StringBuffer* cred      = NULL;
    StringBuffer* msgID     = NULL;
    StringBuffer* respURI   = NULL;
    StringBuffer* meta      = NULL;
    
    sessionID = getSessionID(syncHdr->getSessionID());
    verDTD    = getVerDTD   (syncHdr->getVerDTD());
    verProto  = getVerProto (syncHdr->getVerProto());
    source    = getSource   (syncHdr->getSource());
    target    = getTarget   (syncHdr->getTarget());    
    cred      = getCred     (syncHdr->getCred());
    msgID     = getValue    (MSG_ID,  syncHdr->getMsgID(NULL));
    respURI   = getValue    (RESP_URI, syncHdr->getRespURI(NULL));
    meta      = getMeta     (syncHdr->getMeta());
    
    if (NotZeroStringBufferLenght(9, sessionID, verDTD, verProto, source, target, cred, msgID, respURI, meta)) {
        s = new StringBuffer();
        s->append(verDTD);
        s->append(verProto);
        s->append(sessionID);
        s->append(msgID);
        s->append(target);
        s->append(source);
        s->append(respURI);
        s->append(cred);
        s->append(meta);        
    }
    
    ret = getValue(SYNC_HDR, s);
    
    deleteAllStringBuffer(10, &s, &sessionID, &verDTD, &verProto, &msgID, &respURI, &target, &source, &cred, &meta);

    return ret;
}

StringBuffer* Formatter::getCred(Cred* cred) {
     if (!cred)
        return NULL;
    
    StringBuffer* ret  = NULL;
    StringBuffer* auth = NULL;
    auth = getAuthentication(cred->getAuthentication());
            
    if (auth) { 
        ret = new StringBuffer();
        ret = getValue(CRED, auth);
    }        
    deleteStringBuffer(&auth);

    return ret;
}

StringBuffer* Formatter::getAuthentication(Authentication* auth) {
    
    if (!auth)
        return NULL;

    StringBuffer* ret          = NULL;
    StringBuffer* data         = new StringBuffer();
    StringBuffer* meta         = new StringBuffer();
    
    data = getValue(DATA, auth->getData(NULL));
    meta = getMeta(auth->getMeta());
    
    if (NotZeroStringBufferLenght(2, data, meta)) {
        ret = new StringBuffer();
        ret->append(meta);
        ret->append(data);
    }
    deleteAllStringBuffer(2, &meta, &data);
    
    return ret;
}

StringBuffer* Formatter::getMeta(Meta* meta) {
    
    if (!meta) 
        return NULL;    

    StringBuffer* ret        = NULL;
    StringBuffer* metInf     = NULL;    
    
    metInf = getMetInf(meta->getMetInf());            

    if (NotZeroStringBufferLenght(1, metInf)) {
        ret = getValue(META, metInf);  
    }    
    deleteStringBuffer(&metInf);    

    return ret;        
}

StringBuffer* Formatter::getMetInf(MetInf* metInf) {
    if (!metInf)
        return NULL;

    StringBuffer* ret       = NULL;    
    
    StringBuffer*     format     = NULL;
    StringBuffer*     type       = NULL;
    StringBuffer*     mark       = NULL;
    StringBuffer*     anchor     = NULL;
    StringBuffer*     version    = NULL;
    StringBuffer*     nextNonce  = NULL;
    StringBuffer*     maxMsgSize = NULL;
    StringBuffer*     maxObjSize = NULL;
    StringBuffer*     size       = NULL;
    //ArrayList*   emi        = NULL;
    StringBuffer*     mem        = NULL; 

    // get all the values

    format       = getValue(FORMAT, metInf->getFormat(NULL));
    type         = getValue(TYPE,   metInf->getType(NULL)); 
    mark         = getValue(MARK,   metInf->getMark(NULL));
    
    anchor       = getAnchor(metInf->getAnchor());
    version      = getValue(VERSIONSTR, metInf->getVersion(NULL)); 

    nextNonce    = getNextNonce(metInf->getNextNonce());
    
    maxMsgSize   = getValue(MAX_MESSAGE_SIZE, metInf->getMaxMsgSize());
    maxObjSize   = getValue(MAX_OBJ_SIZE, metInf->getMaxObjSize());
    size         = getValue(SIZE,         metInf->getSize());             
        
    //emi          = getEMI(xml);
    mem          = getMem(metInf->getMem());    
    
    if (NotZeroStringBufferLenght(8, format, type, mark, size, anchor, version, maxMsgSize, maxObjSize)) {
        ret = new StringBuffer();
        ret ->append(format);
        ret ->append(type);
        ret ->append(mark);
        ret ->append(size);
        ret ->append(anchor);
        ret ->append(version);
        ret ->append(nextNonce);
        ret ->append(maxMsgSize);
        ret ->append(maxObjSize);               
        ret ->append(mem);               
    }             
    deleteAllStringBuffer(9, &format, &type, &mark, &version, &maxMsgSize, &maxObjSize, &size, &nextNonce, &mem);
       
    return ret;    
}

StringBuffer* Formatter::getMem(Mem* mem) {    
    if (!mem)
        return NULL;

    StringBuffer* ret = new StringBuffer();                
    
    ret->append(getValue(SHARED_MEM, mem->getSharedMem()));
    ret->append(getValue(FREE_MEM,   mem->getFreeMem()));
    ret->append(getValue(FREE_ID,    mem->getFreeID()));
    
    return ret;    
}

StringBuffer* Formatter::getNextNonce(NextNonce* nextNonce) {    
    if (!nextNonce)
        return NULL;

    StringBuffer* ret = new StringBuffer();             
    ret = getValue(NEXT_NONCE, nextNonce->getValueAsBase64());
    return ret;    
}

StringBuffer* Formatter::getAnchor(Anchor* anchor) {    
    if (!anchor)
        return NULL;

    StringBuffer* ret = new StringBuffer();     

    ret->append(getValue(LAST,  anchor->getLast()));
    ret->append(getValue(NEXT,  anchor->getNext()));
       
    ret = getValue(ANCHOR, ret);    

    return ret;    
}

/*
* Returns a series of 
*  <Source> ... </Source> 
*  <Source> ... </Source> 
*  <Source> ... </Source> 
* 
*  use a SourceArray class type
*/
StringBuffer* Formatter::getSources(ArrayList* sources) {
    
    if (!sources || !NotZeroArrayLenght(1, sources))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < sources->size(); i++) {
        ret->append(getSourceArray(((SourceArray*)sources->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getSourceArray(SourceArray* sourceArray) {    
    if (!sourceArray)
        return NULL;

    StringBuffer* ret = new StringBuffer(); 
    StringBuffer* s   = new StringBuffer(); 

    s->append(getValue(LOC_URI,  sourceArray->getSource()->getLocURI()));
    s->append(getValue(LOC_NAME, sourceArray->getSource()->getLocName()));
    if (NotZeroStringBufferLenght(1, s)) {     
        ret = getValue(SOURCE, s);    
    }    
    
    deleteAllStringBuffer(1, &s);

    return ret;    
}


StringBuffer* Formatter::getSource(Source* source) {    
    if (!source)
        return NULL;

    StringBuffer* ret = new StringBuffer(); 
    StringBuffer* s   = new StringBuffer(); 

    s->append(getValue(LOC_URI,  source->getLocURI()));
    s->append(getValue(LOC_NAME, source->getLocName()));
    if (NotZeroStringBufferLenght(1, s)) {     
        ret = getValue(SOURCE, s);    
    }    
    
    deleteStringBuffer(&s);

    return ret;    
}

StringBuffer* Formatter::getTarget(Target* target) {    
    if (!target)
        return NULL;    

    StringBuffer* ret    = new StringBuffer();
    StringBuffer* s      = new StringBuffer();
    StringBuffer* filter = new StringBuffer();
        
    s->append(getValue(LOC_URI,  target->getLocURI()));
    s->append(getValue(LOC_NAME, target->getLocName()));

    //
    // And now the filter (if any)
    //
    if (target->getFilter()) {
        filter = getFilter(target->getFilter());
        s->append(filter);
    }
    
    if (NotZeroStringBufferLenght(1, s)) {        
        ret = getValue(TARGET, s);
        
    } 
    
    deleteAllStringBuffer(2, &s, &filter);

    return ret;    
}

StringBuffer* Formatter::getSessionID(SessionID* sessionID) {    
    if (!sessionID)
        return NULL;

    StringBuffer* s = new StringBuffer();
    s = getValue(SESSION_ID, sessionID->getSessionID(NULL));    
    return s;    
}

StringBuffer* Formatter::getVerDTD(VerDTD* verDTD) {    
    if (!verDTD)
        return NULL;

    StringBuffer* s = new StringBuffer();
    s = getValue(VER_DTD, verDTD->getValue(NULL));    
    return s;    
}

StringBuffer* Formatter::getCmdID(CmdID* cmdID) {    
    if (!cmdID)
        return NULL;

    StringBuffer* s = new StringBuffer();
    s = getValue(CMD_ID, cmdID->getCmdID());    
    return s;    
}

StringBuffer* Formatter::getVerProto(VerProto* verProto) {    
    if (!verProto)
        return NULL;

    StringBuffer* s = new StringBuffer();
    s = getValue(VER_PROTO, verProto->getVersion(NULL));
    return s;    
}

/*
* Used to retrieve the extra command list that are 
* Exec
* Alert
* Get
* Map
*/
StringBuffer* Formatter::getExtraCommandList(ArrayList* commands) {
    
    StringBuffer*   s               = NULL;    
    
    StringBuffer*   exec            = NULL;
    StringBuffer*   map             = NULL;
    StringBuffer*   alert           = NULL;
    StringBuffer*   get             = NULL;
    BCHAR*        name            = NULL;
    /*
    * Use the name of the command to get the proper action to invoke
    */
    for (int i = 0; i < commands->size(); i++) {
        name = ((AbstractCommand*)(commands->get(i)))->getName();    
        if (name && bstrcmp(name, EXEC) == 0) {
            if (!exec) {
                exec = new StringBuffer();
            }
            exec->append(getExec((Exec*)commands->get(i)));
        } else if (name && bstrcmp(name, ALERT) == 0) {
            if (!alert) {
                alert = new StringBuffer();
            }
            alert->append(getAlert((Alert*)commands->get(i)));
        } else if (name && bstrcmp(name, GET) == 0) {
            if (!get) {
                get = new StringBuffer();
            }
            get->append(getGet((Get*)commands->get(i)));
        } else if (name && bstrcmp(name, MAP) == 0) {
            if (!map) {
                map = new StringBuffer();
            }
            map->append(getMap((Map*)commands->get(i)));
        }
    }    

    if (NotZeroStringBufferLenght(4, exec, map, alert, get)) {
        s = new StringBuffer();
        s->append(exec);
        s->append(map);
        s->append(alert);
        s->append(get);        
    }    
    deleteAllStringBuffer(4, &exec, &map, &alert, &get);
    return s;   
}



/*
* Used to retrieve the common command list that are 
* Add
* Replace
* Copy
* Delete
*/
StringBuffer* Formatter::getCommonCommandList(ArrayList* commands) {
    
    StringBuffer*   s               = NULL;    
    
    StringBuffer*   adds            = NULL;
    StringBuffer*   dels            = NULL;
    StringBuffer*   replaces        = NULL;
    StringBuffer*   copies          = NULL;
    BCHAR*        name            = NULL;
    /*
    * Use the name of the command to get the proper action to invoke
    */
    for (int i = 0; i < commands->size(); i++) {
        name = ((AbstractCommand*)(commands->get(i)))->getName();    
        if (name && bstrcmp(name, COPY) == 0) {
            if (!copies) {
                copies = new StringBuffer();
            }
            copies->append(getCopy((Copy*)commands->get(i)));
        } else if (name && bstrcmp(name, ADD) == 0) {
            if (!adds) {
                adds = new StringBuffer();
            }
            adds->append(getAdd((Add*)commands->get(i)));
        } else if (name && bstrcmp(name, DEL) == 0) {
            if (!dels) {
                dels = new StringBuffer();
            }
            dels->append(getDelete((Delete*)commands->get(i)));
        } else if (name && bstrcmp(name, REPLACE) == 0) {
            if (!replaces) {
                replaces = new StringBuffer();
            }
            replaces->append(getReplace((Replace*)commands->get(i)));
        }
    }    

    if (NotZeroStringBufferLenght(4, copies, adds, replaces, dels)) {
        s = new StringBuffer();
        s->append(copies);
        s->append(adds);
        s->append(replaces);
        s->append(dels);        
    }    
    deleteAllStringBuffer(4, &copies, &adds, &replaces, &dels);
    return s;   
}

/*
* Used to retrieve a specific command like SYNC or ATOMIC or SEQUENCE
*/
StringBuffer* Formatter::getSpecificCommand(ArrayList* commands, BCHAR* commandName) {
    
    StringBuffer*   s               = NULL;        
    StringBuffer*   ret            = NULL;   
    BCHAR*        name            = NULL;
    /*
    * Use the name of the command to get the proper action to invoke
    */
    for (int i = 0; i < commands->size(); i++) {
        name = ((AbstractCommand*)(commands->get(i)))->getName();    
        if (name && bstrcmp(name, SYNC) == 0 && bstrcmp(SYNC, commandName) == 0) {
            if (!ret) {
                ret = new StringBuffer();
            }
            ret->append(getSync((Sync*)commands->get(i)));
        } else if (name && bstrcmp(name, ATOMIC) == 0 && bstrcmp(ATOMIC, commandName) == 0) {
            if (!ret) {
                ret = new StringBuffer();
            }
            ret->append(getAtomic((Atomic*)commands->get(i)));
        } else if (name && bstrcmp(name, SEQUENCE) == 0 && bstrcmp(SEQUENCE, commandName) == 0) {
            if (!ret) {
                ret = new StringBuffer();
            }
            ret->append(getSequence((Sequence*)commands->get(i)));
        } 
    }    

    if (NotZeroStringBufferLenght(1, ret)) {
        s = new StringBuffer();
        s->append(ret);        
    }    
    deleteAllStringBuffer(1, &ret);
    

    return s;   
}



StringBuffer* Formatter::getSyncBody(SyncBody* syncBody) {
       
    StringBuffer*   ret             = NULL;
    StringBuffer*   s               = NULL;    
    ArrayList*      commands        = new ArrayList();
    BCHAR* name                   = NULL;

    StringBuffer*   alerts          = NULL;
    StringBuffer*   statusArray     = NULL;    
    StringBuffer*   sync            = NULL;    
    StringBuffer*   commonCommandList = NULL;    
    StringBuffer*   map             = NULL;    
    StringBuffer*   exec            = NULL;    
    StringBuffer*   get             = NULL;    
    StringBuffer*   put             = NULL;    
    StringBuffer*   results         = NULL;    
    StringBuffer*   search          = NULL;    
    StringBuffer*   sequence        = NULL;    
    StringBuffer*   atomic          = NULL;    

    
    StringBuffer*   finalMessage    = NULL;
    
    commands = syncBody->getCommands();
    
    /*
    * Use the name of the command to get the proper action to invoke
    */
    for (int i = 0; i < commands->size(); i++) {
        name = ((AbstractCommand*)(commands->get(i)))->getName();    
        if (name && bstrcmp(name, STATUS) == 0) {
            if (!statusArray) {
                statusArray = new StringBuffer();
            }
            statusArray->append(getStatus((Status*)commands->get(i)));
        } else if (name && bstrcmp(name, ALERT) == 0) {
            if (!alerts) {
                alerts = new StringBuffer();
            }
            alerts->append(getAlert((Alert*)commands->get(i)));
        } else if (name && bstrcmp(name, SYNC) == 0) {
            if (!sync) {
                sync = new StringBuffer();
            }
            sync->append(getSync((Sync*)commands->get(i)));
        } else if (name && bstrcmp(name, MAP) == 0) {
            if (!map) {
                map = new StringBuffer();
            }
            map->append(getMap((Map*)commands->get(i)));
        } else if (name && bstrcmp(name, EXEC) == 0) {
            if (!exec) {
                exec = new StringBuffer();
            }
            exec->append(getExec((Exec*)commands->get(i)));
        } else if (name && bstrcmp(name, GET) == 0) {
            if (!get) {
                get = new StringBuffer();
            }
            get->append(getGet((Get*)commands->get(i)));
        } else if (name && bstrcmp(name, RESULTS) == 0) {
            if (!results) {
                results = new StringBuffer();
            }
            results->append(getResults((Results*)commands->get(i)));
        } else if (name && bstrcmp(name, PUT) == 0) {
            if (!put) {
                put = new StringBuffer();
            }
            put->append(getPut((Put*)commands->get(i)));
        } else if (name && bstrcmp(name, SEARCH) == 0) {
            if (!search) {
                search = new StringBuffer();
            }
            search->append(getSearch((Search*)commands->get(i)));
        } else if (name && bstrcmp(name, SEQUENCE) == 0) {
            if (!sequence) {
                sequence = new StringBuffer();
            }
            sequence->append(getSequence((Sequence*)commands->get(i)));
        } else if (name && bstrcmp(name, ATOMIC) == 0) {
            if (!atomic) {
                atomic = new StringBuffer();
            }
            atomic->append(getAtomic((Atomic*)commands->get(i)));
        }  
        
    }    
     //
    // get the common command copy, add, delete, replace
    //
    commonCommandList = getCommonCommandList(commands);
    
    finalMessage = getValue(FINAL_MSG, syncBody->getFinalMsg());

    if (finalMessage || NotZeroStringBufferLenght(11, alerts, commonCommandList, statusArray, sync, map, 
                                                     exec, get, put, results, search, sequence)) {
        s = new StringBuffer();
        s->append(statusArray);
        s->append(alerts);
        s->append(atomic);
        s->append(exec);
        s->append(get);
        s->append(map);
        s->append(put);
        s->append(results);
        s->append(search);
        s->append(sequence);
        s->append(sync);
        s->append(commonCommandList); 
        s->append(finalMessage);        
    }
    ret = getValue(SYNC_BODY, s);
    deleteAllStringBuffer(12, &s, &alerts, &finalMessage, &statusArray, &sync, &map, &exec, &get, &put, &results, &search, &sequence);

    return ret;   
}

StringBuffer* Formatter::getSearch(Search* search) {
    
    if (!search)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    

    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;    
    StringBuffer* noResp    = NULL;
    StringBuffer* noResults = NULL;
    StringBuffer* lang      = NULL;
    StringBuffer* data      = NULL;
    StringBuffer* target    = NULL;
    StringBuffer* sources   = NULL;

    cmdID     = getCmdID   (search->getCmdID());
    cred      = getCred    (search->getCred());
    meta      = getMeta    (search->getMeta());    
    noResp    = getValue   (NO_RESP, search->getNoResp());
    noResults = getValue   (NO_RESULTS, search->getNoResults());
    lang      = getValue   (LANG, search->getLang());    
    data      = getData    (search->getData());    
    target    = getTarget  (search->getTarget());
    sources   = getSources (search->getSources());

    if (NotZeroStringBufferLenght(9, cmdID, cred, meta, noResults, noResp, lang, data, target, sources)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(noResults);
        s->append(cred);
        s->append(target);
        s->append(sources);
        s->append(lang);
        s->append(meta);
        s->append(data);
    }

    ret = getValue(SEARCH, s);
    deleteAllStringBuffer(10, &s, &cred, &cmdID, &meta, &noResults, &noResp, &lang, &data, &target, &sources);
   
    return ret;
}


StringBuffer* Formatter::getGet(Get* get) {
    
    if (!get)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    

    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* items     = NULL;
    StringBuffer* noResp    = NULL;
    StringBuffer* lang      = NULL;
    
    cmdID     = getCmdID   (get->getCmdID());
    cred      = getCred    (get->getCred());
    meta      = getMeta    (get->getMeta());
    items     = getItems   (get->getItems());    
    noResp    = getValue   (NO_RESP, get->getNoResp());
    lang      = getValue   (LANG, get->getNoResp());    

    if (NotZeroStringBufferLenght(6, cmdID, cred, meta, items, noResp, lang)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(lang);
        s->append(cred);
        s->append(meta);
        s->append(items);
    }

    ret = getValue(GET, s);
    deleteAllStringBuffer(7, &s, &cred, &cmdID, &meta, &items, &noResp, &lang);
   
    return ret;
}

StringBuffer* Formatter::getExec(Exec* exec) {
    
    if (!exec)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
    
    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* items     = NULL;
    StringBuffer* noResp    = NULL;
    
    cmdID     = getCmdID   (exec->getCmdID());
    cred      = getCred    (exec->getCred());
    items     = getItems   (exec->getItems());    
    noResp    = getValue   (NO_RESP, exec->getNoResp());
    
    if (NotZeroStringBufferLenght(4, cmdID, cred, items, noResp)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(cred);
        s->append(items);
    }

    ret = getValue(EXEC, s);
    deleteAllStringBuffer(5, &s, &cred, &cmdID, &items, &noResp);
   
    return ret;
}

StringBuffer* Formatter::getMap(Map* map) {
    
    if (!map)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    

    StringBuffer* cmdID     = NULL;
    StringBuffer* target    = NULL;
    StringBuffer* source    = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;
    
    StringBuffer* mapItems  = NULL;
           
    cmdID     = getCmdID   (map->getCmdID());
    cred      = getCred    (map->getCred());
    meta      = getMeta    (map->getMeta());
    source    = getSource  (map->getSource());
    target    = getTarget  (map->getTarget());
    mapItems  = getMapItems(map->getMapItems());

    if (NotZeroStringBufferLenght(6, cmdID, cred, meta, source, target, mapItems)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(target);
        s->append(source);
        s->append(cred);
        s->append(meta);
        s->append(mapItems);
    }

    ret = getValue(MAP, s);
    deleteAllStringBuffer(7, &s, &cred, &cmdID, &meta, &source, &target, &mapItems);
   
    return ret;
}

/*
* Returns a series of 
*  <MapItem> ... </MapItem> 
*  <MapItem> ... </MapItem> 
*  <MapItem> ... </MapItem> 
* 
*/
StringBuffer* Formatter::getMapItems(ArrayList* mapItems) {
    
    if (!mapItems || !NotZeroArrayLenght(1, mapItems))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < mapItems->size(); i++) {
        ret->append(getMapItem(((MapItem*)mapItems->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getMapItem(MapItem* mapItem) {
    
    if (!mapItem)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
        
    StringBuffer* target     = NULL;
    StringBuffer* source      = NULL;
    
    target      = getTarget (mapItem->getTarget());
    source      = getSource (mapItem->getSource());
    
    if (NotZeroStringBufferLenght(2, target, source)) {
        s = new StringBuffer();
        s->append(target);
        s->append(source);        
    }

    ret = getValue(MAP_ITEM, s);
    deleteAllStringBuffer(2, &s, &target, &source);
   
    return ret;
}


StringBuffer* Formatter::getSync(Sync* sync) {
    
    if (!sync)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
        
    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* noResp    = NULL;
    StringBuffer* target    = NULL;
    StringBuffer* source    = NULL;
    StringBuffer* numberOfChanges    = NULL;
    StringBuffer* commonCommandList = NULL;
    
    StringBuffer* sequence = NULL;
    StringBuffer* atomic   = NULL;
    

    cmdID     = getCmdID   (sync->getCmdID());
    cred      = getCred    (sync->getCred());
    meta      = getMeta    (sync->getMeta());
    noResp    = getValue   (NO_RESP, sync->getNoResp());
    source    = getSource  (sync->getSource());
    target    = getTarget  (sync->getTarget());
    numberOfChanges = getValue(NUMBER_OF_CHANGES, sync->getNumberOfChanges());    
    commonCommandList = getCommonCommandList(sync->getCommands());
    
    sequence = getSpecificCommand(sync->getCommands(), SEQUENCE);
    atomic   = getSpecificCommand(sync->getCommands(), ATOMIC);

    if (NotZeroStringBufferLenght(10, cmdID, cred, meta, source, target, noResp, numberOfChanges, commonCommandList, sequence, atomic)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(cred);
        s->append(target);
        s->append(source);
        s->append(meta);
        s->append(numberOfChanges);
        s->append(atomic);
        s->append(commonCommandList);
        s->append(sequence);
    }

    ret = getValue(SYNC, s);
    deleteAllStringBuffer(11, &s, &cred, &cmdID, &meta, &source, &target, &noResp, &numberOfChanges, &commonCommandList, &atomic, &sequence);
   
    return ret;
}

StringBuffer* Formatter::getSequence(Sequence* sequence) {
    
    if (!sequence)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    

    StringBuffer* cmdID     = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* noResp    = NULL;
    StringBuffer* commonCommandList = NULL;
    StringBuffer* extraCommandList  = NULL;
    StringBuffer* sync      = NULL;
    StringBuffer* atomic      = NULL;

    cmdID     = getCmdID   (sequence->getCmdID());
    meta      = getMeta    (sequence->getMeta());
    noResp    = getValue   (NO_RESP, sequence->getNoResp());
    commonCommandList = getCommonCommandList(sequence->getCommands());    
    extraCommandList  = getExtraCommandList(sequence->getCommands());
    sync = getSpecificCommand(sequence->getCommands(), SYNC);
    atomic = getSpecificCommand(sequence->getCommands(), ATOMIC);

    if (NotZeroStringBufferLenght(7, cmdID, meta, noResp, commonCommandList, extraCommandList, sync, atomic)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(meta);
        s->append(commonCommandList);
        s->append(extraCommandList);
        s->append(atomic);
        s->append(sync);
        
    }

    ret = getValue(SEQUENCE, s);
    deleteAllStringBuffer(8, &s, &cmdID, &meta, &noResp, &commonCommandList, &extraCommandList, &sync, &atomic);
   
    return ret;
}

StringBuffer* Formatter::getAtomic(Atomic* atomic) {
    
    if (!atomic)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
    
    StringBuffer* cmdID     = NULL;
    StringBuffer* noResp    = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* commonCommandList = NULL;
    StringBuffer* extraCommandList  = NULL;
    StringBuffer* sync      = NULL;
    StringBuffer* sequence  = NULL;

    cmdID     = getCmdID   (atomic->getCmdID());
    noResp    = getValue   (NO_RESP, atomic->getNoResp());
    meta      = getMeta    (atomic->getMeta());
    commonCommandList = getCommonCommandList(atomic->getCommands());    
    extraCommandList  = getExtraCommandList(atomic->getCommands());
    sync      = getSpecificCommand(atomic->getCommands(), SYNC);
    sequence  = getSpecificCommand(atomic->getCommands(), SEQUENCE);

    if (NotZeroStringBufferLenght(7, cmdID, meta, noResp, commonCommandList, extraCommandList, sync, sequence)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(meta);
        s->append(commonCommandList);
        s->append(extraCommandList);
        s->append(sync);
        s->append(sequence);

    }

    ret = getValue(ATOMIC, s);
    deleteAllStringBuffer(8, &s, &cmdID, &meta, &noResp, &commonCommandList, &extraCommandList, &sync,&sequence);
   
    return ret;
}


/*
* Returns a series of 
*  <Copy> ... </Copy> 
*  <Copy> ... </Copy> 
*  <Copy> ... </Copy> 
* 
*/
StringBuffer* Formatter::getCopies(ArrayList* copies) {
    
    if (!copies || !NotZeroArrayLenght(1, copies))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < copies->size(); i++) {
        ret->append(getCopy(((Copy*)copies->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getCopy(Copy* copy) {
    
    if (!copy)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
        
    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* items     = NULL;
    StringBuffer* noResp    = NULL;
    
    cmdID     = getCmdID   (copy->getCmdID());
    cred      = getCred    (copy->getCred());
    meta      = getMeta    (copy->getMeta());
    items     = getItems   (copy->getItems());    
    noResp    = getValue   (NO_RESP, copy->getNoResp());
    
    if (NotZeroStringBufferLenght(5, cmdID, cred, meta, items, noResp)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(cred);
        s->append(meta);
        s->append(items);
    }

    ret = getValue(COPY, s);
    deleteAllStringBuffer(6, &s, &cred, &cmdID, &meta, &items, &noResp);
   
    return ret;
}



/*
* Returns a series of 
*  <Replace> ... </Replace> 
*  <Replace> ... </Replace> 
*  <Replace> ... </Replace> 
* 
*/
StringBuffer* Formatter::getReplaces(ArrayList* replaces) {
    
    if (!replaces || !NotZeroArrayLenght(1, replaces))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < replaces->size(); i++) {
        ret->append(getReplace(((Replace*)replaces->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getReplace(Replace* replace) {
    
    if (!replace)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    

    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* items     = NULL;
    StringBuffer* noResp    = NULL;
    
    cmdID     = getCmdID   (replace->getCmdID());
    cred      = getCred    (replace->getCred());
    meta      = getMeta    (replace->getMeta());
    items     = getItems   (replace->getItems());    
    noResp    = getValue   (NO_RESP, replace->getNoResp());
    
    if (NotZeroStringBufferLenght(5, cmdID, cred, meta, items, noResp)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(cred);
        s->append(meta);
        s->append(items);
    }

    ret = getValue(REPLACE, s);
    deleteAllStringBuffer(6, &s, &cred, &cmdID, &meta, &items, &noResp);
   
    return ret;
}


/*
* Returns a series of 
*  <Delete> ... </Delete> 
*  <Delete> ... </Delete> 
*  <Delete> ... </Delete> 
* 
*  The root is <SyncBody>
*/
StringBuffer* Formatter::getDels(ArrayList* dels) {
    
    if (!dels || !NotZeroArrayLenght(1, dels))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < dels->size(); i++) {
        ret->append(getDelete(((Delete*)dels->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getDelete(Delete* del) {
    
    if (!del)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
            
    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* items     = NULL;
    StringBuffer* noResp    = NULL;
    StringBuffer* archive   = NULL;
    StringBuffer* sftdel    = NULL;
    
    cmdID     = getCmdID   (del->getCmdID());
    cred      = getCred    (del->getCred());
    meta      = getMeta    (del->getMeta());
    items     = getItems   (del->getItems());    
    noResp    = getValue   (NO_RESP, del->getNoResp());
    archive   = getValue   (ARCHIVE, del->getArchive());
    sftdel    = getValue   (SFT_DEL, del->getSftDel());
    
    if (NotZeroStringBufferLenght(7, cmdID, cred, meta, items, noResp, sftdel, archive)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(archive);
        s->append(sftdel);
        s->append(cred);
        s->append(meta);
        s->append(items);
    }

    ret = getValue(DEL, s);
    deleteAllStringBuffer(8, &s, &cred, &cmdID, &meta, &items, &noResp, &sftdel, &archive);
   
    return ret;
}


/*
* Returns a series of 
*  <Add> ... </Add> 
*  <Add> ... </Add> 
*  <Add> ... </Add> 
* 
*  The root is <SyncBody>
*/
StringBuffer* Formatter::getAdds(ArrayList* adds) {
    
    if (!adds || !NotZeroArrayLenght(1, adds))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < adds->size(); i++) {
        ret->append(getAdd(((Add*)adds->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getAdd(Add* add) {
    
    if (!add)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
    
    StringBuffer* cmdID     = NULL;
    StringBuffer* cred      = NULL;
    StringBuffer* meta      = NULL;
    StringBuffer* items     = NULL;
    StringBuffer* noResp    = NULL;
    
    cmdID     = getCmdID   (add->getCmdID());
    cred      = getCred    (add->getCred());
    meta      = getMeta    (add->getMeta());
    items     = getItems   (add->getItems());    
    noResp    = getValue   (NO_RESP, add->getNoResp());
    
    if (NotZeroStringBufferLenght(5, cmdID, cred, meta, items, noResp)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(cred);
        s->append(meta);
        s->append(items);
    }

    ret = getValue(ADD, s);
    deleteAllStringBuffer(6, &s, &cred, &cmdID, &meta, &items, &noResp);
   
    return ret;
}

StringBuffer* Formatter::getPut(Put* put) {
    
    if (!put)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    

    StringBuffer*        cmdID  = NULL;
    StringBuffer*        meta   = NULL;
    StringBuffer*        items  = NULL;
    StringBuffer*        cred   = NULL;
    StringBuffer*        noResp = NULL;
    StringBuffer*        lang   = NULL;

    cmdID     = getCmdID   (put->getCmdID());
    noResp    = getValue   (NO_RESP, put->getNoResp());
    lang      = getValue   (LANG, put->getNoResp());    
    cred      = getCred    (put->getCred());
    meta      = getMeta    (put->getMeta());
    items     = getItems   (put->getItems());        

    if (NotZeroStringBufferLenght(6, cmdID, lang, meta, items, cred, noResp)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);                        
        s->append(lang);
        s->append(cred);
        s->append(meta);
        s->append(items);        
    }

    ret = getValue(PUT, s);
    deleteAllStringBuffer(7, &s, &cmdID, &items, &cred, &lang, &meta, &noResp);
   
    return ret;
}

StringBuffer* Formatter::getResults(Results* results) {
    
    if (!results)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    

    StringBuffer*        cmdID  = NULL;
    StringBuffer*        msgRef = NULL;
    StringBuffer*        cmdRef = NULL;
    StringBuffer*        meta   = NULL;
    StringBuffer*        items  = NULL;
    StringBuffer*        targetRefs  = NULL;
    StringBuffer*        sourceRefs  = NULL;
    
    cmdID     = getCmdID   (results->getCmdID());
    msgRef    = getValue   (MSG_REF, results->getMsgRef(NULL));
    cmdRef    = getValue   (CMD_REF, results->getCmdRef(NULL));
    meta      = getMeta    (results->getMeta());
    items     = getItems   (results->getItems());        
    sourceRefs = getSourceRefs(results->getSourceRef());
    targetRefs = getTargetRefs(results->getTargetRef());    
    
    if (NotZeroStringBufferLenght(7, cmdID, msgRef, cmdRef, meta, items, sourceRefs, targetRefs)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(msgRef);
        s->append(cmdRef);
        s->append(meta);
        s->append(targetRefs);        
        s->append(sourceRefs);
        s->append(items);        
    }

    ret = getValue(RESULTS, s);
    deleteAllStringBuffer(8, &s, &cmdID, &items, &msgRef, &cmdRef, &meta, &sourceRefs, &targetRefs);
   
    return ret;
}

/*
* Returns a series of 
*  <Status> ... </Status> 
*  <Status> ... </Status> 
*  <Status> ... </Status> 
* 
*/
StringBuffer* Formatter::getStatusArray(ArrayList* statusArray) {
    
    if (!statusArray || !NotZeroArrayLenght(1, statusArray))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < statusArray->size(); i++) {
        ret->append(getStatus(((Status*)statusArray->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getStatus(Status* status) {
    
    if (!status)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
    
    StringBuffer*        cmdID  = NULL;
    StringBuffer*        msgRef = NULL;
    StringBuffer*        cmdRef = NULL;
    StringBuffer*        cmd    = NULL;
    StringBuffer*        cred   = NULL;
    StringBuffer*        items  = NULL;
    StringBuffer*        targetRefs  = NULL;
    StringBuffer*        sourceRefs  = NULL;
    StringBuffer*        data        = NULL;
    StringBuffer*        chal        = NULL;
    
    cmdID     = getCmdID   (status->getCmdID());
    msgRef    = getValue   (MSG_REF, status->getMsgRef(NULL));
    cmdRef    = getValue   (CMD_REF, status->getCmdRef(NULL));
    cmd       = getValue   (CMD     , status->getCmd(NULL));
    items     = getItems   (status->getItems());        
    cred      = getCred    (status->getCred());
    sourceRefs = getSourceRefs(status->getSourceRef());
    targetRefs = getTargetRefs(status->getTargetRef());    
    chal      = getChal(status->getChal());    
    data      = getData   (status->getData());    
        
    if (NotZeroStringBufferLenght(10, cmdID, msgRef, cmdRef, cmd, cred, data, items, sourceRefs, targetRefs, chal)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(msgRef);
        s->append(cmdRef);
        s->append(cmd);
        s->append(targetRefs);        
        s->append(sourceRefs);
        s->append(cred);        
        s->append(chal);
        s->append(data);
        s->append(items);
    }

    ret = getValue(STATUS, s);
    deleteAllStringBuffer(11, &s, &cred, &cmdID, &data, &items, &msgRef, &cmdRef, &cmd, &sourceRefs, &targetRefs, &chal);
   
    return ret;
}

StringBuffer* Formatter::getChal(Chal* chal) {
    if (!chal)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
    
    StringBuffer* meta     = getMeta   (chal->getMeta());
    
    if (NotZeroStringBufferLenght(1, meta)) {
        s = new StringBuffer();
        s->append(meta);        
    }

    ret = getValue(CHAL, s);
    deleteAllStringBuffer(2, &s, &meta);
   
    return ret;

}

/*
* Returns a series of 
*  <Alert> ... </Alert> 
*  <Alert> ... </Alert> 
*  <Alert> ... </Alert> 
* 
*  The root is <SyncBody>
*/
StringBuffer* Formatter::getAlerts(ArrayList* alerts) {
    
    if (!alerts || !NotZeroArrayLenght(1, alerts))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < alerts->size(); i++) {
        ret->append(getAlert(((Alert*)alerts->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getAlert(Alert* alert) {
    
    if (!alert)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;    
    
    StringBuffer* cmdID     = getCmdID   (alert->getCmdID());
    StringBuffer* cred      = getCred    (alert->getCred());
    StringBuffer* data      = getValue   (DATA, (long)alert->getData());
    StringBuffer* items     = getItems   (alert->getItems());    
    StringBuffer* noResp    = getValue   (NO_RESP, alert->getNoResp());
    
    if (NotZeroStringBufferLenght(5, cmdID, cred, data, items, noResp)) {
        s = new StringBuffer();
        s->append(cmdID);
        s->append(noResp);
        s->append(cred);
        s->append(data);
        s->append(items);
    }

    ret = getValue(ALERT, s);
    deleteAllStringBuffer(6, &s, &cred, &cmdID, &data, &items, &noResp);
   
    return ret;
}

/*
* Returns a series of 
*  <Item> ... </Item> 
*  <Item> ... </Item> 
*  <Item> ... </Item> 
* 
*/
StringBuffer* Formatter::getItems(ArrayList* items) {
    
    if (!items || !NotZeroArrayLenght(1, items))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < items->size(); i++) {
        ret->append(getItem(((Item*)items->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getItem(Item* item) {
    
    if (!item)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;
    
    StringBuffer*      target   = NULL;
    StringBuffer*      source   = NULL;
    StringBuffer*      meta     = NULL;
    StringBuffer*      data     = NULL;
    StringBuffer*      moreData = NULL;  
    StringBuffer*      targetParent = NULL;
    StringBuffer*      sourceParent = NULL;


    
    target    = getTarget   (item->getTarget());
    source    = getSource   (item->getSource());
    meta      = getMeta     (item->getMeta());    
    data      = getData     (item->getData());
    moreData  = getValue    (MORE_DATA, item->getMoreData());
    targetParent  = getValue    (TARGET_PARENT, item->getTargetParent());
    sourceParent  = getValue    (SOURCE_PARENT, item->getSourceParent());

    
    if (NotZeroStringBufferLenght(7, target, source, targetParent, sourceParent, meta, data, moreData)) {
        s = new StringBuffer();
        s->append(target);
        s->append(source);
        s->append(targetParent);
        s->append(sourceParent);
        s->append(meta);
        s->append(data);
        s->append(moreData);
    }

    ret = getValue(ITEM, s);
    deleteAllStringBuffer(8, &s, &target, &source, &targetParent, &sourceParent, &meta, &data, &moreData);

    return ret;
}

StringBuffer* Formatter::getData(Data* data) {
    
    if (!data)
        return NULL;
    
    StringBuffer* ret       = NULL;
    StringBuffer* s         = NULL;
    BCHAR* simpleData     = NULL;

    if ((simpleData = data->getData(NULL)) != NULL) {        
        s = new StringBuffer();
        s->append(simpleData);
        ret = getValue(DATA, s);     
    }
    deleteAllStringBuffer(1, &s);
    return ret;
}

StringBuffer* Formatter::getData(ComplexData* data) {
    
    if (!data)
        return NULL;
    
    StringBuffer s;
    
    StringBuffer* anchor = getAnchor(data->getAnchor());
    StringBuffer* devInf = getDevInf(data->getDevInf());
    
    if ((anchor != NULL && anchor->length() > 0) ||
        (devInf != NULL && devInf->length() > 0)) {
        s.append(anchor);
        s.append(devInf);
    } else {
        StringBuffer tmp(data->getData());
        tmp.replaceAll("&", "&amp;");
        tmp.replaceAll("<", "&lt;");
        s.append(tmp);        
    }

    deleteAllStringBuffer(2, &anchor, &devInf);

    //
    // Now let's process the list of Property (if any)
    //
    int nProps = 0;
    ArrayList* properties = data->getProperties();
    if (properties) {
        nProps = properties->size();
    }

    StringBuffer* t1 = NULL;
    for (int i=0; i<nProps; ++i) {
        t1 = getProperty((Property*)properties->get(i));
        s.append(t1);
        delete t1; t1 = NULL;
    }
    
    return getValue(DATA, &s);
}

StringBuffer* Formatter::getDevInf(DevInf* devInf) {
    
    if (!devInf)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;
    
    StringBuffer* verDTD    = NULL;
    StringBuffer* man       = NULL;
    StringBuffer* mod       = NULL;
    StringBuffer* oem       = NULL;
    StringBuffer* fwV       = NULL;
    StringBuffer* swV       = NULL;
    StringBuffer* hwV       = NULL;
    StringBuffer* devID     = NULL;
    StringBuffer* devTyp    = NULL;

    StringBuffer* dataStores= NULL;     //DataStore[]
    StringBuffer* ctCaps    = NULL;     // CTCap[]
    StringBuffer* exts      = NULL;         // Ext[]

    StringBuffer* utc                    = NULL;
    StringBuffer* supportLargeObjs       = NULL;
    StringBuffer* supportNumberOfChanges = NULL;      

    StringBuffer* syncCap   = NULL;

    verDTD  = getVerDTD   (devInf->getVerDTD());        
    man     = getValue    (MAN, devInf->getMan(NULL));
    mod     = getValue    (MOD, devInf->getMod(NULL));
    oem     = getValue    (OEM, devInf->getOEM(NULL));
    fwV     = getValue    (FWV, devInf->getFwV(NULL));
    swV     = getValue    (SWV, devInf->getSwV(NULL));
    hwV     = getValue    (HWV, devInf->getHwV(NULL));
    devID   = getValue    (DEV_ID, devInf->getDevID(NULL));
    devTyp  = getValue    (DEV_TYP, devInf->getDevTyp(NULL));
    dataStores = getDataStores(devInf->getDataStore());
    syncCap    = getSyncCap(devInf->getSyncCap());
    ctCaps     = getCTCaps(devInf->getCTCap());
    exts       = getExts(devInf->getExt());

    utc                     = getValue    (UTC, devInf->getUTC());
    supportLargeObjs        = getValue    (SUPPORT_LARGE_OBJECT, devInf->getSupportLargeObjs());
    supportNumberOfChanges  = getValue    (SUPPORT_NUMBER_OF_CHANGES, devInf->getSupportNumberOfChanges());

    if (NotZeroStringBufferLenght(15, verDTD, man, mod, oem, fwV, swV, hwV, devID, devTyp, 
                                      dataStores, syncCap, ctCaps, utc, supportLargeObjs, supportNumberOfChanges)) {
        s = new StringBuffer();
        s->append(verDTD);
        s->append(man);        
        s->append(mod);        
        s->append(oem);        
        s->append(fwV);        
        s->append(swV);        
        s->append(hwV);        
        s->append(devID);        
        s->append(devTyp);    
        s->append(dataStores);
        s->append(ctCaps);
        s->append(exts);
        s->append(syncCap);
        s->append(utc);    
        s->append(supportLargeObjs);    
        s->append(supportNumberOfChanges);            
    }

    ret = getValue(DEV_INF, s);
    
    deleteAllStringBuffer(16, &s, &verDTD, &man, &mod, &oem, &fwV, &swV, &hwV, 
                              &devID, &devTyp, &dataStores, &ctCaps, &exts, &utc, 
                              &supportLargeObjs, &supportNumberOfChanges);

    return ret;
}

/*
* Returns a series of 
*  <Ext> ... </Ext> 
*  <Ext> ... </Ext> 
*  <Ext> ... </Ext> 
* 
*/
StringBuffer* Formatter::getExts(ArrayList* exts) {
    
    if (!exts || !NotZeroArrayLenght(1, exts))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < exts->size(); i++) {
        ret->append(getExt(((Ext*)exts->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getExt(Ext* ext) {
    
    if (!ext)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;
       
    StringBuffer*  xNam    = NULL;
    StringBuffer*  xVals   = NULL;
    
    xNam  = getValue (XNAM, ext->getXNam(NULL));
    xVals = getXVals (ext->getXVal());
    
    if (NotZeroStringBufferLenght(2, xNam, xVals)) {
        s = new StringBuffer();
        s->append(xNam);
        s->append(xVals);
    }

    ret = getValue(EXT, s);
    deleteAllStringBuffer(3, &s, &xNam, &xVals);

    return ret;
}


/*
* Returns a series of 
*  <XVal> ... </XVal> 
*  <XVal> ... </XVal> 
*  <XVal> ... </XVal> 
* 
*/
StringBuffer* Formatter::getXVals(ArrayList* xVals) {
    
    if (!xVals || !NotZeroArrayLenght(1, xVals))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < xVals->size(); i++) {
        ret->append(getXVal(((StringElement*)xVals->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getXVal(StringElement* xVal) {
    
    if (!xVal)
        return NULL;
    
    StringBuffer* ret = NULL;               
    ret  = getValue (XVAL, xVal->getValue(NULL));               
    return ret;
}


/*
* Returns a series of 
*  <DataStore> ... </DataStore> 
*  <DataStore> ... </DataStore> 
*  <DataStore> ... </DataStore> 
* 
*/
StringBuffer* Formatter::getDataStores(ArrayList* dataStores) {
    
    if (!dataStores || !NotZeroArrayLenght(1, dataStores))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < dataStores->size(); i++) {
        ret->append(getDataStore(((DataStore*)dataStores->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getDataStore(DataStore* dataStore) {
    
    if (!dataStore)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;
    
    StringBuffer*    sourceRef      = NULL;
    StringBuffer*    displayName    = NULL;
    StringBuffer*    maxGUIDSize    = NULL;
    StringBuffer*    rxPref         = NULL;
    StringBuffer*    rx             = NULL; // ContentTypeInfo[]
    StringBuffer*    txPref         = NULL;
    StringBuffer*    tx             = NULL; // ContentTypeInfo[]
    StringBuffer*    dsMem          = NULL;
    StringBuffer*    syncCap        = NULL;  
    
    sourceRef   = getSourceRef(dataStore->getSourceRef());
    displayName = getValue(DISPLAY_NAME, dataStore->getDisplayName(NULL));
    maxGUIDSize = getValue(MAX_GUID_SIZE, dataStore->getMaxGUIDSize());
    rxPref      = getContentTypeInfo(dataStore->getRxPref(), RX_PREF);
    rx          = getContentTypeInfos(dataStore->getRx(), RX);
    txPref      = getContentTypeInfo(dataStore->getTxPref(), TX_PREF);
    tx          = getContentTypeInfos(dataStore->getTx(), TX);
    dsMem       = getDSMem(dataStore->getDSMem());
    syncCap     = getSyncCap(dataStore->getSyncCap());

    if (NotZeroStringBufferLenght(9, sourceRef, displayName, maxGUIDSize, rxPref, rx, txPref, tx, dsMem, syncCap)) {
        s = new StringBuffer();
        s->append(sourceRef);
        s->append(displayName);
        s->append(maxGUIDSize);
        s->append(rxPref);
        s->append(rx);
        s->append(txPref);
        s->append(tx);
        s->append(dsMem);
        s->append(syncCap);
    }

    ret = getValue(DATA_STORE, s);
    deleteAllStringBuffer(10, &s, &sourceRef  ,&displayName, &maxGUIDSize, &rxPref, &rx, &txPref, &tx, &dsMem, &syncCap);

    return ret;
}

StringBuffer* Formatter::getSyncCap(SyncCap* syncCap) {
    
    if (!syncCap)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;

    StringBuffer* syncTypes   = NULL;
    
    syncTypes   = getSyncTypes(syncCap->getSyncType());
    
    if (NotZeroStringBufferLenght(1, syncTypes)) {
        s = new StringBuffer();
        s->append(syncTypes);        
    }

    ret = getValue(SYNC_CAP, s);
    deleteAllStringBuffer(2, &s, &syncTypes);

    return ret;
}

StringBuffer* Formatter::getSyncTypes(ArrayList* syncTypes) {
    
    if (!syncTypes || !NotZeroArrayLenght(1, syncTypes))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < syncTypes->size(); i++) {
        ret->append(getSyncType(((SyncType*)syncTypes->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getSyncType(SyncType* syncType) {
    
    if (!syncType)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;       
    int  value  = -1;
        
    value   = syncType->getType();
    
    if (value > -1) {
        s = new StringBuffer();
        s->append(value);        
    }

    ret = getValue(SYNC_TYPE, s);
    deleteAllStringBuffer(1, &s);

    return ret;
}


StringBuffer* Formatter::getDSMem(DSMem* dsMem) {    
    if (!dsMem)
        return NULL;

    StringBuffer* ret = new StringBuffer();                           
    
    ret->append(getValue(SHARED_MEM, dsMem->getSharedMem()));    
    ret->append(getValue(MAX_MEM,    dsMem->getMaxMem()));
    ret->append(getValue(MAX_ID,     dsMem->getMaxID()));
    
    return ret;    
}


StringBuffer* Formatter::getContentTypeInfos(ArrayList* contentTypeInfos, BCHAR* TAG) {
    
    if (!contentTypeInfos || !NotZeroArrayLenght(1, contentTypeInfos))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < contentTypeInfos->size(); i++) {
        ret->append(getContentTypeInfo(((ContentTypeInfo*)contentTypeInfos->get(i)), TAG));   
    }
    return ret;
}


StringBuffer* Formatter::getContentTypeInfo(ContentTypeInfo* contentTypeInfo, BCHAR* TAG) {
    
    if (!contentTypeInfo)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;
       
    StringBuffer*  ctType  = NULL;
    StringBuffer*  verCT   = NULL;
    
    ctType  = getValue (CT_TYPE, contentTypeInfo->getCTType(NULL));
    verCT   = getValue (VER_CT, contentTypeInfo->getVerCT(NULL));
    
    if (NotZeroStringBufferLenght(2, ctType, verCT)) {
        s = new StringBuffer();
        s->append(ctType);
        s->append(verCT);
    }

    ret = getValue(TAG, s);
    deleteAllStringBuffer(3, &s, &ctType, &verCT);

    return ret;
}

StringBuffer* Formatter::getTargetRefs(ArrayList* targetRefs) {
    
    if (!targetRefs || !NotZeroArrayLenght(1, targetRefs))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < targetRefs->size(); i++) {
        ret->append(getTargetRef(((TargetRef*)targetRefs->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getTargetRef(TargetRef* targetRef) {
    
    if (!targetRef)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;
    
    BCHAR*       value  = NULL;
    StringBuffer*  target = NULL;
    
    if ((value = (BCHAR*)targetRef->getValue()) != NULL) {
        ;  // the value value is arleady set.
    } else {
        target = getTarget(targetRef->getTarget());
    }

    if (value || NotZeroStringBufferLenght(1, target)) {
        s = new StringBuffer();
        s->append(value);
        s->append(target);
    }

    ret = getValue(TARGET_REF, s);
    deleteAllStringBuffer(2, &s, &target);
    

    return ret;
}


StringBuffer* Formatter::getSourceRefs(ArrayList* sourceRefs) {
    
    if (!sourceRefs || !NotZeroArrayLenght(1, sourceRefs))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < sourceRefs->size(); i++) {
        ret->append(getSourceRef(((SourceRef*)sourceRefs->get(i))));   
    }
    return ret;
}

StringBuffer* Formatter::getSourceRef(SourceRef* sourceRef) {
    
    if (!sourceRef)
        return NULL;
    
    StringBuffer* ret = NULL;
    StringBuffer* s   = NULL;
    
    BCHAR*       value  = NULL;
    StringBuffer*  source = NULL;
    
    if ((value = (BCHAR*)sourceRef->getValue()) != NULL) {
        ;  // the value value is arleady set.
    } else {
        source = getSource(sourceRef->getSource());
    }

    if (value || NotZeroStringBufferLenght(1, source)) {
        s = new StringBuffer();
        s->append(value);
        s->append(source);
    }

    ret = getValue(SOURCE_REF, s);
    deleteAllStringBuffer(2, &s, &source);
    

    return ret;
}

/*
* Returns a series of 
*  <CTCap> ... </CTCap> 
*  <CTCap> ... </CTCap> 
*  <CTCap> ... </CTCap> 
* 
*/
StringBuffer* Formatter::getCTCaps(ArrayList* ctCaps) {
    
    if (!ctCaps || !NotZeroArrayLenght(1, ctCaps))
        return NULL;
    
    StringBuffer* ret = new StringBuffer();
        
    for (int i = 0; i < ctCaps->size(); i++) {
        ret->append(getCTCap(((CTCap*)ctCaps->get(i))));   
    }
    return ret;
}

//
// TBD
//
StringBuffer* Formatter::getCTCap(CTCap* ctCap) {
    
    if (!ctCap)
        return NULL;
    
    StringBuffer* ret = NULL;    
        
    return ret;
}

/**
 * Returns a StringBuffer containing the string representation of the
 * given PropParam
 */
StringBuffer* Formatter::getPropParam(PropParam* p) {
    if (p == NULL) {
        return NULL;
    }

    StringBuffer s;

    StringBuffer* paramName   = getValue(PARAM_NAME,   p->getParamName()  );
    StringBuffer* displayName = getValue(DISPLAY_NAME, p->getDisplayName());
    StringBuffer* dataType    = getValue(DATA_TYPE,    p->getDataType()   );

    //
    // Val enums
    //
    StringBuffer valEnums;
    ArrayList* enums = p->getValEnums();
    if (enums) {
        StringBuffer* t = NULL;
        for(int i=0; i<enums->size(); ++i) {
            t = getValue(VAL_ENUM, (BCHAR*)enums->get(i));
            valEnums.append(t);
            delete t; t = NULL;
        }
    }

    if (NotZeroStringBufferLenght(3, paramName, displayName, dataType)) {
        s.append(paramName  );
        s.append(displayName);
        s.append(dataType   );
        s.append(valEnums   );
    }

    return getValue(PROP_PARAM, s);
}

/**
 * Returns a StringBuffer containing the string representation of the
 * given Property
 */
StringBuffer* Formatter::getProperty(Property* p) {
    if (p == NULL) {
        return NULL;
    }

    StringBuffer s;

    StringBuffer* displayName = getValue(DISPLAY_NAME, p->getDisplayName());
    StringBuffer* propName    = getValue(PROP_NAME,    p->getPropName()   );
    StringBuffer* dataType    = getValue(DATA_TYPE,    p->getDataType()   );
    StringBuffer* maxOccur    = (p->getMaxOccur()>=0) 
                              ? getValue(MAX_OCCUR, p->getMaxOccur())
                              : NULL
                              ;
                              
    StringBuffer* maxSize     = (p->getMaxSize() >= 0)
                              ? getValue(MAX_SIZE, p->getMaxSize())
                              : NULL
                              ;

    StringBuffer* truncate    = getValue(NO_TRUNCATE,  p->isNoTruncate()  );

    //
    // Property parameters
    //
    StringBuffer propParams;
    ArrayList* parameters = p->getPropParams();
    if (parameters) {
        StringBuffer* t = NULL;
        for(int i=0; i<parameters->size(); ++i) {
            t = getPropParam((PropParam*)parameters->get(i));
            propParams.append(t);
            delete t; t = NULL;
        }
    }

    //
    // Val enums
    //
    StringBuffer valEnums;
    ArrayList* enums = p->getValEnums();
    if (enums) {
        StringBuffer* t = NULL;
        for(int i=0; i<enums->size(); ++i) {
            t = getValue(VAL_ENUM, (BCHAR*)enums->get(i));
            valEnums.append(t);
            delete t; t = NULL;
        }
    }

    if (NotZeroStringBufferLenght(6, displayName, 
                                     propName, 
                                     dataType, 
                                     maxOccur, 
                                     maxSize, 
                                     truncate)) {
        s.append(displayName);
        s.append(propName   );
        s.append(maxSize    );
        s.append(dataType   );
        s.append(propParams );
        s.append(valEnums   );
        s.append(maxOccur   );        
        s.append(truncate   );
    }

    deleteAllStringBuffer(
        6, 
        &displayName, 
        &propName, 
        &dataType, 
        &maxOccur, 
        &maxSize, 
        &truncate
    );

    return getValue(PROPERTY, s);
}


/**
 * Returns a StringBuffer containing the string representation of the
 * given Filter
 */
StringBuffer* Formatter::getFilter(Filter* filter) {
    if (filter == NULL) {
        return NULL;
    }

    StringBuffer s;

    StringBuffer* type   = (filter->getFilterType()) 
                         ? getValue(FILTER_TYPE, filter->getFilterType())
                         : new StringBuffer()
                         ;
    StringBuffer* meta   = getMeta(filter->getMeta());
    StringBuffer* record = getItem(filter->getRecord());
    StringBuffer* field  = getItem(filter->getField());

    if (NotZeroStringBufferLenght(4, type, meta, record, field)) {
        StringBuffer* ret = NULL;

        s.append(type);
        s.append(meta);
        s.append(ret = getValue(RECORD, record)); delete ret; ret = NULL;
        s.append(ret = getValue(FIELD, field)); delete ret; ret = NULL;
    }

    deleteAllStringBuffer(4, &type, &meta, &record, &field);

    return getValue(FILTER, s);
}
