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
#include "base/fscapi.h"
#include "base/Log.h"
#include "base/debug.h"
#include "base/util/utils.h"
#include "base/messages.h"
#include "http/TransportAgentFactory.h"
#include "spds/constants.h"
#include "spds/DataTransformer.h"
#include "spds/DataTransformerFactory.h"
#include "spds/SyncManagerConfig.h"
#include "spds/SyncManager.h"
#include "spds/SyncMLProcessor.h"
#include "spds/spdsutils.h"
#include "syncml/core/TagNames.h"
#include "syncml/core/ObjectDel.h"


/**
 * Is the given status code an error status code? Error codes are the ones
 * outside the range 200-299.
 *
 * @param status the status code to check
 */
BOOL isErrorStatus(int status) {
    return (status) && ((status < 200) || (status > 299));
}

BOOL isAuthFailed(int status) {
    return (status) && ((status == 401) || (status == 407));
}

BOOL isToExit(int* check, int size) {
    BOOL ret = TRUE;
    for (int i = 0; i < size; i++) {
        if (check[i] == 1) 
            ret = FALSE; 
    }
    return ret;
}


SyncManager::SyncManager(SyncManagerConfig* c) : config(*c) {
    initialize();
}

SyncManager::SyncManager(SyncManagerConfig& c) : config(c) {
    initialize();
}

void SyncManager::initialize() {
    AccessConfig& c = config.getAccessConfig();    
    credentialHandler = CredentialHandler();

    wcsncpy(syncURL, c.getSyncURL(), 511);
    wcsncpy(deviceId, c.getDeviceId(), 31);
    
    credentialHandler.setUsername           (c.getUsername());
    credentialHandler.setPassword           (c.getPassword());
    credentialHandler.setClientNonce        (c.getClientNonce());
    credentialHandler.setClientAuthType     (c.getClientAuthType());

    credentialHandler.setServerID           (c.getServerID());
    credentialHandler.setServerPWD          (c.getServerPWD());
    credentialHandler.setServerNonce        (c.getServerNonce());
    credentialHandler.setServerAuthType     (c.getServerAuthType());
    credentialHandler.setServerAuthRequired (c.getServerAuthRequired());
            
    transportAgent = NULL;

    currentState = STATE_START;

    commands = new ArrayList();
    mappings      = NULL;
    check         = NULL;
    sourcesNumber = 0;
    count         = 0;

    maxMsgSize   = c.getMaxMsgSize();
    maxModPerMsg = 150;  // dafault value

    if (c.getMaxModPerMsg() > 0)
        maxModPerMsg = c.getMaxModPerMsg();
    
    syncMLBuilder.set(syncURL, deviceId, maxMsgSize);
    wmemset(credentialInfo, 0, 256);
}

SyncManager::~SyncManager() {
    if (transportAgent) {
        delete transportAgent;
    }
    if (commands) {
        commands->clear();
    }
    if (check) {
        // delete check
    }
    if (mappings) {
        // delete mappings
    }
}

/*
* Modification to performe the sync of an array of sync sources.
*/

