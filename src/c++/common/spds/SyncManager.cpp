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

#include "event/FireEvent.h"


/**
 * Is the given status code an error status code? Error codes are the ones
 * outside the range 200-299.
 *
 * @param status the status code to check
 */
inline static bool isErrorStatus(int status) {
    return (status) && ((status < 200) || (status > 299));
}

/**
 * Return true if the status code is authentication failed
 *
 * @param status the status code to check
 */
inline static bool isAuthFailed(int status) {
    return (status) && ((status == 401) || (status == 407));
}

/**
 * Return true if there's no more work to do
 *
 * @param check - the array of flags to set
 * @param size - the size of the array
 */
static bool isToExit(int* check, int size) {
    for (int i = 0; i < size; i++) {
        if (check[i] == 1) 
            return false; 
    }
    return true;
}

SyncManager::SyncManager(SyncManagerConfig& c) : config(c) {
    initialize();
}

void SyncManager::initialize() {
    // set all values which are checked by the destructor;
    // previously some pointers were only set later, leading to
    // uninitialized memory reads and potential crashes when
    // constructing a SyncManager, but not using it
    transportAgent = NULL;
    mappings       = NULL;
    sources        = NULL;
    currentState   = STATE_START;
    mappings       = NULL;
    check          = NULL;
    sourcesNumber  = 0;
    sources        = NULL;
    count          = 0;
    commands       = NULL;
    devInf         = NULL;

    AccessConfig& c = config.getAccessConfig();   
    DeviceConfig& dc = config.getDeviceConfig();
    
    bstrncpy(syncURL, c.getSyncURL(), 511);
    bstrncpy(deviceId, dc.getDevID(), 31);
    
    credentialHandler.setUsername           (c.getUsername());
    credentialHandler.setPassword           (c.getPassword());
    credentialHandler.setClientNonce        (c.getClientNonce());
    credentialHandler.setClientAuthType     (c.getClientAuthType());

    credentialHandler.setServerID           (c.getServerID());
    credentialHandler.setServerPWD          (c.getServerPWD());
    credentialHandler.setServerNonce        (c.getServerNonce());
    credentialHandler.setServerAuthType     (c.getServerAuthType());
    credentialHandler.setServerAuthRequired (c.getServerAuthRequired());
            
    commands = new ArrayList();

    maxMsgSize   = c.getMaxMsgSize();
    maxModPerMsg = 150;  // dafault value
    readBufferSize = 5000; // default value    

    if (c.getMaxModPerMsg() > 0)
        maxModPerMsg = c.getMaxModPerMsg();
    
    if (c.getReadBufferSize() > 0)
        readBufferSize = c.getReadBufferSize();
    
    syncMLBuilder.set(syncURL, deviceId, maxMsgSize);
    memset(credentialInfo, 0, 256*sizeof(BCHAR));
}

SyncManager::~SyncManager() {
    if (transportAgent) {
        delete transportAgent;
    }
    if (commands) {
        commands->clear(); delete commands; commands = NULL;
    }
    if (check) {
        delete [] check; check = NULL;
    }
    if (mappings) {
        for (int i=0; i<sourcesNumber; i++) {
            deleteArrayList(&mappings[i]);
            delete mappings[i];
        }
        delete [] mappings; mappings = NULL;
    }
    if (sources) {
        // This deletes only SyncSource array
        // We DON'T want to release SyncSource objects here!
        delete [] sources;
    }
    if (devInf) {
        delete devInf;
    }
}

/*
 * Modification to perform the sync of an array of sync sources.
 */

