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

#include <limits.h>

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
 * (if no source has a correct status)
 */
bool SyncManager::isToExit() {
    for (int i = 0; i < sourcesNumber; i++) {
        if (sources[i]->getReport() && sources[i]->getReport()->checkState() == true) 
            return false; 
    }
    return true;
}

/*
 * Utility to set a SyncSource state + errorCode + errorMsg.
 */
void SyncManager::setSourceStateAndError(unsigned int index, SourceState  state,
                                         unsigned int code,  const char*  msg) {

    SyncSourceReport* report = sources[index]->getReport();

    if (!report) {
        return;
    }
    report->setState(state);
    report->setLastErrorCode(code);
    report->setLastErrorMsg(msg);
}



SyncManager::SyncManager(SyncManagerConfig& c, SyncReport& report) : config(c), syncReport(report) {
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
    sourcesNumber  = 0;
    sources        = NULL;
    count          = 0;
    commands       = NULL;
    devInf         = NULL;
    incomingItem   = NULL;

    AccessConfig& c = config.getAccessConfig();   
    DeviceConfig& dc = config.getDeviceConfig();
    
    strncpy(syncURL, c.getSyncURL(), 511);
    strncpy(deviceId, dc.getDevID(), 31);
    
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
    maxObjSize   = dc.getMaxObjSize();
    loSupport    = dc.getLoSupport();
    maxModPerMsg = 150;  // dafault value
    readBufferSize = 5000; // default value    

    if (c.getMaxModPerMsg() > 0)
        maxModPerMsg = c.getMaxModPerMsg();
    
    if (c.getReadBufferSize() > 0)
        readBufferSize = c.getReadBufferSize();
    
    syncMLBuilder.set(syncURL, deviceId);
    memset(credentialInfo, 0, 256*sizeof(char));
}

