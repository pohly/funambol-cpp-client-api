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


/*
 * This class is responsible for the processing of the incoming messages.
 */
/*
 * Constructor
 */
SyncMLProcessor::SyncMLProcessor() : XMLProcessor() {
}

SyncML* SyncMLProcessor::processMsg(wchar_t* msg) {
    SyncML* syncml      = Parser::getSyncML(msg);
    return syncml;
}


int SyncMLProcessor::processSyncHdrStatus(SyncML* syncml) {
    return getStatusCode(syncml->getSyncBody(), NULL, SYNC_HDR);
}

int SyncMLProcessor::processAlertStatus(SyncSource& source, SyncML* syncml, ArrayList* alerts) {

    int ret = -1;
    ArrayList* list     = new ArrayList();
    wchar_t* name = NULL;
    Status* s     = NULL;
    Data* data    = NULL;
    SourceRef* sourceRef    = NULL;

    if (alerts->size()) {
        list = syncml->getSyncBody()->getCommands();
        
        for (int i = 0; i < list->size(); i++) {
            name = ((AbstractCommand*)(list->get(i)))->getName();    // is returned the pointer to the element not a new element
            if (name && wcscmp(name, STATUS) == 0) {
                s = (Status*)list->get(i);
                if (wcscmp(s->getCmd(NULL), ALERT) == 0) { 
                        sourceRef = (SourceRef*)(s->getSourceRef()->get(0));
                        if (wcscmp((wchar_t*)source.getName(), sourceRef->getValue()) == 0) {
                            ret = getAlertStatusCode(s, (wchar_t*)source.getName());                           
                            break;                            
                        }                    
                }                                 
            }    
        }    
    }

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
        Alert* alert = (Alert*)a;
        Item* item = NULL;
        ArrayList* itemList = alert->getItems();

        for (int i = 0; i < itemList->size(); i++) {
            item = (Item*)getArrayElement(itemList, i);

            if (wcscmp(((Target*)item->getTarget())->getLocURI(), source.getName()) == 0) {                   
                if (alert->getData() == NULL) {
                    lastErrorCode = ERR_REPRESENTATION;
                    wcsprintf(lastErrorMsg, TEXT("SyncBody/Alert/Data not found!"));
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

int SyncMLProcessor::processItemStatus(SyncSource& source, SyncBody* syncBody) {
    
    ArrayList* list  = new ArrayList();    
    ArrayList* items = NULL;
    Item* item       = NULL;
    SourceRef* sourceRef = NULL;
    Status* s        = NULL;
    wchar_t* name    = NULL;
    Data* data       = NULL;

    list = getCommands(syncBody, STATUS);

    for (int i = 0; i < list->size(); i++) {
        s = (Status*)list->get(i);
        name = s->getCmd(NULL);    
        
        if (wcscmp(name, SYNC) == 0 && getAlertStatusCode(s, (wchar_t*)source.getName()) < 0) {
            break;
        }         

        data = s->getData();
        if (wcscmp(name, ADD) == 0 || wcscmp(name, REPLACE) == 0 || wcscmp(name, DEL) == 0) {
            int k;

            items = s->getItems();
            for (k = 0; k < items->size(); k++) {
                item = (Item*)items->get(k);
                if (item) {
                    source.setItemStatus(item->getSource()->getLocURI(), wcstol(data->getData(NULL) , NULL, 10));                       
                }
            }
            items = s->getSourceRef();
            for (k = 0; k < items->size(); k++) {
                sourceRef = (SourceRef*)items->get(k);
                if (sourceRef) {
                    source.setItemStatus(sourceRef->getValue(), wcstol(data->getData(NULL) , NULL, 10));                       
                }
            }
        }
    }
    deleteArrayList(&list);    
    return 0;
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
        if (wcscmp(((Target*)sync->getTarget())->getLocURI(), source.getName()) == 0) {                   
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
wchar_t* SyncMLProcessor::getRespURI(SyncHdr* syncHdr) {
    
    wchar_t* respURI = NULL;
    
    if (syncHdr == NULL) {
        goto finally;
    }
    respURI = stringdup(syncHdr->getRespURI(NULL));
    
finally:

    return respURI;
}


Chal* SyncMLProcessor::getChal(SyncBody* syncBody) {    
    
    ArrayList* list = syncBody->getCommands();
    wchar_t* name = NULL;
    Status* s     = NULL;
    Chal* chal    = NULL;

    for (int i = 0; i < list->size(); i++) {
        name = ((AbstractCommand*)(list->get(i)))->getName();    // is returned the pointer to the element not a new element
        if (name && wcscmp(name, STATUS) == 0) {
            s = (Status*)list->get(i);
            if (wcscmp(s->getCmd(NULL), SYNC_HDR) == 0) {
                if (wcscmp(s->getCmdRef(NULL), TEXT("0")) != 0) {

                    wcsprintf(lastErrorMsg, TEXT("Status/CmdRef either not found or not referring to SyncHeader!"));
                    lastErrorCode = ERR_REPRESENTATION;
                    goto finally;
                }      
                
                chal = s->getChal();
                if (chal == NULL) {
                     //
                    // It should not happen
                    //
                    wcsprintf(lastErrorMsg, TEXT("Status/Data not found!"));
                    lastErrorCode = ERR_REPRESENTATION;
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
ArrayList* SyncMLProcessor::getCommands(SyncBody* syncBody, wchar_t* commandName) {
        
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
AbstractCommand* SyncMLProcessor::getCommand(SyncBody* syncBody, wchar_t* commandName, int index) {
      
    int iterator = 0, found = 0;
    ArrayList* list     = syncBody->getCommands();
    int l = list->size();
    AbstractCommand* a  = NULL;
    wchar_t* name = NULL;
    do {
        a = (AbstractCommand*)getArrayElement(list, iterator);
        if (a) {
            name = a->getName();    // is returned the pointer to the element not a new element
            if (name && wcscmp(name, commandName) == 0) {                       
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


int SyncMLProcessor::getStatusCode(SyncBody* syncBody, SyncSource* source, wchar_t* commandName) {
    int ret = -1;
    
    ArrayList* list = syncBody->getCommands();
    wchar_t* name = NULL;
    Status* s     = NULL;
    Data* data    = NULL;

    for (int i = 0; i < list->size(); i++) {
        name = ((AbstractCommand*)(list->get(i)))->getName();    // is returned the pointer to the element not a new element
        if (name && wcscmp(name, STATUS) == 0) {
            s = (Status*)list->get(i);
            if (wcscmp(s->getCmd(NULL), commandName) == 0) {
                if (wcscmp(commandName, SYNC_HDR) == 0) {
                    ret = getSyncHeaderStatusCode(s);
                } else if (wcscmp(commandName, ALERT) == 0) {
                    ret = getAlertStatusCode(s, (wchar_t*)source->getName());
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

    if (wcscmp(s->getCmdRef(NULL), TEXT("0")) != 0) {

        wcsprintf(lastErrorMsg, TEXT("Status/CmdRef either not found or not referring to SyncHeader!"));
        lastErrorCode = ERR_REPRESENTATION;
        goto finally;
    }                
    
    data = s->getData();
    if (data->getData(NULL) == NULL) {
         //
        // It should not happen
        //
        wcsprintf(lastErrorMsg, TEXT("Status/Data not found!"));
        lastErrorCode = ERR_REPRESENTATION;
        goto finally;
    }
    ret = wcstol(data->getData(NULL) , NULL, 10);    
                                

finally:
   
    return ret;
} 

/*
 * Returns the status code for the Alert relative to the given source.
 *
 * @param syncBody - the SyncBody content
 * @param sourceName - the name of the source
 */

 int SyncMLProcessor::getAlertStatusCode(Status* s, wchar_t* sourceName) {
    int ret = -1;
    
    Data* data    = NULL;
    ArrayList* sourceRefs = new ArrayList();
    
    if (s == NULL)
        goto finally;

    sourceRefs = s->getSourceRef();
    if (wcscmp(((SourceRef*)(sourceRefs->get(0)))->getValue(), sourceName) == 0) {                    
        data = s->getData();
        if (data->getData(NULL) == NULL) {
            //
            // It should not happen
            //
            wcsprintf(lastErrorMsg, TEXT("Status/Data not found!"));
            lastErrorCode = ERR_REPRESENTATION;
            goto finally;
        }
        ret = wcstol(data->getData(NULL), NULL, 10);        
    }                

finally:
    return ret;

}

