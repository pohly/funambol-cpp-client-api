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

#include <stdlib.h>

#include "base/Log.h"  // TBR
#include "base/util/ArrayList.h"
#include "base/util/utils.h"
#include "spds/constants.h"
#include "spds/SyncMLProcessor.h"
#include "spds/spdsutils.h"

#include "event/FireEvent.h"

/*
 * This class is responsible for the processing of the incoming messages.
 */
/*
 * Constructor
 */
SyncMLProcessor::SyncMLProcessor() : XMLProcessor() {
}

SyncML* SyncMLProcessor::processMsg(char* msg) {
    SyncML* syncml      = Parser::getSyncML(msg);
    return syncml;
}


int SyncMLProcessor::processSyncHdrStatus(SyncML* syncml) {
    int ret = getStatusCode(syncml->getSyncBody(), NULL, SYNC_HDR);
    
    // Fire Sync Status Event: syncHdr status from server
    fireSyncStatusEvent(SYNC_HDR, ret, NULL, NULL, NULL , SERVER_STATUS);

    return ret;
}

int SyncMLProcessor::processAlertStatus(SyncSource& source, SyncML* syncml, ArrayList* alerts) {

    int ret = -1;
    ArrayList* list     = new ArrayList();
    char* name = NULL;
    Status* s     = NULL;
    Data* data    = NULL;
    SourceRef* sourceRef    = NULL;

    if (alerts->size()) {
        list = syncml->getSyncBody()->getCommands();
        
        for (int i = 0; i < list->size(); i++) {
            // is returned the pointer to the element not a new element
            name = ((AbstractCommand*)(list->get(i)))->getName();
            if (name && strcmp(name, STATUS) == 0) {
                s = (Status*)list->get(i);
                if (strcmp(s->getCmd(), ALERT) == 0) { 
                    sourceRef = (SourceRef*)(s->getSourceRef()->get(0));

                    if (sourceRef) {
                        if (strcmp(_wcc(source.getName()),
                                    sourceRef->getValue()) == 0) {
                            ret = getAlertStatusCode(s, _wcc(source.getName()));
                            break;
                        }
                    } else {
                        // Server did not include <SourceRef>, which
                        // is a violation of the standard for commands
                        // which were sent with <SourceRef>. Happens
                        // with Synthesis server if authentication
                        // failed, in which case we can simply ignore
                        // it.
                    }
                }
            }
        }
    }

    // Fire a syncStatus event: Alert status from server
    fireSyncStatusEvent(ALERT, ret, source.getConfig().getName(), source.getConfig().getURI(), NULL, SERVER_STATUS);

    return ret;

}


/*
 * Processes the initialization response. Returns 0 in case of success, an
 * error code in case of error.
 *
 * @param msg the response from the server
 */
int SyncMLProcessor::processServerAlert(SyncSource& source, SyncML* syncml) {
    
    int ret             = -1;
    int iterator        = 0;
    AbstractCommand* a  = NULL;   
    Item* item          = NULL;
    ArrayList* list     = new ArrayList();
    BOOL found          = FALSE;    

    ret = 0;
    do {
        a = getCommand(syncml->getSyncBody(), ALERT, iterator);                   
        if (a == NULL) {
            // This happens with the Synthesis server's reply:
            // instead of sending SyncBody/Alert we get SyncBody/Put
            // with device infos and a SyncBody/Get requesting our own
            // device infos. Ignoring the request is not correct, but
            // allows synchronization to proceed and complete eventually
            // without any further errors. For that to work we must not
            // set lastErrorCode here, as it will be checked at the end of
            // the sync.
            //
            // lastErrorCode = ERR_REPRESENTATION;
            // sprintf(lastErrorMsg, T("SyncBody/Alert not found!"));
            goto finally;
        }
        Alert* alert = (Alert*)a;
        Item* item = NULL;
        ArrayList* itemList = alert->getItems();

        for (int i = 0; i < itemList->size(); i++) {
            item = (Item*)getArrayElement(itemList, i);
            const char *locURI = ((Target*)item->getTarget())->getLocURI();
            if (strcmp( locURI, _wcc(source.getName()) ) == 0) {                   
                if (alert->getData() == NULL) {
                    lastErrorCode = ERR_REPRESENTATION;
                    sprintf(lastErrorMsg, T("SyncBody/Alert/Data not found!"));
                    goto finally;
                }

                source.setSyncMode((SyncMode)alert->getData());
                ret = 0;
                found = TRUE;
                break;        
            }
        }                        
        iterator++;
        if (found)
            break;

    } while(a);
   
finally:
    
    return ret;
}