SyncManager::~SyncManager() {
    if (transportAgent) {
        delete transportAgent;
    }
    if (commands) {
        commands->clear(); delete commands; commands = NULL;
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
    if (incomingItem) {
        delete incomingItem;
    }
}

/*
 * Modification to perform the sync of an array of sync sources.
 */

int SyncManager::prepareSync(SyncSource** s) {
    
    char* initMsg               = NULL;
    char* respURI               = NULL;
    char* responseMsg           = NULL;
    SyncML*  syncml             = NULL;
    int ret                     = 0;
    int serverRet               = 0;
    int count                   = 0;    
    const char* requestedAuthType  = NULL;
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
    char devInfHash[16 * 4 +1]; // worst case factor base64 is four
    const char *syncURL;
    
    // Fire Sync Begin Event
    fireSyncEvent(NULL, SYNC_BEGIN);

    syncURL = config.getAccessConfig().getSyncURL(); //sizeof(syncURL));

    URL url(syncURL);
    Proxy proxy;

    // Set proxy username/password if proxy is used.
    if (config.getAccessConfig().getUseProxy()) {
        //char* proxyHost = config.getAccessConfig().getProxyHost();
        //int    proxyPort = config.getAccessConfig().getProxyPort();
        char* proxyUser = config.getAccessConfig().getProxyUsername();
        char* proxyPwd  = config.getAccessConfig().getProxyPassword();
        proxy.setProxy(NULL, 0, proxyUser, proxyPwd);
    }
    
    const char* sourceName = NULL;
    
    sourcesNumber = assignSources(s);
    mappings = new ArrayList*[sourcesNumber + 1];
    for (count = 0; count < sourcesNumber; count++) {
        mappings[count] = new ArrayList();
    }

    // --- INFO
    sprintf(logmsg, MSG_SYNC_URL, syncURL);
    LOG.info(logmsg);
    for (count = 0; count < sourcesNumber; count++) {
        sprintf(logmsg, MSG_PREPARING_SYNC, _wcc(sources[count]->getName()));
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
            sprintf(lastErrorMsg, ERRMSG_SOURCE_DEFINITION_NOT_FOUND,
                                   _wcc(sources[count]->getName()));
            LOG.debug(lastErrorMsg);

            setSourceStateAndError(count, SOURCE_INACTIVE, ERR_SOURCE_DEFINITION_NOT_FOUND, lastErrorMsg);
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
        if (strcmp(devInfHash, config.getDeviceConfig().getDevInfHash())) {
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
        if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
            continue;

        if (sources[count]->getPreferredSyncMode() == SYNC_NONE) {
            sources[count]->getReport()->setState(SOURCE_INACTIVE);
        }
    }
        
    if (isToExit()) {
        if (!ret) {
            // error: no source to sync
            ret = lastErrorCode = ERR_NO_SOURCE_TO_SYNC;
            sprintf(lastErrorMsg, ERRMSG_NO_SOURCE_TO_SYNC);
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
            char anc[DIM_ANCHOR];
            timestamp = (unsigned long)time(NULL);
            for (count = 0; count < sourcesNumber; count ++) {
                if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
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
            strcpy(credentialInfo, cred->getAuthentication()->getData(NULL));
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
        syncml = syncMLBuilder.prepareInitObject(cred, alerts, commands, maxMsgSize, maxObjSize);
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
            const char* ua = getUserAgent(config);
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

            if ( addressChange && lastErrorCode == ERR_READING_CONTENT ) {
                // This is not an error if it's an AddressChange
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
            sprintf(logmsg, T("Error from server %d"), ret);
            LOG.error(logmsg);
            goto finally;
        }

        for (count = 0; count < sourcesNumber; count ++) {
            if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
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
                sprintf(logmsg, T("Alert Status from server = %d"), ret);
                LOG.error(logmsg);
                setSourceStateAndError(count, SOURCE_ERROR, ret, logmsg);
            }
        } 
        if (isToExit()) {
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
                    if (strcmp(credentialHandler.getServerAuthType(NULL), AUTH_TYPE_MD5) == 0 ||
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
            if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
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
            char* name = cmd->getName();
            if (name) {
                BOOL isPut = !strcmp(name, PUT);
                BOOL isGet = !strcmp(name, GET);

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
                                !strcmp(target->getLocURI(),
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
                            fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), NULL, NULL , CLIENT_STATUS);

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
            if (strcmp(credentialHandler.getClientAuthType(NULL),requestedAuthType) != 0 ) {           
                if (clientChal && strcmp(requestedAuthType, AUTH_TYPE_MD5) == 0) {
                    credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());
                }                 
            } else { 
                if (strcmp(requestedAuthType, AUTH_TYPE_MD5) == 0 && clientAuthRetries == 1)  {
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
            if (clientChal && strcmp(clientChal->getType(NULL), AUTH_TYPE_MD5) == 0) {                    
                credentialHandler.setClientNonce(clientChal->getNextNonce()->getValueAsBase64());                
            }
            isClientAuthenticated = TRUE;
            for (count = 0; count < sourcesNumber; count ++) {
                if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
                    continue;
                ret = syncMLProcessor.processServerAlert(*sources[count], syncml);
                if (isErrorStatus(ret)) {
                    sprintf(logmsg, T("AlertStatus from server %d"), ret);
                    LOG.error(logmsg);
                    setSourceStateAndError(count, SOURCE_ERROR, ret, logmsg); 
                }
                fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_SYNCMODE_REQUESTED);
            }  
       }
    
    } while(isClientAuthenticated == FALSE || isServerAuthenticated == FALSE);

    config.getAccessConfig().setClientNonce(credentialHandler.getClientNonce(NULL));
    config.getAccessConfig().setServerNonce(credentialHandler.getServerNonce(NULL));
    config.getDeviceConfig().setDevInfHash(devInfHash);
    
    if (isToExit()) {
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
        if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
            continue;

        Sync* sync = syncMLProcessor.processSyncResponse(*sources[count], syncml);

        if (sync) {
            // Fire SyncSource event: BEGIN sync of a syncsource (server modifications)
            // (fire only if <sync> tag exist)
            fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_BEGIN);

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
                    cmdInfo.size = meta->getSize();
                }
                else {
                    cmdInfo.dataType = 0;
                    cmdInfo.format = 0;
                    cmdInfo.size = 0;
                }

                for (int j = 0; j < list->size(); j++) {
                    Item *item = (Item*)list->get(j);
                    if (item == NULL) {
                        LOG.error("SyncManager::sync() - unexpected NULL item.");
                        result = TRUE;
                        goto finally;
                    }
                    // Size might have been included in either the command or the item meta information.
                    // The check for size > 0 is necessary because the function returns 0 even if no
                    // size information was sent by the server - that's okay, for items that really have
                    // size 0 the value doesn't matter as they shouldn't be split into chunks.
                    Meta *itemMeta = item->getMeta();
                    if (itemMeta && itemMeta->getSize() > 0) {
                        cmdInfo.size = itemMeta->getSize();
                    }

                    //
                    // set the syncItem element
                    //
                    status = processSyncItem(item, cmdInfo, syncMLBuilder);

                    if (status) {
                        syncMLBuilder.addItemStatus(previousStatus, status);
                        deleteStatus(&status);
                    }
                }

                if (previousStatus) {
                    statusList.add(previousStatus);
                    deleteArrayList(&previousStatus);                    
                }
            }
        // Fire SyncSourceEvent: END sync of a syncsource (server modifications)
        fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_END);

        }                               
    } // End for (count = 0; count < sourcesNumber; count ++)

    
  finally:
    this->count = oldCount;
    return result;
}