int SyncManager::prepareSync(SyncSource** s) {
    
    BCHAR* initMsg            = NULL;
    BCHAR* respURI            = NULL;
    BCHAR* responseMsg        = NULL;
    SyncML*  syncml             = NULL;
    int ret                     = 0;
    int serverRet               = 0;
    int count                   = 0;    
    const BCHAR* requestedAuthType  = NULL;
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
    StringBuffer* devInfStr     = NULL;
    BOOL putDevInf              = FALSE;
    BCHAR devInfHash[16 * 4 +1]; // worst case factor base64 is four
    const BCHAR *syncURL;
    
    // Fire Sync Begin Event
    fireSyncEvent(NULL, SYNC_BEGIN);

    syncURL = config.getAccessConfig().getSyncURL(); //sizeof(syncURL));

    URL url(syncURL);
    Proxy proxy;

    // Set proxy username/password if proxy is used.
    if (config.getAccessConfig().getUseProxy()) {
        //BCHAR* proxyHost = config.getAccessConfig().getProxyHost();
        //int    proxyPort = config.getAccessConfig().getProxyPort();
        BCHAR* proxyUser = config.getAccessConfig().getProxyUsername();
        BCHAR* proxyPwd  = config.getAccessConfig().getProxyPassword();
        proxy.setProxy(NULL, 0, proxyUser, proxyPwd);
    }
    
    const BCHAR* sourceName = NULL;
    
    sourcesNumber = assignSources(s);
    check = new int[sourcesNumber + 1];
    mappings = new ArrayList*[sourcesNumber + 1];
    for (count = 0; count < sourcesNumber; count++) {
        check[count] = 1;
        mappings[count] = new ArrayList();
    }
    check[sourcesNumber] = 0;

    // --- INFO
    bsprintf(logmsg, MSG_SYNC_URL, syncURL);
    LOG.info(logmsg);
    for (count = 0; count < sourcesNumber; count++) {
        bsprintf(logmsg, MSG_PREPARING_SYNC, _wcc(sources[count]->getName()));
        LOG.info(logmsg);
    }
    // ---

    syncMLBuilder.resetMessageID();
    syncMLBuilder.resetCommandID();
    syncMLBuilder.resetMessageID();
    unsigned long timestamp = (unsigned long)time(NULL);
    config.getAccessConfig().setBeginSync(timestamp);

    for (count = 0; count < sourcesNumber; count ++) {
        if (readSyncSourceDefinition(*sources[count]) == false) {
            ret = lastErrorCode = ERR_SOURCE_DEFINITION_NOT_FOUND;
            bsprintf(lastErrorMsg, ERRMSG_SOURCE_DEFINITION_NOT_FOUND,
                                   _wcc(sources[count]->getName()));
            LOG.debug(lastErrorMsg);
            check[count] = 0;
        }
    }

    // Create the device informations.
    devInf = createDeviceInfo();

    // check device information for changes
    if (devInf) {
        char md5[16];
        devInfStr = Formatter::getDevInf(devInf);
        LOG.debug(T("devinfo: %s"), devInfStr->c_str());
        // Add syncUrl to devInfHash, so hash changes if syncUrl has changed
        devInfStr->append("<SyncURL>");
        devInfStr->append(config.getAccessConfig().getSyncURL());
        devInfStr->append("</SyncURL>");
        calculateMD5(devInfStr->c_str(), devInfStr->length(), md5);
        devInfHash[b64_encode(devInfHash, md5, sizeof(md5))] = 0;
        LOG.debug(T("devinfo hash: %s"), devInfHash);

        // compare against previous device info hash:
        // if different, then the local config has changed and
        // infos should be sent again
        if (bstrcmp(devInfHash, config.getDeviceConfig().getDevInfHash())) {
            putDevInf = TRUE;
        }
        LOG.debug(T("devinfo %s"), putDevInf ? T("changed, retransmit") : T("unchanged, no need to send"));
    } else {
        LOG.debug(T("no devinfo available"));
    }

    // have device infos changed since the last time that they were
    // sent or is this the initial sync?
    

    // disable all SyncSources without a preferred sync mode
    for (count = 0; count < sourcesNumber; count ++) {
        if (!check[count])
            continue;

        if (sources[count]->getPreferredSyncMode() == SYNC_NONE) {
            check[count] = 0;
        }
    }
        
    if (isToExit(check, sourcesNumber)) {
        if (!ret) {
            // error: no source to sync
            ret = lastErrorCode = ERR_NO_SOURCE_TO_SYNC;
            bsprintf(lastErrorMsg, ERRMSG_NO_SOURCE_TO_SYNC);
        }

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
            BCHAR anc[DIM_ANCHOR];
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
            bstrcpy(credentialInfo, cred->getAuthentication()->getData(NULL));
        }

        // actively send out device infos?
        if (putDevInf) {
            AbstractCommand* put = syncMLBuilder.prepareDevInf(NULL, *devInf);
            if (put) {
                commands->add(*put);
                delete put;
            }
            putDevInf = FALSE;
        }
        
        // "cred" only contains an encoded strings as username, also
        // need the original username for LocName
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
        LOG.debug("%s", initMsg);
        
        currentState = STATE_PKG1_SENDING;

        if (transportAgent == NULL) {
            transportAgent = TransportAgentFactory::getTransportAgent(url, proxy);
            transportAgent->setReadBufferSize(readBufferSize);
            // Here we also ensure that the user agent string is valid
            const BCHAR* ua = getUserAgent(config);
            LOG.debug("User Agent = %s", ua);
            transportAgent->setUserAgent(ua);
            delete [] ua; ua = NULL;
            
            if (maxMsgSize > 0) {
                transportAgent->setMaxMsgSize(maxMsgSize);
            }
            
        } else {
            transportAgent->setURL(url);
        }
        if (lastErrorCode != 0) { // connection: lastErrorCode = 2005: Impossible to establish internet connection
            ret = lastErrorCode;
            goto finally;
        }  

        deleteSyncML(&syncml);         
        deleteChal(&serverChal);
        deleteArrayList(&commands);
        deleteCred(&cred);

        //Fire Initialization Event
        fireSyncEvent(NULL, SEND_INITIALIZATION);

        responseMsg = transportAgent->sendMessage(initMsg);
        // Non-existant or empty reply?
        // Synthesis server replies with empty message to
        // a message that it cannot parse.
        if (responseMsg == NULL || !responseMsg[0]) {
            if (responseMsg) {
                delete [] responseMsg;
                responseMsg = NULL;
            }

            // This is an error only is it is not an AddressChange
            if ( addressChange && lastErrorCode == ERR_READING_CONTENT ) {
                ret = 0;
            }
            else {
                // use last error code if one has been set (might not be the case)
                ret = lastErrorCode;
                if (!ret) {
                    ret = ERR_READING_CONTENT;
                }
            }
            goto finally;
        }

        // increment the msgRef after every send message
        syncMLBuilder.increaseMsgRef();
        syncMLBuilder.resetCommandID();

        syncml = syncMLProcessor.processMsg(responseMsg);
        safeDelete(&responseMsg);
        safeDelete(&initMsg);
        
        if (syncml == NULL) {
            ret = lastErrorCode;
            LOG.error(T("Error processing alert response."));
            goto finally;
        }
        
        // ret = syncMLProcessor.processInitResponse(*sources[0], syncml, alerts);

        ret = syncMLProcessor.processSyncHdrStatus(syncml);

        if (ret == -1) {
            ret = lastErrorCode;
            LOG.error(T("Error processing SyncHdr Status"));
            goto finally;

        } else if (isErrorStatus(ret) && ! isAuthFailed(ret)) {
            lastErrorCode = ret;
            bsprintf(logmsg, T("Error from server %d"), ret);
            LOG.error(logmsg);
            goto finally;
        }

        for (count = 0; count < sourcesNumber; count ++) {
            if (!check[count])
                continue;

            int sourceRet = syncMLProcessor.processAlertStatus(*sources[count], syncml, alerts);
            if (isAuthFailed(ret) && sourceRet == -1) {
                // Synthesis server does not include SourceRefs if
                // authentication failed. Remember the authentication
                // failure in that case, otherwise we'll never get to the getChal() below.
            } else {
                ret = sourceRet;
            }

            if (ret == -1 || ret == 404 || ret == 415) {
                lastErrorCode = ret;
                bsprintf(logmsg, T("AlertStatus from server %d"), ret);
                LOG.error(logmsg);
                check[count] = 0;   
            }
        } 
        if (isToExit(check, sourcesNumber)) {
            // error. no source to sync
            ret = lastErrorCode;
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
                    if (bstrcmp(credentialHandler.getServerAuthType(NULL), AUTH_TYPE_MD5) == 0 ||
                        serverAuthRetries == 1)
                    {
                        serverChal   = credentialHandler.getServerChal(isServerAuthenticated);
                        authStatusCode = 401;
                        
                    } else {
                        ret = -1;   //XXX
                        LOG.error(T("Server not authenticated"));
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
        // Process Put/Get commands
        //
        list = syncml->getSyncBody()->getCommands();
        int cmdindex;
        for (cmdindex = 0; cmdindex < list->size(); cmdindex++) {
            AbstractCommand* cmd = (AbstractCommand*)list->get(cmdindex);
            BCHAR* name = cmd->getName();
            if (name) {
                BOOL isPut = !bstrcmp(name, PUT);
                BOOL isGet = !bstrcmp(name, GET);

                if (isGet || isPut) {
                    int statusCode = 200; // if set, then send it (on by default)

                    if (isGet) {
                        Get *get = (Get *)cmd;
                        ArrayList *items = get->getItems();
                        BOOL sendDevInf = FALSE;

                        Results results;
                        for (int i = 0; i < items->size(); i++) {
                            Item *item = (Item *)items->get(i);
                        
                            // we are not very picky: as long as the Item is
                            // called "./devinf11" as required by the standard
                            // we return our device infos
                            Target *target = item->getTarget();
                            if (target && target->getLocURI() &&
                                !bstrcmp(target->getLocURI(),
                                         DEVINF_URI)) {
                                sendDevInf = TRUE;
                            } else {
                                LOG.debug(T("ignoring request to Get item #%d"), i);
                            }
                        }

                        // cannot send if we have nothing, then simply acknowledge the request,
                        // but ignore it
                        if (sendDevInf && devInf) {
                            AbstractCommand *result = syncMLBuilder.prepareDevInf(cmd, *devInf);
                            if (result) {
                                commands->add(*result);
                                delete result;
                            }
                        }
                    } else {
                        // simply acknowledge Put
                    }

                    if (statusCode) {
                        status = syncMLBuilder.prepareCmdStatus(*cmd, statusCode);
                        if (status) {
		                    // Fire Sync Status Event: status from client
                            fireSyncStatusEvent(status->getName(), status->getStatusCode(), sources[count]->getConfig().getURI(), NULL , CLIENT_STATUS);

                            commands->add(*status);
                            deleteStatus(&status);    
                        }
                    }
                }
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
            if (bstrcmp(credentialHandler.getClientAuthType(NULL),requestedAuthType) != 0 ) {           
                if (clientChal && bstrcmp(requestedAuthType, AUTH_TYPE_MD5) == 0) {
                    credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                }                 
            } else { 
                if (bstrcmp(requestedAuthType, AUTH_TYPE_MD5) == 0 && clientAuthRetries == 1)  {
                    credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                 
                } else {
                    ret = 401;
                    LOG.error(T("Client not authenticated"));
                    goto finally;
                }                                    
            }
            credentialHandler.setClientAuthType(requestedAuthType); 
            clientAuthRetries++;            

       } else {            
            if (clientChal && bstrcmp(clientChal->getType(NULL), AUTH_TYPE_MD5) == 0) {                    
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
    config.getDeviceConfig().setDevInfHash(devInfHash);
    
    if (isToExit(check, sourcesNumber)) {
        // error. no source to sync
        ret = -1;
        goto finally;
    }

    currentState = STATE_PKG1_SENT;

 
// ---------------------------------------------------------------------------------------
finally:

    if(ret) {
        //Fire Sync Error Event
        fireSyncEvent(lastErrorMsg, SYNC_ERROR);
    }

    if (respURI) {
        delete [] respURI;
    }
    if (responseMsg) {
        safeDelete(&responseMsg);
    }
    if (initMsg) {
        safeDelete(&initMsg);
    }
    if (devInfStr) {
        delete devInfStr;
    }

    deleteSyncML(&syncml);
    deleteCred(&cred);
    deleteAlert(&alert);
    deleteStatus(&status);
    deleteChal(&serverChal);    
    return ret;
}

//
// utility function to process any <Sync> command that the server might
// have included in its <SyncBody>
//
// @param syncml       the server response
// @param statusList   list to which statuses for changes are to be added
// @return TRUE if a fatal error occurred
//
BOOL SyncManager::checkForServerChanges(SyncML* syncml, ArrayList &statusList)
{
    BOOL result = FALSE;
    
    // Danger, danger: count is a member variable!
    // It has to be because that's the context for some of
    // the other methods. Modifying it has to be careful to
    // restore the initial value before returning because
    // our caller might use it, too.
    int oldCount = this->count;
    
    for (count = 0; count < sourcesNumber; count ++) {
        if (!check [count])
            continue;

        Sync* sync = syncMLProcessor.processSyncResponse(*sources[count], syncml);

        if (sync) {
            // Fire SyncSource event: BEGIN sync of a syncsource (server modifications)
            // (fire only if <sync> tag exist)
            fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getSyncMode(), SYNC_SOURCE_BEGIN);

            ArrayList* items = sync->getCommands();
            Status* status = syncMLBuilder.prepareSyncStatus(*sources[count], sync);
			statusList.add(*status);
            deleteStatus(&status);

            ArrayList* previousStatus = new ArrayList();
            for (int i = 0; i < items->size(); i++) {
                CommandInfo cmdInfo;
                ModificationCommand* modificationCommand = (ModificationCommand*)(items->get(i));
                Meta* meta = modificationCommand->getMeta();
                ArrayList* list = modificationCommand->getItems();
                        
                cmdInfo.commandName = modificationCommand->getName();
                cmdInfo.cmdRef = modificationCommand->getCmdID()->getCmdID();

                if (meta) {
                    cmdInfo.dataType = meta->getType();
                    cmdInfo.format = meta->getFormat(); 
                }
                else {
                    cmdInfo.dataType = 0;
                    cmdInfo.format = 0;
                }

                for (int j = 0; j < list->size(); j++) {
                    Item *item = (Item*)list->get(j);
                    if (item == NULL) {
                        LOG.error("SyncManager::sync() - unexpected NULL item.");
                        result = TRUE;
                        goto finally;
                    }

                    //
                    // set the syncItem element
                    //
                    status = processSyncItem(item, cmdInfo);

                    syncMLBuilder.addItemStatus(previousStatus, status);
                    deleteStatus(&status);            
                }

                if (previousStatus) {
                    statusList.add(previousStatus);
                    deleteArrayList(&previousStatus);                    
                }
            }
        // Fire SyncSourceEvent: END sync of a syncsource (server modifications)
        fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getSyncMode(), SYNC_SOURCE_END);

        }                               
    } // End for (count = 0; count < sourcesNumber; count ++)

    
  finally:
    this->count = oldCount;
    return result;
}


int SyncManager::sync() {

    BCHAR* msg         = NULL;
    BCHAR* responseMsg = NULL;
    Status* status       = NULL;
    SyncML* syncml       = NULL;
    SyncItem* syncItem   = NULL;
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
    BOOL isAtLeastOneSourceCorrect = FALSE;

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

        // Fire SyncSource event: BEGIN sync of a syncsource (client modifications)
        fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getSyncMode(), SYNC_SOURCE_BEGIN);

        if ( sources[count]->beginSync() ) {
            // Error from SyncSource
            lastErrorCode = ERR_UNSPECIFIED;
            ret = lastErrorCode;
            check[count]=0;            
            continue;
        }
        else
        {
            isAtLeastOneSourceCorrect = TRUE;
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
                if (bstrcmp(sources[count]->getConfig().getEncoding(), B64_ENCODING) == 0) {
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
                case SYNC_SLOW: 
                    {   
                        syncItem = NULL;
                        if (tot == 0) {                    
                            syncItem = sources[count]->getFirstItem();                        
                        }
                        tot = 0;
                        do {
                            if (syncItem == NULL) {
                                syncItem = sources[count]->getNextItem();                   
                            }
                            if (modificationCommand == NULL) {
                                modificationCommand = 
                                    syncMLBuilder.prepareModificationCommand(
                                                            REPLACE_COMMAND_NAME, 
                                                            syncItem,
                                                            sources[count]->getConfig().getType()
                                                  );
                            } else {
                                syncMLBuilder.addItem(modificationCommand,
                                                      REPLACE_COMMAND_NAME,
                                                      syncItem, 
                                                      sources[count]->getConfig().getType());
                            }

                            if (syncItem) {
                                // the item is only the pointer not another instance. to save mem
                                delete syncItem; syncItem = NULL;
                            }
                            else {
                                last = TRUE;
                                break;
                            }
                            tot++;
                        } while( tot < maxModPerMsg);                                                    
                    }
                    break;

                case SYNC_REFRESH_FROM_SERVER:
                    last = TRUE; 

                    allItemsList[count] = new ArrayList();
                    syncItem = sources[count]->getFirstItemKey();
                    if(syncItem) {
                        allItemsList[count]->add((ArrayElement&)*syncItem);
                        delete syncItem; syncItem = NULL;
                    }
                    syncItem = sources[count]->getNextItemKey();
                    while(syncItem) {
                        allItemsList[count]->add((ArrayElement&)*syncItem);
                        delete syncItem; syncItem = NULL;
                        syncItem = sources[count]->getNextItemKey();	
                    } 
                    break;

                case SYNC_ONE_WAY_FROM_SERVER:
                    last = TRUE;
                    break;

                case SYNC_REFRESH_FROM_CLIENT:
                    {   
                        syncItem = NULL;
                        if (tot == 0) {                    
                            syncItem = sources[count]->getFirstItem();
                        }
                        tot = 0;
                        do {
                            if (syncItem == NULL) {
                                syncItem = sources[count]->getNextItem();                  
                            }
                            if (modificationCommand == NULL) {
                                modificationCommand = syncMLBuilder.prepareModificationCommand(REPLACE_COMMAND_NAME, 
                                                                                               syncItem, sources[count]->getConfig().getType());
                            } else {
                                syncMLBuilder.addItem(modificationCommand, REPLACE_COMMAND_NAME, syncItem, 
                                                      sources[count]->getConfig().getType());
                            }

                            if (syncItem) {
                                delete syncItem; syncItem = NULL;// the item is only the pointer not another instance. to save mem                        
                            }
                            else {
                                last = TRUE;
                                break;
                            }
                            tot++;
                        } while( tot < maxModPerMsg);                                                    
                    }
                    break;

                default:
                    {    
                        tot = 0;
                        //
                        // New Item
                        //
                        syncItem = NULL;
                        if (step == 0) {
                            syncItem = sources[count]->getFirstNewItem(); 
                            step++;
                            if (syncItem == NULL)
                                step++;  
                            else {
				                // Fire Sync Item Event - New Item Detected
                                fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_ADDED_BY_CLIENT);
                            }
                        }
                        if (step == 1) {                                                          
                            do {
                                if (syncItem == NULL) {
                                    syncItem = sources[count]->getNextNewItem();
                                }
                                if (modificationCommand == NULL) {
                                    modificationCommand = syncMLBuilder.prepareModificationCommand(ADD_COMMAND_NAME, 
                                                                                                   syncItem, sources[count]->getConfig().getType());
                                } else {
                                    syncMLBuilder.addItem(modificationCommand, ADD_COMMAND_NAME, 
                                                          syncItem, sources[count]->getConfig().getType());
                                }

                                if (syncItem) {
                                    // Fire Sync Item Event - New Item Detected
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_ADDED_BY_CLIENT);
                                    delete syncItem; syncItem = NULL;
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
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

                            syncItem = sources[count]->getFirstUpdatedItem();

                            step++;
                            if (syncItem == NULL)
                                step++;    
                            else {
                                // Fire Sync Item Event - Item Updated
                                fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                            }

                        }
                        if (step == 3) {
                            do {				
                                if (syncItem == NULL) {
                                    syncItem = sources[count]->getNextUpdatedItem();
                                }
                                if (modificationCommand == NULL) {
                                    modificationCommand = syncMLBuilder.prepareModificationCommand(REPLACE_COMMAND_NAME, 
                                                                                                   syncItem, sources[count]->getConfig().getType());
                                } else {
                                    syncMLBuilder.addItem(modificationCommand, REPLACE_COMMAND_NAME, 
                                                          syncItem, sources[count]->getConfig().getType());
                                }

                                if (syncItem) {  
                                    // Fire Sync Item Event - Item Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                    delete syncItem; syncItem = NULL;
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
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

                            syncItem = sources[count]->getFirstDeletedItem();

                            step++;
                            if (syncItem == NULL)
                                step++;    
                            else {
                                // Fire Sync Item Event - Item Deleted
                                fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_DELETED_BY_CLIENT);
                            }
                        }
                        if (step == 5) {
                            do {
                                if (syncItem == NULL) {
                                    syncItem = sources[count]->getNextDeletedItem();                       
                                }
                                if (modificationCommand == NULL) {
                                    modificationCommand = syncMLBuilder.prepareModificationCommand(DELETE_COMMAND_NAME, 
                                                                                                   syncItem, sources[count]->getConfig().getType());
                                } else {
                                    syncMLBuilder.addItem(modificationCommand, DELETE_COMMAND_NAME, 
                                            syncItem, sources[count]->getConfig().getType());
                                }

                                if (syncItem) {
                                    // Fire Sync Item Event - Item Deleted
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_DELETED_BY_CLIENT);
                                    delete syncItem; syncItem = NULL;
                                }
                                else {
                                    step++;
                                    break;
                                }
                                tot++;
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
            delete list;
            commands->add(*sync);
            delete sync;

            //
            // Check if all the sources were synced.
            // If not the prepareSync doesn't use the <final/> tag 
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
            LOG.debug("%s", msg);

            //Fire Modifications Event
            fireSyncEvent(NULL, SEND_MODIFICATION);

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
                lastErrorCode = ret;
                bsprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
                LOG.error(lastErrorMsg);
                goto finally;

            }
            ret = 0;
            // 
            // Process the status of the item sent by client. It invokes the 
            // source method 
            //
            int itemret = syncMLProcessor.processItemStatus(*sources[count], syncml->getSyncBody());
            if(itemret){
                char *name = toMultibyte(sources[count]->getName());
                LOG.error("Error #%d in source %s", itemret, name);
                delete [] name;
                // skip the source, and set an error
                check[count] = 0;
                lastErrorCode = itemret;
                break;
            }

            // The server might have included a <Sync> command without waiting
            // for a 222 alert. If it hasn't, then nothing is done here.
            ArrayList statusList;
            if (checkForServerChanges(syncml, statusList)) {
                goto finally;
            }
            if (statusList.size()) {
                Status* status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
                commands->add(*status);
                deleteStatus(&status);
                commands->add(&statusList);
            }

            // deleteSyncML(&syncml);

        } while (last == FALSE);

        // Fire SyncSourceEvent: END sync of a syncsource (client modifications)
        fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getSyncMode(), SYNC_SOURCE_END);

    } // end for (count = 0; count < sourcesNumber; count ++)
    
    if (isToExit(check, sourcesNumber)) {
        // error. no source to sync
        ret = lastErrorCode;
        goto finally;
    }
    
    deleteSyncML(&syncml);

    //
    // If this was the last chunk, we move the state to STATE_PKG3_SENT
    // At this time "last" is always true. The client is going to send
    // the 222 package for to get the server modification if at least a source is correct
    //
    last = TRUE;
    currentState = STATE_PKG3_SENT;    

    //
    // send 222 alert code to retrieve the item from server
    //    
    if ((FALSE == isFinalfromServer) && (TRUE == isAtLeastOneSourceCorrect))
    {
        status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
	    commands->add(*status);
        deleteStatus(&status); 
        for (count = 0; count < sourcesNumber; count ++) {
            if(!check[count])
                continue;
            if ((sources[count]->getSyncMode() != SYNC_ONE_WAY_FROM_CLIENT) &&
                (sources[count]->getSyncMode() != SYNC_REFRESH_FROM_CLIENT))
            {
                alert = syncMLBuilder.prepareRequestAlert(*sources[count]);
                commands->add(*alert);
                deleteAlert(&alert);
            }
        }

        syncml = syncMLBuilder.prepareSyncML(commands, last);
        msg    = syncMLBuilder.prepareMsg(syncml);        

        LOG.debug(T("Alert to request server changes"));
        LOG.debug("%s", msg);

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
            lastErrorCode = ret;
            bsprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
            LOG.error(lastErrorMsg);
            goto finally;
        }
        ret = 0;

        //
        // Process the items returned from server
        //

        do {              
            last = syncml->getSyncBody()->getFinalMsg();
            ArrayList statusList;

            status = syncMLBuilder.prepareSyncHdrStatus(NULL, 200);
            commands->add(*status);
            deleteStatus(&status); 

            if (checkForServerChanges(syncml, statusList)) {
                goto finally;
            }

            commands->add(&statusList);

            if (!last) {
                deleteSyncML(&syncml);
                syncml = syncMLBuilder.prepareSyncML(commands, last);
                msg    = syncMLBuilder.prepareMsg(syncml);        

                LOG.debug(T("Status to the server"));
                LOG.debug("%s", msg);

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
                    lastErrorCode = ret;
                    bsprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
                    LOG.error(lastErrorMsg);
                    goto finally;
                }
                ret = 0;
            }        
        } while (last == FALSE);
    }


finally:

    if (isAtLeastOneSourceCorrect == TRUE)
    {
        safeDelete(&responseMsg);
        safeDelete(&msg);    
        deleteSyncML(&syncml);    
    }
    else
    {
        ret = -1;
        LOG.debug("sources not available");
    }

    if (ret) {
        //Fire Sync Error Event
        fireSyncEvent(lastErrorMsg, SYNC_ERROR);
    }
    return ret;
}


int SyncManager::endSync() {

    BCHAR* mapMsg         = NULL;
    BCHAR* responseMsg    = NULL;
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
        if ((sources[count]->getSyncMode()) != SYNC_ONE_WAY_FROM_CLIENT &&
            (sources[count]->getSyncMode()) != SYNC_REFRESH_FROM_CLIENT )

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

                syncml = syncMLBuilder.prepareSyncML(commands, iterator != toSync ? FALSE : last);
                mapMsg = syncMLBuilder.prepareMsg(syncml);                   

                LOG.debug(T("Mapping"));
                LOG.debug("%s", mapMsg);

                //Fire Finalization Event
                fireSyncEvent(NULL, SEND_FINALIZATION);

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
                    lastErrorCode = ret;
                    bsprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
                    LOG.error(lastErrorMsg);
                    goto finally;
                }
                ret = 0;

                //
                // Process the status of mapping
                //                        
                ret = syncMLProcessor.processMapResponse(*sources[count], syncml->getSyncBody());
                deleteSyncML(&syncml);
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

            int sret = sources[count]->endSync();
            if (sret) {
                lastErrorCode = sret;
            }
        }        
    }         
        
 finally:

    for (count = 0; count < sourcesNumber; count ++) {
        if (!check[count])
            continue;
   
        commitChanges(*sources[count]);
    }
    /*
	if (mappings) {
        for (int i=0; i<sourcesNumber; i++) {
            deleteArrayList(&mappings[i]);
            if (mappings[i]) { delete mappings[i]; mappings[i] = NULL; } 
        }
        delete [] mappings; mappings = NULL;
    }
    */

    config.getAccessConfig().setEndSync((unsigned long)time(NULL));
    safeDelete(&responseMsg);
    safeDelete(&mapMsg);
    BCHAR g[768]; bsprintf(g, "ret: %i, lastErrorCode: %i, lastErrorMessage: %s", ret, lastErrorCode, lastErrorMsg); LOG.debug(g);

	// Fire Sync End Event
    fireSyncEvent(NULL, SYNC_END);

    if (ret){
        //Fire Sync Error Event
        fireSyncEvent(lastErrorMsg, SYNC_ERROR);

        return ret;
    }
    else if (lastErrorCode){
        return lastErrorCode;
    }
    else
        return 0;
}