char** SyncMLProcessor::getSortedSourcesFromServer(SyncML* syncml, int sourcesNumber) {
    
    char** sourceList = new char*[sourcesNumber+1];
    int iterator        = 0;
    AbstractCommand* a  = NULL;   
    Item* item          = NULL;
    ArrayList* list     = new ArrayList();  

    do {
        a = getCommand(syncml->getSyncBody(), ALERT, iterator);                   
        if (a == NULL) {
            goto finally;
        }
        Alert* alert = (Alert*)a;
        Item* item = NULL;
        ArrayList* itemList = alert->getItems();

        for (int i = 0; i < itemList->size(); i++) {
            item = (Item*)getArrayElement(itemList, i);
            const char *locURI = ((Target*)item->getTarget())->getLocURI();
            sourceList[iterator] = stringdup(locURI);
        }                        
        iterator++;

    } while(a);
   
finally:
    sourceList[iterator] = NULL;
    return sourceList;
}




int SyncMLProcessor::processItemStatus(SyncSource& source, SyncBody* syncBody) {
    
    ArrayList* items = NULL;
    Item* item       = NULL;
    SourceRef* sourceRef = NULL;
    Status* s = NULL;
    char* name = NULL;
    Data* data = NULL;
    int ret = 0;

    ArrayList* list = getCommands(syncBody, STATUS);

    for (int i = 0; i < list->size(); i++) {
        s = (Status*)list->get(i);
        name = s->getCmd();    
        data = s->getData();
        if (strcmp(name, SYNC) == 0){
            char *srcname = toMultibyte(source.getName());
            int alertStatus = getAlertStatusCode(s, srcname);
            delete [] srcname;
            
			// Fire Sync Status Event: sync status from server
            fireSyncStatusEvent(SYNC, s->getStatusCode(), source.getConfig().getName(), source.getConfig().getURI(), NULL, SERVER_STATUS);            

            if(alertStatus < 0 || alertStatus >=300){
                if ((ret = alertStatus) < 0)
                    LOG.error("processItemStatus: status not found in SYNC");
                else
                    LOG.error("processItemStatus: server sent status %d in SYNC", alertStatus);                
                break;            
            }
            
        }         
        
        else if (strcmp(name, ADD) == 0 ||
            strcmp(name, REPLACE) == 0 ||
            strcmp(name, DEL) == 0) {

            int k;

            items = s->getItems();
            long val = strtol(data->getData() , NULL, 10);
            for (k = 0; k < items->size(); k++) {
                item = (Item*)items->get(k);
                if (item) {
                    Source* itemSource = item->getSource();
                    if (itemSource) {
                        WCHAR *uri = toWideChar(itemSource->getLocURI());
                        // Fire Sync Status Event: item status from server
                        fireSyncStatusEvent(s->getCmd(), s->getStatusCode(), source.getConfig().getName(), source.getConfig().getURI(), uri, SERVER_STATUS);
                        // Update SyncReport
                        source.getReport()->addItem(SERVER, s->getCmd(), uri, s->getStatusCode());
                        
                        source.setItemStatus(uri, val);
                        delete [] uri;
                    } else {
                        // the item might consist of additional information, as in:
                        // <SourceRef>pas-id-44B544A600000092</SourceRef>
                        // <Data>200</Data>
                        // <Item><Data>Conflict resolved by server</Data></Item>
                    }
                }
            }
            items = s->getSourceRef();
            for (k = 0; k < items->size(); k++) {
                sourceRef = (SourceRef*)items->get(k);
                if (sourceRef) {
                    WCHAR *srcref = toWideChar(sourceRef->getValue());
			        // Fire Sync Status Event: item status from server
                    fireSyncStatusEvent(s->getCmd(), s->getStatusCode(), source.getConfig().getName(), source.getConfig().getURI(), srcref, SERVER_STATUS);
                    // Update SyncReport
                    source.getReport()->addItem(SERVER, s->getCmd(), srcref, s->getStatusCode());

                    source.setItemStatus(srcref, val);
                    delete [] srcref;
                }
            }
        }
    }

    deleteArrayList(&list);    
    return ret;
}