int SyncManager::sync() {

    char* msg            = NULL;
    char* responseMsg    = NULL;
    Status* status       = NULL;
    SyncML* syncml       = NULL;
    /** Current item to be transmitted. Might be split across multiple messages if LargeObjectSupport is on. */
    SyncItem* syncItem   = NULL;
    /** number of bytes already transmitted from syncItem */
    long syncItemOffset  = 0;
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
        if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
            continue;
        toSync++;
    }

    for (count = 0; count < sourcesNumber; count ++) {
        if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
            continue;

        // note: tot == 0 is used to detect when to start iterating over
        // items from the beginning
        tot  = 0;
        step = 0;
        last = FALSE;        
        iterator++;

        // Fire SyncSource event: BEGIN sync of a syncsource (client modifications)
        fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_BEGIN);

        if ( sources[count]->beginSync() ) {
            // Error from SyncSource
            lastErrorCode = ERR_UNSPECIFIED;
            ret = lastErrorCode;
            // syncsource should have set its own errors. If not, set default error.
            if (!sources[count]->getReport() || sources[count]->getReport()->checkState()) {
                setSourceStateAndError(count, SOURCE_ERROR, ERR_UNSPECIFIED, "Error in begin sync");
            }
            continue;
        }
        else
        {
            isAtLeastOneSourceCorrect = TRUE;
        }

        // keep sending changes for current source until done with it
        do {
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

            // Accumulate changes for the current sync source until either
            // - maximum number of items per message exceeded (tot >= maxModPerMsg)
            // - an item cannot be sent completely because the message size would be exceeded
            //
            // In each loop iteration at least one change must be sent to ensure progress.
            // Keeping track of the current message size is a heuristic which assumes a constant
            // overhead for each message and change item and then adds the actual item data sent.
            deleteSyncML(&syncml);
            static long msgOverhead = 2000;
            static long changeOverhead = 150;
            long msgSize = 0;
            Sync* sync = syncMLBuilder.prepareSyncCommand(*sources[count]);
            ArrayList* list = new ArrayList();

            switch (sources[count]->getSyncMode()) {
                case SYNC_SLOW: 
                    {
                        if (syncItem == NULL) {
                            if (tot == 0) {
                                syncItem = getItem(*sources[count], &SyncSource::getFirstItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }
                        }
                        tot = 0;
                        do {
                            if (syncItem == NULL) {
                                syncItem = getItem(*sources[count], &SyncSource::getNextItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }

                            if (tot &&
                                maxMsgSize &&
                                syncItem &&
                                msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                // avoid adding another item that exceeds the message size
                                break;
                            }
                            
                            msgSize += changeOverhead;
                            msgSize +=
                                syncMLBuilder.addItem(modificationCommand,
                                                      syncItemOffset,
                                                      (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                      REPLACE_COMMAND_NAME,
                                                      syncItem, 
                                                      sources[count]->getConfig().getType());

                            if (syncItem) {
                                if (syncItemOffset == syncItem->getDataSize()) {
                                    // the item is only the pointer not another instance. to save mem
                                    delete syncItem; syncItem = NULL;
                                } else {
                                    // assert(msgSize >= maxMsgSize);
                                    break;
                                }
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
                    syncItem = getItem(*sources[count], &SyncSource::getFirstItemKey);
                    if(syncItem) {
                        allItemsList[count]->add((ArrayElement&)*syncItem);
                        delete syncItem; syncItem = NULL;
                    }
                    syncItem = getItem(*sources[count], &SyncSource::getNextItemKey);
                    while(syncItem) {
                        allItemsList[count]->add((ArrayElement&)*syncItem);
                        delete syncItem; syncItem = NULL;
                        syncItem = getItem(*sources[count], &SyncSource::getNextItemKey);	
                    } 
                    break;

                case SYNC_ONE_WAY_FROM_SERVER:
                    last = TRUE;
                    break;

                case SYNC_REFRESH_FROM_CLIENT:
                    {   
                        if (syncItem == NULL) {
                            if (tot == 0) {                    
                                syncItem = getItem(*sources[count], &SyncSource::getFirstItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }
                        }
                        tot = 0;
                        do {
                            if (syncItem == NULL) {
                                syncItem = getItem(*sources[count], &SyncSource::getNextItem);
                                syncItemOffset = 0;
                                if (syncItem) {
                                    // Fire Sync Item Event - Item sent as Updated
                                    fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                }
                            }
                            
                            if (tot &&
                                maxMsgSize &&
                                syncItem &&
                                msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                // avoid adding another item that exceeds the message size
                                break;
                            }
                            
                            msgSize += changeOverhead;
                            msgSize +=
                                syncMLBuilder.addItem(modificationCommand,
                                                      syncItemOffset,
                                                      (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                      REPLACE_COMMAND_NAME, syncItem, 
                                                      sources[count]->getConfig().getType());

                            if (syncItem) {
                                if (syncItemOffset == syncItem->getDataSize()) {
                                    delete syncItem; syncItem = NULL;// the item is only the pointer not another instance. to save mem
                                } else {
                                    // assert(msgSize >= maxMsgSize);
                                    break;
                                }
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
                        if (step == 0) {
                            // assert(syncItem == NULL);
                            syncItem = getItem(*sources[count], &SyncSource::getFirstNewItem);
                            syncItemOffset = 0;
                            step++;
                            if (syncItem == NULL)
                                step++;
                        }
                        if (step == 1) {                                                          
                            do {
                                if (syncItem == NULL) {
                                    syncItem = getItem(*sources[count], &SyncSource::getNextNewItem);
                                    syncItemOffset = 0;
                                }

                                if (tot &&
                                    maxMsgSize &&
                                    syncItem &&
                                    msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                    // avoid adding another item that exceeds the message size
                                    break;
                                }
                            
                                msgSize += changeOverhead;
                                msgSize +=
                                    syncMLBuilder.addItem(modificationCommand,
                                                          syncItemOffset,
                                                          (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                          ADD_COMMAND_NAME, 
                                                          syncItem, sources[count]->getConfig().getType());

                                if (syncItem) {
                                    if (syncItemOffset == syncItem->getDataSize()) {
                                        // Fire Sync Item Event - New Item Detected
                                        fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_ADDED_BY_CLIENT);
                                        delete syncItem; syncItem = NULL;
                                    } else {
                                        // assert(msgSize >= maxMsgSize);
                                        break;
                                    }
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

                            // assert(syncItem == NULL);
                            syncItem = getItem(*sources[count], &SyncSource::getFirstUpdatedItem);
                            syncItemOffset = 0;

                            step++;
                            if (syncItem == NULL)
                                step++;

                        }
                        if (step == 3) {
                            do {				
                                if (syncItem == NULL) {
                                    syncItem = getItem(*sources[count], &SyncSource::getNextUpdatedItem);
                                    syncItemOffset = 0;
                                }

                                if (tot &&
                                    maxMsgSize &&
                                    syncItem &&
                                    msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                    // avoid adding another item that exceeds the message size
                                    break;
                                }
                            

                                msgSize += changeOverhead;
                                msgSize +=
                                    syncMLBuilder.addItem(modificationCommand,
                                                          syncItemOffset,
                                                          (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                          REPLACE_COMMAND_NAME, 
                                                          syncItem, sources[count]->getConfig().getType());

                                if (syncItem) {  
                                    if (syncItemOffset == syncItem->getDataSize()) {
                                        // Fire Sync Item Event - Item Updated
                                        fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_UPDATED_BY_CLIENT);
                                        delete syncItem; syncItem = NULL;
                                    } else {
                                        // assert(msgSize >= maxMsgSize);
                                        break;
                                    }
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

                            syncItem = getItem(*sources[count], &SyncSource::getFirstDeletedItem);
                            syncItemOffset = 0;

                            step++;
                            if (syncItem == NULL)
                                step++;
                        }
                        if (step == 5) {
                            do {
                                if (syncItem == NULL) {
                                    syncItem = getItem(*sources[count], &SyncSource::getNextDeletedItem);
                                    syncItemOffset = 0;
                                }

                                if (tot &&
                                    maxMsgSize &&
                                    syncItem &&
                                    msgSize + changeOverhead + syncItem->getDataSize() - syncItemOffset > maxMsgSize) {
                                    // avoid adding another item that exceeds the message size
                                    break;
                                }

                                msgSize += changeOverhead;
                                msgSize +=
                                    syncMLBuilder.addItem(modificationCommand,
                                                          syncItemOffset,
                                                          (maxMsgSize && loSupport) ? (maxMsgSize - msgSize) : LONG_MAX,
                                                          DELETE_COMMAND_NAME, 
                                                          syncItem, sources[count]->getConfig().getType());

                                if (syncItem) {
                                    if (syncItemOffset == syncItem->getDataSize()) {
                                        // Fire Sync Item Event - Item Deleted
                                        fireSyncItemEvent(sources[count]->getConfig().getURI(), syncItem->getKey(), ITEM_DELETED_BY_CLIENT);
                                        delete syncItem; syncItem = NULL;
                                    } else {
                                        // assert(msgSize >= maxMsgSize);
                                        break;
                                    }
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
            long realMsgSize = strlen(msg);
            LOG.debug("%s estimated size %ld, allowed size %ld, real size %ld / estimated size %ld = %ld%%",
                      MSG_MODIFICATION_MESSAGE,
                      msgSize, maxMsgSize, realMsgSize, msgSize,
                      msgSize ? (100 * realMsgSize / msgSize) : 100);
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
                sprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
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
                setSourceStateAndError(count, SOURCE_ERROR, itemret, "Error in sync status sent by server.");
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
        fireSyncSourceEvent(sources[count]->getConfig().getURI(), sources[count]->getConfig().getName(), sources[count]->getSyncMode(), 0, SYNC_SOURCE_END);

    } // end for (count = 0; count < sourcesNumber; count ++)
    
    if (isToExit()) {
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
            if(!sources[count]->getReport() || !sources[count]->getReport()->checkState())
                continue;
            if ((sources[count]->getSyncMode() != SYNC_ONE_WAY_FROM_CLIENT) &&
                (sources[count]->getSyncMode() != SYNC_REFRESH_FROM_CLIENT))
            {
                alert = syncMLBuilder.prepareAlert(*sources[count]);
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
            sprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
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
                    sprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
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

    char* mapMsg            = NULL;
    char* responseMsg       = NULL;
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
        if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
            continue;
        if ((sources[count]->getSyncMode()) != SYNC_ONE_WAY_FROM_CLIENT &&
            (sources[count]->getSyncMode()) != SYNC_REFRESH_FROM_CLIENT )

        toSync++;
    }

    for (count = 0; count < sourcesNumber; count ++) {
        if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
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
                    sprintf(lastErrorMsg, T("Server Failure: server returned error code %i"), ret);
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
        if (!sources[count]->getReport() || !sources[count]->getReport()->checkState())
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
    char g[768]; sprintf(g, "ret: %i, lastErrorCode: %i, lastErrorMessage: %s", ret, lastErrorCode, lastErrorMsg); LOG.debug(g);

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
    char anchor[DIM_ANCHOR];
    
    if (config.getSyncSourceConfig(_wcc(source.getName())) == NULL) {
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

    const char* name = _wcc(source.getName());
    unsigned long next = source.getNextSync();

    char anchor[DIM_ANCHOR];
    timestampToAnchor(next, anchor);

    sprintf(logmsg, DBG_COMMITTING_SOURCE, name, anchor);
    LOG.debug(logmsg);

    for (unsigned int i = 0; i<n; ++i) {
        if (strcmp(name, configs[i].getName()) == NULL) {
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

SyncItem* SyncManager::getItem(SyncSource& source, SyncItem* (SyncSource::* getItemFunction)()) {
    SyncItem *syncItem = (source.*getItemFunction)();

    if (!syncItem) {
        return NULL;
    }

    // change encoding automatically?
    const char* encoding   = source.getConfig().getEncoding();
    const char* encryption = source.getConfig().getEncryption();
    if (!syncItem->getDataEncoding()) {
        if ( (encoding && encoding[0]) || (encryption && encryption[0]) ) {
            if (syncItem->changeDataEncoding(encoding, encryption, credentialInfo)) {
                delete syncItem;
                syncItem = NULL;
            }
        }
    }
    return syncItem;
}

Status *SyncManager::processSyncItem(Item* item, const CommandInfo &cmdInfo, SyncMLBuilder &syncMLBuilder)
{
    int code = 0;
    const char* itemName;
    Status *status = 0;

    Source* s = item->getSource();
    if (s) { 
        itemName = s->getLocURI();
    }
    else {
        Target* t = item->getTarget();                
        itemName = t->getLocURI();
    }

    // Fill item -------------------------------------------------
    WCHAR *iname = toWideChar(itemName);
    BOOL append = TRUE;
    if (incomingItem) {
        BOOL newItem = FALSE;
        
        if (iname) {
            if (incomingItem->getKey()) {
                if(wcscmp(incomingItem->getKey(), iname)) {
                    // another item before old one is complete
                    newItem = TRUE;
                }
            } else {
                incomingItem->setKey(iname);
            }
        }

        // if no error yet, also check for the same command and same source
        if (incomingItem->cmdName.c_str() && strcmp(incomingItem->cmdName.c_str(), cmdInfo.commandName) ||
            count != incomingItem->sourceIndex) {
            newItem = TRUE;
        }
        if (newItem) {
            // send 223 alert:
            //
            // "The Alert should contain the source and/or target information from
            // the original command to enable the sender to identify the failed command."
            //
            // The target information is the one from the item's source.
            Alert *alert = syncMLBuilder.prepareAlert(*sources[incomingItem->sourceIndex], 223);
            commands->add(*alert);
            delete alert;

            delete incomingItem;
            incomingItem = NULL;
        }
    } else {
        incomingItem = new IncomingSyncItem(iname, cmdInfo, count);

        // incomplete item?
        if (item->isMoreData()) {
            // reserve buffer in advance, append below
            long size = cmdInfo.size;
            if (size < 0 || maxObjSize && size > maxObjSize) {
                // invalid size, "Request entity too large"
                status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, itemName, cmdInfo.cmdRef, 416);
                delete incomingItem;
                incomingItem = NULL;
            } else {
                incomingItem->setData(NULL, size);
            }
        } else {
            // simply copy all data below
            append = FALSE;
        }
    }
    delete [] iname;

    if (incomingItem) {
        ComplexData *cdata = item->getData();
        if (cdata) {
            char* data = cdata->getData();
            char* format = 0;

            //
            // Retrieving how the content has been encoded.
            // Remember that in the item and convert it once it is
            // complete.
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
            if (format && !incomingItem->getDataEncoding()) {
                incomingItem->setDataEncoding(format);
            }

            // append or set new data
            long size = strlen(data);
            if (append) {
                if (size + incomingItem->offset > incomingItem->getDataSize()) {
                    // overflow, signal error: "Size mismatch"
                    status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, itemName, cmdInfo.cmdRef, 424);
                    sprintf(lastErrorMsg, "Item size mismatch: real size = %d, declared size = %d", incomingItem->offset, incomingItem->getDataSize());
                    lastErrorCode = OBJECT_SIZE_MISMATCH;
                    delete incomingItem;
                    incomingItem = NULL;
                } else {
                    memcpy((char *)incomingItem->getData() + incomingItem->offset, data, size);
                }
            } else {
                incomingItem->setData(data, size);
            }
            if (incomingItem) {
                incomingItem->offset += size;
            }
        }
    }

    if (incomingItem) {
        if (cmdInfo.dataType) {
            WCHAR *dtype = toWideChar(cmdInfo.dataType);
            incomingItem->setDataType(dtype);
            delete [] dtype;
        }
        WCHAR *sparent = toWideChar(item->getSourceParent());
        incomingItem->setSourceParent(sparent);
        delete [] sparent;
        WCHAR *tparent = toWideChar(item->getTargetParent());
        incomingItem->setTargetParent(tparent);
        delete [] tparent;

        incomingItem->setModificationTime(sources[count]->getNextSync());

        if (!item->isMoreData()) {
            // sanity check: is the item complete?
            // >= is used because for deleted items the server might have sent -1 (Synthesis server does that).
            if (incomingItem->offset >= incomingItem->getDataSize()) {
                // attempt to transform into plain format, if that fails let the client deal with
                // the encoded content
                incomingItem->changeDataEncoding(SyncItem::encodings::plain, NULL, credentialInfo);

                // Process item ------------------------------------------------------------
                if ( strcmp(cmdInfo.commandName, ADD) == 0) {  
                    // Fire Sync Item Event - New Item Added by Server
                    fireSyncItemEvent(sources[count]->getConfig().getURI(), incomingItem->getKey(), ITEM_ADDED_BY_SERVER);

                    incomingItem->setState(SYNC_STATE_NEW);
                    code = sources[count]->addItem(*incomingItem);      
                    status = syncMLBuilder.prepareItemStatus(ADD, itemName, cmdInfo.cmdRef, code);

                    // Fire Sync Status Event: item status from client
                    fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getURI(), incomingItem->getKey(), CLIENT_STATUS);
                    // Update SyncReport
                    sources[count]->getReport()->addItem(CLIENT, COMMAND_ADD, incomingItem->getKey(), status->getStatusCode());

                    // If the add was successful, set the id mapping
                    if (code >= 200 && code <= 299) {
                        char *key = toMultibyte(incomingItem->getKey());
                        SyncMap syncMap(item->getSource()->getLocURI(), key);
                        mappings[count]->add(syncMap);
                        delete [] key;
                    }                    
                }
                else if (strcmp(cmdInfo.commandName, REPLACE) == 0) {  
                    // Fire Sync Item Event - Item Updated by Server
                    fireSyncItemEvent(sources[count]->getConfig().getURI(), incomingItem->getKey(), ITEM_UPDATED_BY_SERVER);

                    incomingItem->setState(SYNC_STATE_UPDATED);
                    code = sources[count]->updateItem(*incomingItem);
                    status = syncMLBuilder.prepareItemStatus(REPLACE, itemName, cmdInfo.cmdRef, code);                
            
                    // Fire Sync Status Event: item status from client
                    fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getURI(), incomingItem->getKey(), CLIENT_STATUS);
                    // Update SyncReport
                    sources[count]->getReport()->addItem(CLIENT, COMMAND_REPLACE, incomingItem->getKey(), status->getStatusCode());
                }
                else if (strcmp(cmdInfo.commandName, DEL) == 0) {
                    // Fire Sync Item Event - Item Deleted by Server
                    fireSyncItemEvent(sources[count]->getConfig().getURI(), incomingItem->getKey(), ITEM_DELETED_BY_SERVER);

                    incomingItem->setState(SYNC_STATE_DELETED);
                    code = sources[count]->deleteItem(*incomingItem);        
                    status = syncMLBuilder.prepareItemStatus(DEL, itemName, cmdInfo.cmdRef, code);           
	    
                    // Fire Sync Status Event: item status from client
                    fireSyncStatusEvent(status->getCmd(), status->getStatusCode(), sources[count]->getConfig().getURI(), incomingItem->getKey(), CLIENT_STATUS);
                    // Update SyncReport
                    sources[count]->getReport()->addItem(CLIENT, COMMAND_DELETE, incomingItem->getKey(), status->getStatusCode());
                }

                delete incomingItem;
                incomingItem = NULL;
            } else {
                // error: incomplete item, but no more data - "Size Mismatch"
                status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, itemName, cmdInfo.cmdRef, 424);
                sprintf(lastErrorMsg, "Item size mismatch: real size = %d, declared size = %d", incomingItem->offset, incomingItem->getDataSize());
                lastErrorCode = OBJECT_SIZE_MISMATCH;
                delete incomingItem;
                incomingItem = NULL;
            }
        } else {
            // keep the item, tell server "Chunked item accepted and buffered"
            status = syncMLBuilder.prepareItemStatus(cmdInfo.commandName, itemName, cmdInfo.cmdRef, 213);
        }
    }

    if (incomingItem) {
        // Make sure that more data comes by asking for it.
        // The standard says that if there are other commands, the 222 alert
        // may be omitted, but because that's hard to determine here we always
        // send it, just to be on the safe side.
        Alert *alert = syncMLBuilder.prepareAlert(*sources[count], 222);
        commands->add(*alert);
        delete alert;
    }
    
    return status;
}

/*
 * Creates the device info for this client and sources.
 */
DevInf *SyncManager::createDeviceInfo()
{
    const char *rxType, *rxVer,
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
    devinfo->setSupportLargeObjs(loSupport);
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
        const char *syncModes = (*source)->getConfig().getSyncModes();
        if (syncModes) {
            char buffer[80];
            const char *mode = syncModes;

            while (*mode) {
                // skip leading spaces and commas
                while (isspace(*mode) || *mode == ',') {
                    mode++;
                }
                // fast-forward to comma
                const char *eostr = mode;
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
                memcpy(buffer, mode, sizeof(char) * len);
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
static void fillContentTypeInfoList(ArrayList &l, const char* types) { 
 
    l.clear();
    if (!types) {
        return;
    }

    char typeName[80];
    char typeVersion[20];
    const char* curr = types;
    const char* eostr = NULL;
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
 * @return      : user agent property as a new char*
 *                (need to be freed by the caller)
 */
const char* SyncManager::getUserAgent(SyncManagerConfig& config) {

    char* ret;
    StringBuffer userAgent(config.getAccessConfig().getUserAgent());
    StringBuffer buffer;
    
    if (userAgent.length()) {
        ret = stringdup(userAgent.c_str());
    }
    // Use 'mod + SwV' parameters for user agent
    else {
        const char* mod = config.getDeviceConfig().getMod();
        const char* swV = config.getDeviceConfig().getSwv();
        if (mod && strcmp(mod, "")) {
            buffer.append(mod);
            if (swV && strcmp(swV, "")) {
                buffer.append(" ");
                buffer.append(swV);
            }
            ret = stringdup(buffer.c_str());
        } else {
            // Default user agent value
            ret = stringdup(CHAR_USER_AGENT);
        }
    }

    return ret;
}