int SyncManager::prepareSync(SyncSource** s) {
    
    wchar_t* initMsg            = NULL;
    wchar_t* respURI            = NULL;
    wchar_t* responseMsg        = NULL;
    SyncML*  syncml             = NULL;
    int ret                     = 0;
    int serverRet               = 0;
    int count                   = 0;    
    const wchar_t* requestedAuthType  = NULL;
    ArrayList* list             = new ArrayList();
    ArrayList* alerts           = new ArrayList();
    
    // for authentication improvments
    BOOL isServerAuthRequired   = credentialHandler.getServerAuthRequired();
    int clientAuthRetries       = 1;
    int serverAuthRetries       = 1;    
    int authStatusCode          = 200;

    BOOL isClientAuthenticated  = FALSE;
    BOOL isServerAuthenticated  = FALSE;
    Chal*   clientChal          = NULL; // The chal of the server to the client
    Chal*   serverChal          = NULL; // The chal of the client to the server
    Status* status              = NULL; // The status from the client to the server
    Cred*   cred                = NULL;
    Alert*  alert               = NULL;
    SyncSource** buf            = NULL;
    const wchar_t *syncURL;
    
    syncURL = config.getAccessConfig().getSyncURL(); //sizeof(syncURL));

    URL url(syncURL);
    Proxy proxy = Proxy();
    
    const wchar_t* sourceName = NULL;
    
    sourcesNumber = assignSources(s);
    check = new int[sourcesNumber + 1];
    mappings = new ArrayList*[sourcesNumber + 1];
    for (count = 0; count < sourcesNumber; count++) {
        check[count] = 1;
        mappings[count] = new ArrayList();
    }
    check[sourcesNumber] = 0;

    // --- INFO
    wcsprintf(logmsg, MSG_SYNC_URL, syncURL);
    LOG.info(logmsg);
    for (count = 0; count < sourcesNumber; count++) {
        wcsprintf(logmsg, MSG_PREPARING_SYNC, sources[count]->getName());
        LOG.info(logmsg);
    }
    // ---

    //
    syncMLBuilder.resetCommandID();
    syncMLBuilder.resetMessageID();
    unsigned long timestamp = (unsigned long)time(NULL);
    config.getAccessConfig().setBeginSync(timestamp);
    for (count = 0; count < sourcesNumber; count ++) {
        if (readSyncSourceDefinition(*sources[count]) == false) {
        ret = lastErrorCode = ERR_SOURCE_DEFINITION_NOT_FOUND;
            wcsprintf(lastErrorMsg, ERRMSG_SOURCE_DEFINITION_NOT_FOUND, sources[count]->getName());
            LOG.debug(lastErrorMsg);
            check[count] = 0;
        }
    }
    
    for (count = 0; count < sourcesNumber; count ++) {
        if (!check[count])
            continue;

        if (sources[count]->getSyncMode() == SYNC_NONE) {
            check[count] = 0;
        }
    }
        
    if (isToExit(check, sourcesNumber)) {
        // error. no source to sync
        ret = -1;
        goto finally;
    }
          
    if (isServerAuthRequired == FALSE) 
        isServerAuthenticated = TRUE;

    do {
        deleteCred(&cred);
        deleteAlert(&alert);
        deleteSyncML(&syncml);
        deleteArrayList(&alerts);

        bool addressChange = false;

        // credential of the client
        if (isClientAuthenticated == FALSE) {
            wchar_t anc[DIM_ANCHOR];
            timestamp = (unsigned long)time(NULL);
            for (count = 0; count < sourcesNumber; count ++) {
                if (!check[count])
                    continue;
                sources[count]->setNextSync(timestamp);
                timestampToAnchor(sources[count]->getNextSync(), anc);
                sources[count]->setNextAnchor(anc);
                // Test if this source is for AddressChangeNotification
                int prefmode = sources[count]->getPreferredSyncMode();
                if( prefmode == SYNC_ADDR_CHANGE_NOTIFICATION ) {
                    alert = syncMLBuilder.prepareAddrChangeAlert(*sources[count]);
                    if(!alert) {
                        ret = lastErrorCode = 745; // FIXME
                        goto finally;
                    }
                    addressChange = true;   // remember that this sync is for
                                            // address change notification
                }
                else {
                    alert = syncMLBuilder.prepareInitAlert(*sources[count]);
                }
                alerts->add(*alert);
                deleteAlert(&alert);
            }                        
            cred = credentialHandler.getClientCredential();             
            wcscpy(credentialInfo, cred->getAuthentication()->getData(NULL)); LOG.debug(credentialInfo);
        }
        syncml = syncMLBuilder.prepareInitObject(cred, alerts, commands);
        if (syncml == NULL) {
            ret = lastErrorCode;
            goto finally;
        }  
       
        initMsg = syncMLBuilder.prepareMsg(syncml);
        if (initMsg == NULL) {
            ret = lastErrorCode;
            goto finally;
        }        

        LOG.debug(MSG_INITIALIZATATION_MESSAGE);
        LOG.debug(initMsg);

        currentState = STATE_PKG1_SENDING;

        if (transportAgent == NULL) {
            transportAgent = TransportAgentFactory::getTransportAgent(url, proxy);
            if (maxMsgSize > 0) {
                transportAgent->setMaxMsgSize(maxMsgSize);
            }
        } else {
            transportAgent->setURL(url);
        }
                
        deleteSyncML(&syncml);         
        deleteChal(&serverChal);
        deleteArrayList(&commands);
        deleteCred(&cred);

        responseMsg = transportAgent->sendMessage(initMsg);
        if (responseMsg == NULL) {
            // This is an error only is it is not an AddressChange
            if ( addressChange && lastErrorCode == ERR_READING_CONTENT ) {
                ret = 0;
            }
            else {
                ret = lastErrorCode;
            }
            goto finally;
        }

        // increment the msgRef after every send message
        syncMLBuilder.increaseMsgRef();
        syncMLBuilder.resetCommandID();

        syncml = syncMLProcessor.processMsg(responseMsg);
        if (syncml == NULL) {
            ret = lastErrorCode;
            goto finally;
        }

        // ret = syncMLProcessor.processInitResponse(*sources[0], syncml, alerts);

        ret = syncMLProcessor.processSyncHdrStatus(syncml);

        if (ret == -1) {
            ret = lastErrorCode;
            goto finally;

        } else if (isErrorStatus(ret) && ! isAuthFailed(ret)) {
            lastErrorCode = ret;
            goto finally;
        }

        for (count = 0; count < sourcesNumber; count ++) {
            if (!check[count])
                continue;

            ret = syncMLProcessor.processAlertStatus(*sources[count], syncml, alerts);
            if (ret == -1 || ret == 404) {
                check[count] = 0;   
            }
        } 
        if (isToExit(check, sourcesNumber)) {
            // error. no source to sync
            ret = -1;
            goto finally;
        }
        
        //
        // Set the uri with session
        //
        respURI = syncMLProcessor.getRespURI(syncml->getSyncHdr());
        if (respURI) {
            url = respURI;
            transportAgent->setURL(url);
        }
        //
        // Server Authentication
        //
        if (isServerAuthenticated == FALSE) {
            
            cred = syncml->getSyncHdr()->getCred();
            if (cred == NULL) {
                if (serverAuthRetries == 1) {
                    // create the serverNonce if needed and set into the CredentialHendler, serverNonce property
                    serverChal = credentialHandler.getServerChal(isServerAuthenticated); 
                    authStatusCode = 407;                    
                    serverAuthRetries++;
                } else {
                     ret = -1;
                     goto finally;
                }

            } else {                
                isServerAuthenticated = credentialHandler.performServerAuth(cred);                                
                if (isServerAuthenticated) {
                    serverChal   = credentialHandler.getServerChal(isServerAuthenticated); 
                    authStatusCode = 212;                    
                }
                else {
                    if (wcscmp(credentialHandler.getServerAuthType(NULL), AUTH_TYPE_MD5) == 0 || serverAuthRetries == 1) {
                        serverChal   = credentialHandler.getServerChal(isServerAuthenticated);
                        authStatusCode = 401;                        
                        
                    } else {
                        ret = -1;
                        goto finally;
                    }
                    serverAuthRetries++;
                }
            } 
            cred = NULL; // this cred is only a reference
        } else  {
            authStatusCode = 200;            
        }
        status = syncMLBuilder.prepareSyncHdrStatus(serverChal, authStatusCode);
        commands->add(*status);
        deleteStatus(&status);
        list = syncMLProcessor.getCommands(syncml->getSyncBody(), ALERT);    
        for (count = 0; count < sourcesNumber; count ++) {
            if (!check[count])
                continue;

            status = syncMLBuilder.prepareAlertStatus(*sources[count], list, authStatusCode);    
            if (status) {
                commands->add(*status);
                deleteStatus(&status);    
            }
        }  
        
        //
        // Client Authentication. The auth of the client on the server
        //
        clientChal = syncMLProcessor.getChal(syncml->getSyncBody());

        if (isAuthFailed(ret)) {                        
            if (clientChal == NULL) {
                requestedAuthType = credentialHandler.getClientAuthType();
            } else {
                requestedAuthType = clientChal->getType(NULL);
            }
            if (wcscmp(credentialHandler.getClientAuthType(NULL),requestedAuthType) != 0 ) {           
                if (clientChal && wcscmp(requestedAuthType, AUTH_TYPE_MD5) == 0) {
                    credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                }                 
            } else { 
                if (wcscmp(requestedAuthType, AUTH_TYPE_MD5) == 0 && clientAuthRetries == 1)  {
                    credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                 
                } else {
                    ret = 401;
                    goto finally;
                }                                    
            }
            credentialHandler.setClientAuthType(requestedAuthType); 
            clientAuthRetries++;            

       } else {            
            if (clientChal && wcscmp(clientChal->getType(NULL), AUTH_TYPE_MD5) == 0) {                    
                credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());                
            }
            isClientAuthenticated = TRUE;
            for (count = 0; count < sourcesNumber; count ++) {
                if (!check[count])
                    continue;
                ret = syncMLProcessor.processServerAlert(*sources[count], syncml);
                if (isErrorStatus(ret)) {
                    check[count] = 0;   
                }
            }  
       }
    
    } while(isClientAuthenticated == FALSE || isServerAuthenticated == FALSE);

    config.getAccessConfig().setClientNonce(credentialHandler.getClientNonce(NULL));
    config.getAccessConfig().setServerNonce(credentialHandler.getServerNonce(NULL));
    
    if (isToExit(check, sourcesNumber)) {
        // error. no source to sync
        ret = -1;
        goto finally;
    }

    currentState = STATE_PKG1_SENT;
    