/*
 * Processes the initialization response. Returns 0 in case of success, an
 * error code in case of error.
 *
 * @param msg the response from the server
 */

Sync* SyncMLProcessor::processSyncResponse(SyncSource& source, SyncML* syncml) {    
    
    int iterator = 0, ret = 0;    
    
    AbstractCommand* a  = NULL;           
    Sync* sync          = NULL;

    ret = getStatusCode(syncml->getSyncBody(), &source, SYNC_HDR);
    if ((ret < 200) || (ret > 299)) {
        goto finally;
    }

    while((a = getCommand(syncml->getSyncBody(), SYNC, iterator)) != NULL){  
        sync = (Sync*)a;
        const char *locuri = ((Target*)(sync->getTarget()))->getLocURI();
        if (strcmp(locuri, _wcc(source.getName())) == 0) {                   
            
            //
            // To handle the NumberOfChanges. The default is -1 that means the server doesn't send
            // any tag <NumberOfChanges>. Whit value >= 0 the value is correct
            //
    
            long noc = sync->getNumberOfChanges();
            fireSyncSourceEvent(source.getConfig().getURI(), source.getConfig().getName(), source.getSyncMode(), noc, SYNC_SOURCE_TOTAL_SERVER_ITEMS);

            break;
        }  
        sync = NULL;
        iterator++;
    }
        
finally:
    
    return sync;
 
}

/*
 * Processes the map message response. Returns 0 in case of success, an
 * error code in case of error.
 * It feeds the given source with the server side modifications
 *
 * @param source the source
 * @param msg the response from the server
 */
int SyncMLProcessor::processMapResponse(SyncSource& source, SyncBody* syncBody) {
    int ret = -1;

    //
    // for now it is always ok
    //
    //
    // First of all check the status for the SyncHead
    //
    // TBD
    ret = getStatusCode(syncBody, &source, SYNC_HDR);

    // Fire Sync Status Event: map status from server (TBD)
    //fireSyncStatusEvent(MAP, ret, source.getConfig().getURI(), NULL, SERVER_STATUS); 

    if ((ret < 200) || (ret >299)) {
        goto finally;
    }

    ret = 0;

finally:

    return ret;
}

/*
 * Returns the SyncHeader/RespURI element of the given message. If the element is not
 * found it returns NULL. The returned respURI is allocated with the new operator
 * and must be discarded with delete by the caller.
 *
 * @param SyncHdr - the SyncHdr object - NOT NULL
 */
char* SyncMLProcessor::getRespURI(SyncHdr* syncHdr) {
    
    char* respURI = NULL;
    
    if (syncHdr == NULL) {
        goto finally;
    }
    respURI = stringdup(syncHdr->getRespURI(NULL));
    
finally:

    return respURI;
}


Chal* SyncMLProcessor::getChal(SyncBody* syncBody) {    
    
    ArrayList* list = syncBody->getCommands();
    char* name = NULL;
    Status* s     = NULL;
    Chal* chal    = NULL;

    for (int i = 0; i < list->size(); i++) {
        name = ((AbstractCommand*)(list->get(i)))->getName();    // is returned the pointer to the element not a new element
        if (name && strcmp(name, STATUS) == 0) {
            s = (Status*)list->get(i);
            if (strcmp(s->getCmd(NULL), SYNC_HDR) == 0) {
                if (strcmp(s->getCmdRef(NULL), T("0")) != 0) {

                    sprintf(lastErrorMsg, T("Status/CmdRef either not found or not referring to SyncHeader!"));
                    lastErrorCode = ERR_REPRESENTATION;
                    goto finally;
                }      
                
                chal = s->getChal();
                if (chal == NULL) {
                    //
                    // no chal found
                    //
                    goto finally;
                }                
                break;
            }                                 
        }    
    }    
   
finally:
   
    return chal;
} 

/*
* Return an array list of commands of the given command name. It return an ArrayList that have to be 
* discarded by the caller
*/
ArrayList* SyncMLProcessor::getCommands(SyncBody* syncBody, char* commandName) {
        
    ArrayList* ret = new ArrayList();
    AbstractCommand* a = NULL;

    for (int i = 0; i < syncBody->getCommands()->size(); i++) {    
        a = getCommand(syncBody, commandName, i);
        if (a) 
            ret->add(*a);
    }
    return ret;
}