BOOL SyncManager::readSyncSourceDefinition(SyncSource& source) {
    BCHAR anchor[DIM_ANCHOR];
    
    if (config.getSyncSourceConfig(_wcc(source.getName()), source.getConfig()) == FALSE) {
        return FALSE;
    }

    SyncSourceConfig& ssc(source.getConfig());

    // only copy properties which either have a different format
    // or are expected to change during the synchronization
    timestampToAnchor(ssc.getLast(), anchor);
    source.setLastAnchor(anchor);
    timestampToAnchor(source.getNextSync(), anchor);
    source.setNextAnchor(anchor);

    return TRUE;
}


BOOL SyncManager::commitChanges(SyncSource& source) { 
    unsigned int n = config.getSyncSourceConfigsCount();
    SyncSourceConfig* configs = config.getSyncSourceConfigs();

    const BCHAR* name = _wcc(source.getName());
    unsigned long next = source.getNextSync();

    BCHAR anchor[DIM_ANCHOR];
    timestampToAnchor(next, anchor);

    bsprintf(logmsg, DBG_COMMITTING_SOURCE, name, anchor);
    LOG.debug(logmsg);

    for (unsigned int i = 0; i<n; ++i) {
        if (bstrcmp(name, configs[i].getName()) == NULL) {
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

/**
 * Buid a new SyncItem, using the data in Item
 *
 * @param item - the item data
 * @param format - the format specified in the command, or NULL
 * @return - the new SyncItem. Must be deleted by the caller
 */
Status *SyncManager::processSyncItem(Item* item, const CommandInfo &cmdInfo)
{
    int code = 0;
    const BCHAR* itemName;

    Source* s = item->getSource();
    if (s) { 
        itemName = s->getLocURI();
    }
    else {
        Target* t = item->getTarget();                
        itemName = t->getLocURI();
    }

    // Fill item -------------------------------------------------
    wchar_t *iname = toWideChar(itemName);
    SyncItem syncItem(iname);
    delete [] iname;

    ComplexData *cdata = item->getData();
    if (cdata) {
        BCHAR* data = cdata->getData();
        BCHAR* format = 0;

        //
        // Retrieving how the content has been encoded
        // and then processing the content accordingly
        //
        if (cmdInfo.format) {
            format = cmdInfo.format;
        }
        else {
            Meta* m = item->getMeta();
            if (m) {
                format = m->getFormat();                            
            }
        }
        if (format) {
            long size = 0;
            char *convertedData = processItemContent(data, format, &size);
            syncItem.setData(convertedData, size);
            delete [] convertedData;
        }
        else {
            syncItem.setData(data, strlen(data));
        }
    }
    if (cmdInfo.dataType) {
        wchar_t *dtype = toWideChar(cmdInfo.dataType);
        syncItem.setDataType(dtype);
        delete [] dtype;
    }
    wchar_t *sparent = toWideChar(item->getSourceParent());
    syncItem.setSourceParent(sparent);
    delete [] sparent;
    wchar_t *tparent = toWideChar(item->getTargetParent());
    syncItem.setTargetParent(tparent);
    delete [] tparent;

    syncItem.setModificationTime(sources[count]->getNextSync());

    // Process item ------------------------------------------------------------
    Status *status = 0;
    if ( bstrcmp(cmdInfo.commandName, ADD) == 0) {  
        // Fire Sync Item Event - New Item Added by Server
        fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem.getKey(), ITEM_ADDED_BY_SERVER);

        syncItem.setState(SYNC_STATE_NEW);
        code = sources[count]->addItem(syncItem);      
        status = syncMLBuilder.prepareItemStatus(ADD, itemName, cmdInfo.cmdRef, code);

	    // Fire Sync Status Event: item status from client
        fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getURI(), syncItem.getKey(), CLIENT_STATUS);

        // If the add was successful, set the id mapping
        if (code >= 200 && code <= 299) {
            BCHAR *key = toMultibyte(syncItem.getKey());
            SyncMap syncMap(item->getSource()->getLocURI(), key);
            mappings[count]->add(syncMap);
            delete [] key;
        }                    
    }
    else if (bstrcmp(cmdInfo.commandName, REPLACE) == 0) {  
        // Fire Sync Item Event - Item Updated by Server
        fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem.getKey(), ITEM_UPDATED_BY_SERVER);

        syncItem.setState(SYNC_STATE_UPDATED);
        code = sources[count]->updateItem(syncItem);
        status = syncMLBuilder.prepareItemStatus(REPLACE, itemName, cmdInfo.cmdRef, code);                

	    // Fire Sync Status Event: item status from client
        fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getURI(), syncItem.getKey(), CLIENT_STATUS);
    }
    else if (bstrcmp(cmdInfo.commandName, DEL) == 0) {
        // Fire Sync Item Event - Item Deleted by Server
        fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem.getKey(), ITEM_DELETED_BY_SERVER);

        syncItem.setState(SYNC_STATE_DELETED);
        code = sources[count]->deleteItem(syncItem);        
        status = syncMLBuilder.prepareItemStatus(DEL, itemName, cmdInfo.cmdRef, code);           
	    
        // Fire Sync Status Event: item status from client
        fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getURI(), syncItem.getKey(), CLIENT_STATUS);
    }                
    return status;
}