// ---------------------------------------------------------------------------------------
finally:

    if (respURI) {
        delete [] respURI;
    }
    if (responseMsg) {
        safeDelete(&responseMsg);
    }
    if (initMsg) {
        safeDelete(&initMsg);
    }

    deleteSyncML(&syncml);
    deleteCred(&cred);
    deleteAlert(&alert);
    deleteStatus(&status);
    deleteChal(&serverChal);    
    return ret;
}

int SyncManager::sync() {

    wchar_t* msg         = NULL;
    wchar_t* responseMsg = NULL;
    Status* status       = NULL;
    SyncML* syncml       = NULL;
    SyncItem* syncItem   = NULL;
    SyncItem* sItem      = NULL;
    Alert* alert         = NULL;
    ModificationCommand* modificationCommand = NULL;
    unsigned int tot     = 0; 
    unsigned int step    = 0; 
    unsigned int toSync  = 0;
    unsigned int iterator= 0;
    int ret              = 0;  
    BOOL last            = FALSE;
    ArrayList* list      = new ArrayList();
    BOOL isFinalfromServer = FALSE;

    //for refresh from server sync
    allItemsList = new ArrayList*[sourcesNumber];

    //
    // If this is the first message, currentState is STATE_PKG1_SENT,
    // otherwise it is already in STATE_PKG3_SENDING.
    //
    if (currentState == STATE_PKG1_SENT) {
        currentState = STATE_PKG3_SENDING;
    }

    // The real number of source to sync
    for (count = 0; count < sourcesNumber; count ++) {
        allItemsList[count] = NULL;
        if (!check[count])
            continue;
        toSync++;
    }

    for (count = 0; count < sourcesNumber; count ++) {
        if (!check[count])
            continue;

        tot  = 0;
        step = 0;
        last = FALSE;        
        iterator++;

        if ( sources[count]->beginSync() ) {
            // Error from SyncSource
            lastErrorCode = ERR_UNSPECIFIED;
            ret = lastErrorCode;
            check[count]=0;
            continue;
        }

        do {

            //
            // Sets the proper encoding for the content of the items.
            //
            if (config.isEncryption()) {
                syncMLBuilder.setEncPassword(credentialInfo);
                syncMLBuilder.setEncoding(DESB64);
            } else {
                // TBD: if we are using vcard/icalendar, we need to 
                // set the encoding to PLAIN
                if (wcscmp(sources[count]->getEncoding(), B64_ENCODING) == 0) {
                    syncMLBuilder.setEncoding(B64);
                } else {
                    syncMLBuilder.setEncoding(PLAIN);
                }   

            }

            if (modificationCommand) {
                delete modificationCommand; 
                modificationCommand = NULL;
            }

            if (commands->isEmpty()) {

                status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
                commands->add(*status);
                deleteStatus(&status);

                /* The server should not send any alert...
                   list = syncMLProcessor.getCommands(syncml->getSyncBody(), ALERT);   
                   status = syncMLBuilder.prepareAlertStatus(*sources[0], list, 200);

                   if (status) {
                   commands->add(*status);
                   deleteStatus(&status);    
                   }
                   deleteArrayList(&list);
                 */
            }

            deleteSyncML(&syncml);

            Sync* sync = syncMLBuilder.prepareSyncCommand(*sources[count]);
            ArrayList* list = new ArrayList();

            switch (sources[count]->getSyncMode()) {
                case SYNC_SLOW: {   
                                    sItem = NULL;
                                    if (tot == 0) {                    
                                        sItem = sources[count]->getFirstItem();                        
                                        if (sItem) {
                                            syncItem = (SyncItem*)sItem->clone();
                                        }
                                    }
                                    tot = 0;
                                    do {
                                        if (syncItem == NULL) {
                                            sItem = sources[count]->getNextItem();
                                            if (sItem)
                                                syncItem = (SyncItem*)sItem->clone();                     
                                        }
                                        if (modificationCommand == NULL) {
                                            modificationCommand = syncMLBuilder.prepareModificationCommand(REPLACE_COMMAND_NAME, 
                                                    syncItem, sources[count]->getType());
                                        } else {
                                            syncMLBuilder.addItem(modificationCommand, REPLACE_COMMAND_NAME, syncItem, 
                                                    sources[count]->getType());
                                        }

                                        if (syncItem) {
                                            delete syncItem; syncItem = NULL;// the item is only the pointer not another instance. to save mem                        
                                        }
                                        else {
                                            last = TRUE;
                                            break;
                                        }
                                        tot++;
                                        sItem = NULL;
                                    } while( tot < maxModPerMsg);                                                    
                                }; break;

                case SYNC_REFRESH_FROM_SERVER:
                                last = TRUE; 

                                allItemsList[count] = new ArrayList();
                                sItem = sources[count]->getFirstItemKey();
                                if(sItem) {
                                    allItemsList[count]->add((ArrayElement&)*sItem);
                                    delete sItem;
                                }
                                sItem = sources[count]->getNextItemKey();
                                while(sItem) {
                                    allItemsList[count]->add((ArrayElement&)*sItem);
                                    delete sItem;
                                    sItem = sources[count]->getNextItemKey();	
                                } 

                                break;

                case SYNC_ONE_WAY_FROM_SERVER:
                                last = TRUE;
                                break;

                case SYNC_REFRESH_FROM_CLIENT:
                    {   
                        sItem = NULL;
                        if (tot == 0) {                    
                            sItem = sources[count]->getFirstItem();                        
                            if (sItem) {
                                syncItem = (SyncItem*)sItem->clone();
                            }
                        }
                        tot = 0;
                        do {
                            if (syncItem == NULL) {
                                sItem = sources[count]->getNextItem();
                                if (sItem)
                                    syncItem = (SyncItem*)sItem->clone();                     
                            }
                            if (modificationCommand == NULL) {
                                modificationCommand = syncMLBuilder.prepareModificationCommand(REPLACE_COMMAND_NAME, 
                                        syncItem, sources[count]->getType());
                            } else {
                                syncMLBuilder.addItem(modificationCommand, REPLACE_COMMAND_NAME, syncItem, 
                                        sources[count]->getType());
                            }

                            if (syncItem) {
                                delete syncItem; syncItem = NULL;// the item is only the pointer not another instance. to save mem                        
                            }
                            else {
                                last = TRUE;
                                break;
                            }
                            tot++;
                            sItem = NULL;
                        } while( tot < maxModPerMsg);                                                    
                    }
                    break;

                default:
                    {    
                        tot = 0;
                        //
                        // New Item
                        //
                        sItem = NULL;
                        if (step == 0) {
                            sItem = sources[count]->getFirstNewItem(); 
                            if (sItem)
                                syncItem = (SyncItem*)sItem->clone();
                            step++;
                            if (syncItem == NULL)
                                step++;                        
                        }
                        if (step == 1) {                                                          
                            do {
                                if (syncItem == NULL) {
                                    sItem = sources[count]->getNextNewItem();
                                    if (sItem)   
                                        syncItem = (SyncItem*)sItem->clone(); 
                                }
                                if (modificationCommand == NULL) {
                                    modificationCommand = syncMLBuilder.prepareModificationCommand(ADD_COMMAND_NAME, 
                                            syncItem, sources[count]->getType());
                                } else {
                                    syncMLBuilder.addItem(modificationCommand, ADD_COMMAND_NAME, 
                                            syncItem, sources[count]->getType());
                                }

                                if (syncItem) {
                                    delete syncItem; syncItem = NULL;
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
                                sItem = NULL;
                            } while( tot < maxModPerMsg);
                        }

                        //
                        // Updated Item
                        //
                        if (step == 2) {

                            if (modificationCommand) {
                                list->add(*modificationCommand);
                                delete modificationCommand; 
                                modificationCommand = NULL;
                            }

                            sItem = sources[count]->getFirstUpdatedItem();
                            if (sItem)
                                syncItem = (SyncItem*)sItem->clone();

                            step++;
                            if (syncItem == NULL)
                                step++;    
                        }
                        if (step == 3) {
                            do {
                                if (syncItem == NULL) {
                                    sItem = sources[count]->getNextUpdatedItem();
                                    if (sItem)
                                        syncItem = (SyncItem*)sItem->clone();

                                }
                                if (modificationCommand == NULL) {
                                    modificationCommand = syncMLBuilder.prepareModificationCommand(REPLACE_COMMAND_NAME, 
                                            syncItem, sources[count]->getType());
                                } else {
                                    syncMLBuilder.addItem(modificationCommand, REPLACE_COMMAND_NAME, 
                                            syncItem, sources[count]->getType());
                                }

                                if (syncItem) {                            
                                    delete syncItem; syncItem = NULL;
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
                                sItem = NULL;
                            } while( tot < maxModPerMsg);                                                      
                        } 

                        //
                        // Deleted Item
                        //
                        if (step == 4) {

                            if (modificationCommand) {
                                list->add(*modificationCommand);
                                delete modificationCommand; 
                                modificationCommand = NULL;
                            }

                            sItem = sources[count]->getFirstDeletedItem();

                            if (sItem)
                                syncItem = (SyncItem*)sItem->clone();

                            step++;
                            if (syncItem == NULL)
                                step++;    
                        }
                        if (step == 5) {
                            do {
                                if (syncItem == NULL) {
                                    sItem = sources[count]->getNextDeletedItem(); 
                                    if (sItem)
                                        syncItem = (SyncItem*)sItem->clone();                        
                                }
                                if (modificationCommand == NULL) {
                                    modificationCommand = syncMLBuilder.prepareModificationCommand(DELETE_COMMAND_NAME, 
                                            syncItem, sources[count]->getType());
                                } else {
                                    syncMLBuilder.addItem(modificationCommand, DELETE_COMMAND_NAME, 
                                            syncItem, sources[count]->getType());
                                }

                                if (syncItem) {                            
                                    delete syncItem; syncItem = NULL;
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
                                sItem = NULL;
                            } while( tot < maxModPerMsg);           
                        }
                        if (step == 6 && syncItem == NULL)
                            last = TRUE;

                        break;
                    }
            }

            if (modificationCommand) {
                list->add(*modificationCommand);
                delete modificationCommand; 
                modificationCommand = NULL;
            }                     
            sync->setCommands(list);
            commands->add(*sync);

            //
            // Check if all the sources were synced. If not the prepareSync doesn't use the <final/> tag 
            //
            syncml = syncMLBuilder.prepareSyncML(commands, (iterator != toSync ? FALSE : last));                        

            msg    = syncMLBuilder.prepareMsg(syncml);       

            deleteSyncML(&syncml);
            deleteArrayList(&commands);

            if (msg == NULL) {
                ret = lastErrorCode;
                goto finally;
            }

            // Synchronization message:

            LOG.debug(MSG_MODIFICATION_MESSAGE);
            LOG.debug(msg);

            responseMsg = transportAgent->sendMessage(msg);
            if (responseMsg == NULL) {
                ret=lastErrorCode;
                goto finally;
            }
            // increment the msgRef after every send message
            syncMLBuilder.increaseMsgRef();
            syncMLBuilder.resetCommandID();

            syncml = syncMLProcessor.processMsg(responseMsg);
            safeDelete(&responseMsg);
            safeDelete(&msg);

            if (syncml == NULL) {
                ret = lastErrorCode;
                goto finally;
            }               

            isFinalfromServer = syncml->isLastMessage();
            ret = syncMLProcessor.processSyncHdrStatus(syncml);

            if (isErrorStatus(ret)) {
                ret = lastErrorCode;
                wsprintf(lastErrorMsg, TEXT("Error: error code %i"), ret);
                LOG.debug(lastErrorMsg);
                goto finally;

            }
            ret = 0;
            // 
            // Process the status of the item sent by client. It invokes the 
            // source method 
            //
            syncMLProcessor.processItemStatus(*sources[count], syncml->getSyncBody());


            // deleteSyncML(&syncml);

        } while (last == FALSE);

    } // end for (count = 0; count < sourcesNumber; count ++)

    //
    // If this was the last chunk, we move the state to STATE_PKG3_SENT
    //
    if (last) {
        currentState = STATE_PKG3_SENT;
    }        

    //
    // send 222 alert code to retrieve the item from server
    //    

    if (isFinalfromServer == FALSE)
    {
        status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
        commands->add(*status);
        deleteStatus(&status); 
        for (count = 0; count < sourcesNumber; count ++) {
            if(!check[count])
                continue;
            alert = syncMLBuilder.prepareRequestAlert(*sources[count]);
            commands->add(*alert);
            deleteAlert(&alert);
        }

        syncml = syncMLBuilder.prepareSyncML(commands, last);
        msg    = syncMLBuilder.prepareMsg(syncml);        

        LOG.debug(TEXT("Alert to request server changes"));
        LOG.debug(msg);

        responseMsg = transportAgent->sendMessage(msg);
        if (responseMsg == NULL) {
            ret=lastErrorCode;
            goto finally;
        }      
        // increment the msgRef after every send message
        syncMLBuilder.increaseMsgRef();
        syncMLBuilder.resetCommandID();

        deleteSyncML(&syncml);
        safeDelete(&msg);

        syncml = syncMLProcessor.processMsg(responseMsg);
        safeDelete(&responseMsg);
        deleteArrayList(&commands);

        if (syncml == NULL) {
            ret = lastErrorCode;
            goto finally;
        }  
        ret = syncMLProcessor.processSyncHdrStatus(syncml);

        if (isErrorStatus(ret)) {
            ret = lastErrorCode;
            wsprintf(lastErrorMsg, TEXT("Error: error code %i"), ret);
            LOG.debug(lastErrorMsg);
            goto finally;
        }
        ret = 0;

        //
        // Process the items returned from server
        //

        do {              
            last = syncml->getSyncBody()->getFinalMsg();
            int code = 0, i = 0, j = 0;             
            Item* item              = NULL;
            ArrayList* list         = new ArrayList();  
            ArrayList* statusList   = new ArrayList();
            ArrayList* items        = new ArrayList();
            Meta* meta              = NULL;
            Target* t               = NULL;
            Source* s               = NULL;
            wchar_t* commandName    = NULL;
            wchar_t* dataType       = NULL;        
            wchar_t* itemName       = NULL;
            SyncItem  syncItem;
            SyncMap*   syncMap      = NULL;
            Sync* sync              = NULL;        

            status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
            commands->add(*status);
            deleteStatus(&status); 

            /* The server should not send any alert...
               list = syncMLProcessor.getCommands(syncml->getSyncBody(), ALERT);   
               status = syncMLBuilder.prepareAlertStatus(*sources[0], list, 200);

               if (status) {
               commands->add(*status);
               deleteStatus(&status);    
               }
               deleteArrayList(&list);
             */
            for (count = 0; count < sourcesNumber; count ++) {
                if (!check[count])
                    continue;

                sync = syncMLProcessor.processSyncResponse(*sources[count], syncml);

                if (sync) {
                    // items = sync->getCommands()->clone();
                    items = sync->getCommands();
                    status = syncMLBuilder.prepareSyncStatus(*sources[count], sync);
                    statusList->add(*status);
                    deleteStatus(&status);
                }                               

                ArrayList* previousStatus = new ArrayList();
                wchar_t* format = NULL;
                for (i = 0; i < items->size(); i++) {
                    modificationCommand = (ModificationCommand*)(items->get(i));
                    meta = modificationCommand->getMeta();
                    if (meta) {
                        dataType = meta->getType(NULL);
                        format = meta->getFormat(NULL); 
                    }
                    list = modificationCommand->getItems();
                    commandName = modificationCommand->getName();    

                    for (j = 0; j < list->size(); j++) {
                        item = (Item*)list->get(j);
                        if (item == NULL)
                            goto finally;

                        //
                        // set the syncItem element
                        //
                        s = item->getSource();
                        if (s) { 
                            itemName = (wchar_t*)s->getLocURI();                        
                        } else {
                            t = item->getTarget();                
                            itemName = (wchar_t*)t->getLocURI();                        
                        }
                        syncItem = SyncItem(itemName);

                        wchar_t* toConvert = NULL;
                        char* data = NULL;
                        long size = 0;
                        if (item->getData()) {
                            toConvert = item->getData()->getData(NULL);    

                            //
                            // Retrieving how the content has been encoded
                            // and then processing the content accordingly
                            //
                            if (format == NULL) {
                                Meta* m = item->getMeta();
                                if (m) {
                                    format = m->getFormat(NULL);                            
                                }
                            } 
                            if (format) {
                                data = processItemContent(toConvert, format, &size);
                                syncItem.setData(data, size);
                                delete [] data; data = NULL;
                            } else {
                                data = toMultibyte(toConvert);
                                syncItem.setData(data, strlen(data));                        
                                delete [] data; data = NULL;
                            }
                        }
                        if (dataType) {
                            syncItem.setDataType(dataType);   
                        }
                        syncItem.setSourceParent(item->getSourceParent());
                        syncItem.setTargetParent(item->getTargetParent());

                        syncItem.setModificationTime(sources[count]->getNextSync());

                        if (commandName && wcscmp(commandName, ADD) == 0) {   
                            syncItem.setState(SYNC_STATE_NEW);
                            code = sources[count]->addItem(syncItem);      
                            status = syncMLBuilder.prepareItemStatus(ADD, itemName, 
                                    modificationCommand->getCmdID()->getCmdID(), 
                                    code);                    
                            if (code >= 200 && code <= 299) {
                                syncMap = new SyncMap((wchar_t*)item->getSource()->getLocURI(), syncItem.getKey()); 
                                mappings[count]->add(*syncMap);

                                //for refresh sync from server; delete from list the items that were already on device
                                if(allItemsList[count]) {
                                    int size = allItemsList[count]->size();
                                    for(int i = 0; i < size; i++) {
                                        SyncItem* syncItemRefresh = (SyncItem*)((SyncItem*)allItemsList[count]->get(i));
                                        if(syncItemRefresh->getKey() == syncItem.getKey()) {
                                            allItemsList[count]->remove(i); 
                                            delete syncItemRefresh;
                                        }
                                    }
                                }

                                delete syncMap; syncMap = NULL;
                            }                    
                        }
                        else if (commandName && wcscmp(commandName, REPLACE) == 0) {        
                            syncItem.setState(SYNC_STATE_UPDATED);
                            code = sources[count]->updateItem(syncItem);                
                            status = syncMLBuilder.prepareItemStatus(REPLACE, itemName, 
                                    modificationCommand->getCmdID()->getCmdID(), 
                                    code);                
                        }
                        else if (commandName && wcscmp(commandName, DEL) == 0) {
                            syncItem.setState(SYNC_STATE_DELETED);
                            code = sources[count]->deleteItem(syncItem);        
                            status = syncMLBuilder.prepareItemStatus(DEL, itemName, 
                                    modificationCommand->getCmdID()->getCmdID(), 
                                    code);            
                        }                
                        syncMLBuilder.addItemStatus(previousStatus, status);
                        deleteStatus(&status);            
                    }

                    if (previousStatus) {
                        statusList->add(previousStatus);
                        deleteArrayList(&previousStatus);                    
                    }

                }
            } // End for (count = 0; count < sourcesNumber; count ++) 

            // deleteArrayList(&items);                
            // deleteArrayList(&list);

            commands->add(statusList);

            if (!last) {
                deleteSyncML(&syncml);
                syncml = syncMLBuilder.prepareSyncML(commands, last);
                msg    = syncMLBuilder.prepareMsg(syncml);        

                LOG.debug(TEXT("Status to the server"));
                LOG.debug(msg);

                responseMsg = transportAgent->sendMessage(msg);
                if (responseMsg == NULL) {
                    ret=lastErrorCode;
                    goto finally;
                }      
                // increment the msgRef after every send message
                syncMLBuilder.increaseMsgRef();
                syncMLBuilder.resetCommandID();

                deleteSyncML(&syncml);
                safeDelete(&msg);

                syncml = syncMLProcessor.processMsg(responseMsg);
                safeDelete(&responseMsg);
                deleteArrayList(&commands);
                if (syncml == NULL) {
                    ret = lastErrorCode;
                    goto finally;
                }  
                ret = syncMLProcessor.processSyncHdrStatus(syncml);

                if (isErrorStatus(ret)) {
                    ret = lastErrorCode;
                    wsprintf(lastErrorMsg, TEXT("Error: error code %i"), ret);
                    LOG.debug(lastErrorMsg);
                    goto finally;
                }
                ret = 0;

            }        
            deleteArrayList(&statusList);

        } while (last == FALSE);

    }
finally:

    safeDelete(&responseMsg);
    safeDelete(&msg);    
    deleteSyncML(&syncml);    

    return ret;
}


int SyncManager::endSync() {

    wchar_t* mapMsg         = NULL;
    wchar_t* responseMsg    = NULL;
    SyncML*  syncml         = NULL;
    BOOL     last           = TRUE;
    int ret                 = 0;   
    Map* map                = NULL;
    Status* status          = NULL;
    ArrayList* list         = new ArrayList();
    unsigned int iterator   = 0;
    unsigned int toSync     = 0;
    int i = 0, tot = -1;

    // The real number of source to sync
    for (count = 0; count < sourcesNumber; count ++) {
        if (!check[count])
            continue;
        toSync++;
    }

    for (count = 0; count < sourcesNumber; count ++) {
        if (!check[count])
            continue;

		if ((sources[count]->getSyncMode()) != SYNC_ONE_WAY_FROM_CLIENT &&
            (sources[count]->getSyncMode()) != SYNC_REFRESH_FROM_CLIENT )
		{
			iterator++;
			last = FALSE;
			i = 0;
			do {
				tot = -1;
				if (commands->isEmpty()) {
					status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
					commands->add(*status);
					deleteStatus(&status); 
            
					/* The server should not send any alert...
					list = syncMLProcessor.getCommands(syncml->getSyncBody(), ALERT);   
					status = syncMLBuilder.prepareAlertStatus(*sources[0], list, 200);
					if (status) {
						commands->add(*status);
						deleteStatus(&status);    
					}
					deleteArrayList(&list),
					*/
				}
        
				if (mappings[count]->size() > 0)
					map = syncMLBuilder.prepareMapCommand(*sources[count]);
				else if (iterator != toSync)
					break;
				else
					last = TRUE;
        
				for (i; i < mappings[count]->size(); i++) {                                                      
					tot++;
					MapItem* mapItem = syncMLBuilder.prepareMapItem((SyncMap*)mappings[count]->get(i));
					syncMLBuilder.addMapItem(map, mapItem);
            
					deleteMapItem(&mapItem);                        
            
					if (tot == ((int)maxModPerMsg - 1)) {
						i++;
						last = FALSE;
						break; 
                    
					}
					last = TRUE;
				}
            
				if (i == mappings[count]->size()) {
					last = TRUE;
				}

				if (mappings[count]->size() > 0)
					commands->add(*map);

				syncml    = syncMLBuilder.prepareSyncML(commands, iterator != toSync ? FALSE : last);
				mapMsg    = syncMLBuilder.prepareMsg(syncml);                   
    
				LOG.debug(TEXT("Mapping"));
				LOG.debug(mapMsg);
    
				responseMsg = transportAgent->sendMessage(mapMsg);
				if (responseMsg == NULL) {
					ret=lastErrorCode;
					goto finally;
				}      
				// increment the msgRef after every send message
				syncMLBuilder.increaseMsgRef();
				syncMLBuilder.resetCommandID();

				deleteSyncML(&syncml);
				safeDelete(&mapMsg);

				syncml = syncMLProcessor.processMsg(responseMsg);
				safeDelete(&responseMsg);
				deleteArrayList(&commands);
            
				if (syncml == NULL) {
					ret = lastErrorCode;
					goto finally;
				}  
				ret = syncMLProcessor.processSyncHdrStatus(syncml);

				if (isErrorStatus(ret)) {
					ret = lastErrorCode;
					wsprintf(lastErrorMsg, TEXT("Error: error code %i"), ret);
					LOG.debug(lastErrorMsg);
					goto finally;
				}
				ret = 0;

				//
				// Process the status of mapping
				//                        
					ret = syncMLProcessor.processMapResponse(*sources[count], syncml->getSyncBody());
				if (ret == -1) {
					ret = lastErrorCode;
					goto finally;
				}

			} while(!last);           
            
			if(allItemsList[count]) {
				int size = allItemsList[count]->size();
				for(int i = 0; i < size; i++) {
					SyncItem* syncItem = (SyncItem*)((SyncItem*)allItemsList[count]->get(i));
					if(syncItem) {
						sources[count]->deleteItem(*syncItem);
						delete syncItem;
					}
				}
			}

			sources[count]->endSync();
		}         
    }         
            
 finally:

    for (count = 0; count < sourcesNumber; count ++) {
        if (!check[count])
            continue;
        commitChanges(*sources[count]);
	}
	
    config.getAccessConfig().setEndSync((unsigned long)time(NULL));
    safeDelete(&responseMsg);
    safeDelete(&mapMsg);

    //
    // This commitSync is no used because the saving of the configuration is done into
    // the Sync4jClient.
    // The operation of save the config is ONLY for the default config (the DMConfig)
    //
    //config.save();

    return ret;
}

BOOL SyncManager::readSyncSourceDefinition(SyncSource& source) {
    SyncSourceConfig ssc;
    wchar_t anchor[DIM_ANCHOR];

    if (config.getSyncSourceConfig(source.getName(), ssc) == FALSE) {
        return FALSE;
    }

    // syncMode setted only if value has no good value...
    if (source.getPreferredSyncMode() == NULL ||
        source.getPreferredSyncMode() < 0     ||
        source.getPreferredSyncMode() > 210      )  {

        //source.setPreferredSyncMode(syncModeCode(ssc.getSync()));
    }
    source.setPreferredSyncMode(syncModeCode(ssc.getSync()));
    source.setType(ssc.getType());
    source.setLastSync(ssc.getLast());
    timestampToAnchor(ssc.getLast(), anchor);
    source.setLastAnchor(anchor);
    timestampToAnchor(source.getNextSync(), anchor);
    source.setNextAnchor(anchor);
    source.setRemoteURI(ssc.getURI());
    source.setEncoding(ssc.getEncoding());

    return TRUE;
}


BOOL SyncManager::commitChanges(SyncSource& source) {
    unsigned int n = config.getSyncSourceConfigsCount();
    SyncSourceConfig* configs = config.getSyncSourceConfigs();

    const wchar_t* name = source.getName();
    unsigned long next = source.getNextSync();

    wchar_t anchor[DIM_ANCHOR];
    timestampToAnchor(next, anchor);

    wcsprintf(logmsg, DBG_COMMITTING_SOURCE, name, anchor);
    LOG.debug(logmsg);

    for (unsigned int i = 0; i<n; ++i) {
        if (wcscmp(name, configs[i].getName()) == NULL) {
            configs[i].setLast(next);
            return TRUE;
        }
    }

    return FALSE;
}


int SyncManager::assignSources(SyncSource** sources) {
    
    int count = 0, i = 0;
    
    if (sources == NULL)
        goto finally;;    

    while(1) {
        if (sources[count])
            count++;
        else
            break;
    }
    this->sources = new SyncSource*[count + 1];
    for (i = 0; i < count + 1; i++) {
        this->sources[i] = sources[i];
    }

finally:

    return count;

}

char* SyncManager::processItemContent(wchar_t* toConvert, wchar_t* format, long *size) {
    
    wchar_t* p = NULL;
    wchar_t* encodings = stringdup(format);
    wchar_t* encoding = NULL;
    TransformationInfo info;

    char* c = wc2utf8(toConvert);
    info.size = strlen(c);
    info.password = credentialInfo; //(wchar_t*)config.getAccessConfig().getPassword();

    while ((p = wcsrchr(encodings, L';'))) {
        encoding = p+1;
        decodeSyncItemContent(&c, info, encoding);
        if (lastErrorCode != ERR_NONE) {
            break;
        }
        *p = 0;
    }

    if (wcslen(encodings) > 0) {
        
        decodeSyncItemContent(&c, info, encodings);        
    }
    c[info.size] = 0;
    *size = info.size;
    //wchar_t* ret = utf82wc(c);
    delete [] encodings;
    //delete [] c;    
    return c;
    
}

void SyncManager::decodeSyncItemContent(char** c, TransformationInfo& info, wchar_t* encoding) {
    
    char* decodedData = NULL;
   
    resetError();
   
    DataTransformer* dt = DataTransformerFactory::getDecoder(encoding);

    if (dt == NULL) {
        //
        // lastErrorCode contains already the error
        //
        goto exit;
    }
    
    decodedData = dt->transform(*c, info);

    if (lastErrorCode != ERR_NONE) {
        goto exit;
    }
    
    //
    // If the transformer has allocated new memory, we set it into the 
    // sync item, otherwise we just need to adjust the data size.
    //
    if (info.newReturnedData) {   
        
        strncpy(*c, decodedData, info.size);
        if (decodedData) {
            delete [] decodedData;
        }
        
    } else {               
       
    }       

exit:

    if (dt) {
        delete dt;
    }
    
}