// ------------------------------------------------------------- Private methods

/*
* To get a generic array element. It returns the <index> arrayElement it founds.
* 0-based.
*/
ArrayElement* SyncMLProcessor::getArrayElement(ArrayList* list, int index) {
    
    if (list == NULL)
        return NULL;

    ArrayElement* a     = NULL;    
    int count           = 0;
    for (int i = 0; i < list->size(); i++) {               
            if (count == index) {
                a = list->get(i);
                break;
            }        
            ++ count;         
    }   
    return a;
}

/*
* Return the index number of occurrence of this command. If doesn't exists return NULL;
* The first command has number 0.
*/
AbstractCommand* SyncMLProcessor::getCommand(SyncBody* syncBody, char* commandName, int index) {
      
    int iterator = 0, found = 0;
    ArrayList* list     = syncBody->getCommands();
    int l = list->size();
    AbstractCommand* a  = NULL;
    char* name = NULL;
    do {
        a = (AbstractCommand*)getArrayElement(list, iterator);
        if (a) {
            name = a->getName();    // is returned the pointer to the element not a new element
            if (name && strcmp(name, commandName) == 0) {                       
                if (found == index)
                    break;
                else
                    found++;
            }    
        }        
        ++iterator;
    } while(a);
    
    return a;
}


int SyncMLProcessor::getStatusCode(SyncBody* syncBody, SyncSource* source, char* commandName) {    
    int ret = -1;
    
    ArrayList* list = syncBody->getCommands();
    char* name = NULL;
    Status* s     = NULL;
    Data* data    = NULL;

    for (int i = 0; i < list->size(); i++) {
        name = ((AbstractCommand*)(list->get(i)))->getName();    // is returned the pointer to the element not a new element
        if (name && strcmp(name, STATUS) == 0) {
            s = (Status*)list->get(i);
            if (strcmp(s->getCmd(), commandName) == 0) {
                if (strcmp(commandName, SYNC_HDR) == 0) {
                    ret = getSyncHeaderStatusCode(s);
                } else if (strcmp(commandName, ALERT) == 0) {
                    ret = getAlertStatusCode(s, (char*)source->getName());
                }    
                break;
            }                                 
        }    
    }    
   
    return ret;

}

/*
 * Returns the status code for the SyncHeader command included
 * in the message sent by the client.
 *
 * @param syncBody - the SyncBody content
 */
int SyncMLProcessor::getSyncHeaderStatusCode(Status* s) {
    
    int ret = -1;    
    Data* data    = NULL;
   
    if (s == NULL) 
        goto finally;

    if (strcmp(s->getCmdRef(NULL), T("0")) != 0) {

        sprintf(lastErrorMsg, T("Status/CmdRef either not found or not referring to SyncHeader!"));
        lastErrorCode = ERR_REPRESENTATION;
        goto finally;
    }                
    
    data = s->getData();
    if (data->getData(NULL) == NULL) {
         //
        // It should not happen
        //
        sprintf(lastErrorMsg, T("Status/Data not found!"));
        lastErrorCode = ERR_REPRESENTATION;
        goto finally;
    }
    ret = strtol(data->getData() , NULL, 10);    
                                

finally:
   
    return ret;
} 

/*
 * Returns the status code for the Alert relative to the given source.
 *
 * @param syncBody - the SyncBody content
 * @param sourceName - the name of the source
 */

 int SyncMLProcessor::getAlertStatusCode(Status* s, const char* sourceName) {
    int ret = -1;
    
    Data* data    = NULL;
    ArrayList* sourceRefs = new ArrayList();
    
    if (s == NULL)
        goto finally;

    sourceRefs = s->getSourceRef();
    if (strcmp(((SourceRef*)(sourceRefs->get(0)))->getValue(), sourceName) == 0) {                    
        data = s->getData();
        if (data->getData(NULL) == NULL) {
            //
            // It should not happen
            //
            sprintf(lastErrorMsg, T("Status/Data not found!"));
            lastErrorCode = ERR_REPRESENTATION;
            goto finally;
        }
        ret = strtol(data->getData(), NULL, 10);        
    }                

finally:
    return ret;

}