char* SyncManager::processItemContent(const BCHAR* toConvert,
                                      const BCHAR* format,
                                      long *size) {
    
    BCHAR* p = NULL;
    BCHAR* encodings = stringdup(format);
    BCHAR* encoding = NULL;
    TransformationInfo info;

    char* c = stringdup(toConvert);
    info.size = strlen(c);
    info.password = credentialInfo; //(BCHAR*)config.getAccessConfig().getPassword();

    while ((p = strrchr(encodings, CHR(';')))) {
        encoding = p+1;
        decodeSyncItemContent(&c, info, encoding);
        if (lastErrorCode != ERR_NONE) {
            break;
        }
        *p = 0;
    }

    if (bstrlen(encodings) > 0) {
        decodeSyncItemContent(&c, info, encodings);
    }
    c[info.size] = 0;
    *size = info.size;
    
    if (encodings) { delete [] encodings; encodings = NULL; }
    return c;
    
}

void SyncManager::decodeSyncItemContent(char** c,
                                        TransformationInfo& info,
                                        const BCHAR* encoding) {
    
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

    if (lastErrorCode == ERR_UNSPECIFIED) {
        goto exit;
    }
    
    //
    // If the transformer has allocated new memory, we set it into the 
    // sync item, otherwise we just need to adjust the data size.
    //
    if (info.newReturnedData) {   
        /// FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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


/*
 * Creates the device info for this client and sources.
 */
DevInf *SyncManager::createDeviceInfo()
{
    const BCHAR *rxType, *rxVer,
        *txType, *txVer;

    // check that essential information is available
    // for each source
    for (SyncSource **source = sources;
         *source;
         source++) {
        
        rxType = (*sources)->getConfig().getType();
        txType = (*sources)->getConfig().getType();
        rxVer  = (*sources)->getConfig().getVersion();
        txVer  = (*sources)->getConfig().getVersion();

        if (!rxType || !rxVer || !txType || !txVer) {
            return NULL;
        }
    }
    
    DevInf *devinfo = new DevInf();
    //
    // Copy devInf params from current Config.
    //
    VerDTD v(config.getDeviceConfig().getVerDTD());
    devinfo->setVerDTD(&v);
    devinfo->setMan(config.getDeviceConfig().getMan());
    devinfo->setMod(config.getDeviceConfig().getMod());
    devinfo->setOEM(config.getDeviceConfig().getOem());
    devinfo->setFwV(config.getDeviceConfig().getFwv());
    devinfo->setSwV(config.getDeviceConfig().getSwv());
    devinfo->setHwV(config.getDeviceConfig().getHwv());
    devinfo->setDevID(config.getDeviceConfig().getDevID());
    devinfo->setDevTyp(config.getDeviceConfig().getDevType());
    devinfo->setUTC(config.getDeviceConfig().getUtc());
    devinfo->setSupportLargeObjs(config.getDeviceConfig().getLoSupport());
    devinfo->setSupportNumberOfChanges(config.getDeviceConfig().getNocSupport());

    static const struct {
        SyncMode mode;
        int type;
    } mapping[] = {
        { SYNC_TWO_WAY, 1 },             // Support of 'two-way sync'
        { SYNC_SLOW, 2 },                // Support of 'slow two-way sync'
        { SYNC_ONE_WAY_FROM_CLIENT, 3 }, // Support of 'one-way sync from client only'
        { SYNC_REFRESH_FROM_CLIENT, 4 }, // Support of 'refresh sync from client only'
        { SYNC_ONE_WAY_FROM_SERVER, 5 }, // Support of 'one-way sync from server only'
        { SYNC_REFRESH_FROM_SERVER, 6 }, // Support of 'refresh sync from server only'
        // 7, // Support of 'server alerted sync'
        { SYNC_NONE, -1 }
    };

    ArrayList dataStores;
    for (SyncSource **source = sources;
         *source;
         source++) {
        ArrayList syncModeList;
        const BCHAR *syncModes = (*source)->getConfig().getSyncModes();
        if (syncModes) {
            BCHAR buffer[80];
            const BCHAR *mode = syncModes;

            while (*mode) {
                // skip leading spaces and commas
                while (isspace(*mode) || *mode == ',') {
                    mode++;
                }
                // fast-forward to comma
                const BCHAR *eostr = mode;
                while (*eostr && *eostr != ',') {
                    eostr++;
                }
                // strip spaces directly before comma
                while (eostr > mode && isspace(eostr[-1])) {
                    eostr--;
                }
                // make temporary copy (mode is read-only)
                size_t len = eostr - mode;
                if (len > sizeof(buffer) - 1) {
                    len = sizeof(buffer) - 1;
                }
                memcpy(buffer, mode, sizeof(BCHAR) * len);
                buffer[len] = 0;
                SyncMode sm = syncModeCode(buffer);
                for (int i = 0; mapping[i].type >= 0; i++) {
                    if (mapping[i].mode == sm) {
                        SyncType syncType(mapping[i].type);
                        syncModeList.add(syncType);
                        break;
                    }
                }

                // next item
                mode = eostr;
            }
        }

        char* name = toMultibyte((*source)->getName());
        SourceRef sourceRef(name);
        delete [] name; name = NULL;

        rxType = (*sources)->getConfig().getType();
        txType = (*sources)->getConfig().getType();
        rxVer  = (*sources)->getConfig().getVersion();
        txVer  = (*sources)->getConfig().getVersion();
        
        ContentTypeInfo rxPref(rxType, rxVer);
        ArrayList rx;
        fillContentTypeInfoList(rx, (*source)->getConfig().getSupportedTypes());
        ContentTypeInfo txPref(txType, txVer);
        ArrayList tx;
        fillContentTypeInfoList(tx, (*source)->getConfig().getSupportedTypes());
        SyncCap syncCap(&syncModeList);
        DataStore dataStore(&sourceRef,
                            NULL,
                            -1,
                            &rxPref,
                            &rx,
                            &txPref,
                            &tx,
                            NULL,
                            &syncCap);
        dataStores.add(dataStore);
    }
    devinfo->setDataStore(&dataStores);

#if 0
    // dummy CTCap - has no effect because Formatter::getCTCaps() has
    // not be implemented yet
    ArrayList empty;
    ArrayList ctPropParams;
    CTPropParam param(T("X-FOO"),
                      NULL, 0, NULL, &empty);
    ctPropParams.add(param);
    CTTypeSupported cttType(T("text/x-foo"), &ctPropParams);
    ArrayList ctCap;
    ctCap.add(cttType);
    devinfo->setCTCap(&ctCap);
#endif

    return devinfo;
}


/* Copy from SyncSourceConfig::getSupportedTypes() format into array list
 * of ContentTypeInfos.
 * @param types: formatted string of 'type:version' for each supported type
 *               i.e. "text/x-s4j-sifc:1.0,text/x-vcard:2.1"
 * @param l    : arraylist of ContentTypeInfo, to fill
 */
static void fillContentTypeInfoList(ArrayList &l, const BCHAR* types) { 
 
    l.clear();
    if (!types) {
        return;
    }

    char typeName[80];
    char typeVersion[20];
    const BCHAR* curr = types;
    const BCHAR* eostr = NULL;
    size_t len = 0;

    while (*curr) {
        // -------- Type Name ----------
        // skip leading spaces and commas
        while (isspace(*curr) || *curr == ',') {
            curr++;
        }
        // fast-forward to colon (next separator)
        eostr = curr;
        while (*eostr && *eostr != ':') {
            eostr++;
        }
        // copy type name
        len = eostr - curr;
        strncpy(typeName, curr, len);
        typeName[len] = 0;
    
        // ------- Type Version ---------
        curr = eostr;

        // skip leading spaces and colon
        while (isspace(*curr) || *curr == ':') {
            curr++;
        }
        // fast-forward to comma (next separator)
        eostr = curr;
        while (*eostr && *eostr != ',') {
            eostr++;
        }
        // copy type version
        len = eostr - curr;
        strncpy(typeVersion, curr, len);
        typeVersion[len] = 0;

        //
        // Add contentTypeInfo
        //
        ContentTypeInfo cti(typeName, typeVersion);
        l.add(cti);

        curr = eostr;
    }
}

/*
 * Ensure that the user agent string is valid.
 * If property 'user agent' is empty, it is replaced by 'mod' and 'SwV'
 * properties from DeviceConfig. 
 * If also 'mod' property is empty, return a default user agent.
 *
 * @param config: reference to the current SyncManagerConfig
 * @return      : user agent property as a new BCHAR*
 *                (need to be freed by the caller)
 */
const BCHAR* SyncManager::getUserAgent(SyncManagerConfig& config) {

    BCHAR* ret;
    StringBuffer userAgent(config.getAccessConfig().getUserAgent());
    StringBuffer buffer;
    
    if (userAgent.length()) {
        ret = stringdup(userAgent.c_str());
    }
    // Use 'mod + SwV' parameters for user agent
    else {
        const BCHAR* mod = config.getDeviceConfig().getMod();
        const BCHAR* swV = config.getDeviceConfig().getSwv();
        if (mod && strcmp(mod, "")) {
            buffer.append(mod);
            if (swV && strcmp(swV, "")) {
                buffer.append(" ");
                buffer.append(swV);
            }
            ret = stringdup(buffer.c_str());
        } else {
            // Default user agent value
            ret = stringdup(BCHAR_USER_AGENT);
        }
    }

    return ret;
}

