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
#include "base/debug.h"
#include "base/errors.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "client/DMTClientConfig.h"
#include "spdm/constants.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/DMTree.h"
#include "spdm/ManagementNode.h"


void DMTClientConfig::initialize() {
    dmt = NULL;
    syncMLNode = NULL;
    sourcesNode = NULL;
}

DMTClientConfig::DMTClientConfig() : SyncManagerConfig() {
    initialize();
    rootContext = 0;
}


DMTClientConfig::DMTClientConfig(const BCHAR* root): SyncManagerConfig() {
    initialize();
    rootContext = new BCHAR[bstrlen(root)+1];
    bstrcpy(rootContext, root);
}


DMTClientConfig::~DMTClientConfig() {
    if (rootContext) {
        delete [] rootContext;
    }
    close();
}

BOOL DMTClientConfig::getSyncSourceConfig(
						const BCHAR* name,
						SyncSourceConfig &sc,
						BOOL refresh)
{
    if ((name == NULL) || (bstrlen(name) == 0)) {
        return FALSE;
    }

    //
    // If refresh is true, we need to re-read the syncsource settings
    // from the DM tree.
    //
    // PS: for now we use a brute force approach so that we refresh the
    // entire configuration. A better implementation would be to just
    // refresh the single source.
    //
    if (refresh) {
      read();
    }

    for (unsigned int i=0; i<sourceConfigsCount; ++i) {
        if (bstrcmp(sourceConfigs[i].getName(), name) == 0) {
            sc.assign(sourceConfigs[i]);
            return TRUE;
        }
    }

    return FALSE;
}


BOOL DMTClientConfig::read() {
    int n = 0, i = 0; // number of sync sources

    BOOL ret = FALSE;

    LOG.debug(DBG_READING_CONFIG_FROM_DM);

    //
    // Reading syncml node
    //
    //BCHAR nodeName[DIM_MANAGEMENT_PATH];

    if (!open()) {
        return FALSE;
    }

    readAccessConfig(*syncMLNode);
    readDeviceConfig(*syncMLNode);

    n = sourcesNode->getChildrenMaxCount();

    //
    // Let's remove previously created config objects and reinitialize
    // sourceConfigs
    //
    if (sourceConfigs) {
        delete [] sourceConfigs;
    }
    sourceConfigs = new SyncSourceConfig[n];
    sourceConfigsCount = n;

    for (i=0; i<n; ++i) {
        // node owns children, we must not delete them
        readSourceConfig(i, *(sourcesNode) );
    }

    ret = TRUE;

//finally:

    close();
    return ret;
}


BOOL DMTClientConfig::save() {
    BOOL ret = FALSE;
    unsigned int i = 0;

    LOG.debug(DBG_WRITING_CONFIG_TO_DM);
    if (!open()) {
        return FALSE;
    }

    if (accessConfig.getDirty()) {
        resetError();

        //
        // SyncML management node
        //
        saveAccessConfig(*syncMLNode);
    }
    //
    // TBD: handle the dirty flag
    //

    saveDeviceConfig(*syncMLNode);

    //
    // Sources management node
    //
    for(i=0; i<sourceConfigsCount; ++i) {
        saveSourceConfig(i, *(sourcesNode) );

        if (lastErrorCode != ERR_NONE) {
            goto finally;
        }
    }

    ret = (lastErrorCode == ERR_NONE);

finally:

    close();
    return ret;
}

BOOL DMTClientConfig::open() {
    BCHAR nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;

    dmt = DMTreeFactory::getDMTree(rootContext);

    bsprintf(nodeName, T("%s%s"), rootContext, T(CONTEXT_SPDS_SYNCML));
    syncMLNode = dmt->getManagementNode(nodeName);
    if (!syncMLNode ) {
        goto failed;
    }
    
    bsprintf(nodeName, T("%s%s"), rootContext, T(CONTEXT_SPDS_SOURCES));
    sourcesNode = dmt->getManagementNode(nodeName);
    if (!sourcesNode) {
        goto failed;
    }

    return TRUE;

failed:
    lastErrorCode = ERR_INVALID_CONTEXT;
    bsprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, nodeName);
    close();
    return FALSE;
}

ManagementNode* DMTClientConfig::getSyncMLNode() {
    return syncMLNode;
}

int DMTClientConfig::getNumSources() {
    return sourcesNode ?
        sourcesNode->getChildrenMaxCount() :
        -1;
}

ManagementNode* DMTClientConfig::getSyncSourceNode(int index) {
    return sourcesNode ?
        sourcesNode->getChild(index) :
        NULL;
}

void DMTClientConfig::close() {
    if (syncMLNode) {
        delete syncMLNode;
        syncMLNode = NULL;
    }
    if (sourcesNode) {
        delete sourcesNode;
        sourcesNode = NULL;
    }
    if (dmt) {
        delete dmt;
        dmt = NULL;
    }
}


/*
 * Read Access Config properties stored in DMTree.
 * Access properties are placed in 3 nodes under syncML node
 * (Auth - Conn - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 * @return : TRUE if config is correctly read
 */
BOOL DMTClientConfig::readAccessConfig(ManagementNode& n) {

    BOOL ret = TRUE;
    BCHAR *tmp;
    BCHAR nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;
    ManagementNode* node;

    BCHAR syncMLContext[DIM_MANAGEMENT_PATH];
    bsprintf(syncMLContext, T("%s"), n.getFullName());

    //
    // Auth properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_AUTH));
    node = dmt->getManagementNode(nodeName);
    if (node) {

        tmp = node->getPropertyValue(PROPERTY_USERNAME);
        accessConfig.setUsername(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_PASSWORD);
        accessConfig.setPassword(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SERVER_ID);     
        accessConfig.setServerID(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SERVER_PWD);     
        accessConfig.setServerPWD(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SERVER_NONCE);     
        accessConfig.setServerNonce(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_CLIENT_NONCE);     
        accessConfig.setClientNonce(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_CLIENT_AUTH_TYPE);     
        accessConfig.setClientAuthType(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SERVER_AUTH_TYPE);     
        accessConfig.setServerAuthType(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_IS_SERVER_REQUIRED);     
        accessConfig.setServerAuthRequired((*tmp == '1') ? TRUE : FALSE);
        delete [] tmp;

        delete node; 
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    //
    // Conn properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_CONN));
    node = dmt->getManagementNode(nodeName);
    if (node) {
    
        tmp = node->getPropertyValue(PROPERTY_SYNC_URL);
        accessConfig.setSyncURL(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_USE_PROXY);     
        accessConfig.setUseProxy((*tmp == '1') ? TRUE : FALSE);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_PROXY_HOST);     
        accessConfig.setProxyHost(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_PROXY_PORT);     
        accessConfig.setProxyPort(bstrtol(tmp, NULL, 10));
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_PROXY_USERNAME);     
        accessConfig.setProxyUsername(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_PROXY_PASSWORD);     
        accessConfig.setProxyPassword(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_CHECK_CONN);     
        accessConfig.setCheckConn((*tmp == '1') ? TRUE : FALSE);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_RESPONSE_TIMEOUT);     
        accessConfig.setResponseTimeout(bstrtol(tmp, NULL, 10));
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_READ_BUFFER_SIZE);     
        accessConfig.setReadBufferSize(bstrtol(tmp, NULL, 10));
        delete [] tmp;
        
        tmp = node->getPropertyValue(PROPERTY_USER_AGENT);     
        accessConfig.setUserAgent(tmp);
        delete [] tmp;

        delete node; 
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    //
    // Ext properties (other misc props)
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_EXT));
    node = dmt->getManagementNode(nodeName);
    if (node) {

        tmp = node->getPropertyValue(PROPERTY_FIRST_TIME_SYNC_MODE);
        SyncMode i = (SyncMode)(*tmp ? bstrtol(tmp, NULL, 10) : 0);
        accessConfig.setFirstTimeSyncMode(i);   
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_MAX_MSG_SIZE);     
        accessConfig.setMaxMsgSize(bstrtol(tmp, NULL, 10));
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_ENCRYPTION);     
        accessConfig.setEncryption((*tmp == '1') ? TRUE : FALSE);
        delete [] tmp;
        
        tmp = node->getPropertyValue(PROPERTY_SYNC_BEGIN);     
        accessConfig.setBeginSync(bstrtol(tmp, NULL, 10));
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SYNC_END);     
        accessConfig.setEndSync(bstrtol(tmp, NULL, 10));
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_MAX_MOD_PER_MSG);     
        accessConfig.setMaxModPerMsg(bstrtol(tmp, NULL, 10));
        delete [] tmp;

        delete node; 
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    return TRUE;
}

/*
 * Save Access Config properties in DMTree.
 * Access properties are placed in 3 nodes under syncML node
 * (Auth - Conn - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 */
void DMTClientConfig::saveAccessConfig(ManagementNode& n) {

    BCHAR buf[512];
    ManagementNode* node;
    BCHAR nodeName[DIM_MANAGEMENT_PATH];

    BCHAR syncMLContext[DIM_MANAGEMENT_PATH];
    bsprintf(syncMLContext, T("%s"), n.getFullName());

    //
    // Auth properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_AUTH));
    node = dmt->getManagementNode(nodeName);
    if (node) {
        node->setPropertyValue(PROPERTY_USERNAME, accessConfig.getUsername());
        node->setPropertyValue(PROPERTY_PASSWORD, accessConfig.getPassword());
        node->setPropertyValue(PROPERTY_SERVER_ID, accessConfig.getServerID());
        node->setPropertyValue(PROPERTY_SERVER_PWD, accessConfig.getServerPWD());
        node->setPropertyValue(PROPERTY_SERVER_NONCE, accessConfig.getServerNonce());
        node->setPropertyValue(PROPERTY_CLIENT_NONCE, accessConfig.getClientNonce());
        node->setPropertyValue(PROPERTY_CLIENT_AUTH_TYPE, accessConfig.getClientAuthType());
        node->setPropertyValue(PROPERTY_SERVER_AUTH_TYPE, accessConfig.getServerAuthType());
        node->setPropertyValue(PROPERTY_IS_SERVER_REQUIRED,
		                      (accessConfig.getServerAuthRequired() ? T("1") : T("0") ) ); 
        delete node; 
        node = NULL;
    }

    //
    // Conn properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_CONN));
    node = dmt->getManagementNode(nodeName);
    if (node) {

        node->setPropertyValue(PROPERTY_SYNC_URL, accessConfig.getSyncURL());
        node->setPropertyValue(PROPERTY_USE_PROXY,
		                      (accessConfig.getUseProxy() ? T("1"): T("0")) );
        node->setPropertyValue(PROPERTY_PROXY_HOST, accessConfig.getProxyHost());
        bsprintf(buf, T("%lu"), accessConfig.getProxyPort());
        node->setPropertyValue(PROPERTY_PROXY_PORT, buf);
        node->setPropertyValue(PROPERTY_PROXY_USERNAME, accessConfig.getProxyUsername());
        node->setPropertyValue(PROPERTY_PROXY_PASSWORD, accessConfig.getProxyPassword());
        node->setPropertyValue(PROPERTY_CHECK_CONN,
		                      (accessConfig.getCheckConn() ? T("1"): T("0")) );
        bsprintf(buf, T("%lu"), accessConfig.getResponseTimeout());
        node->setPropertyValue(PROPERTY_RESPONSE_TIMEOUT, buf); 
        bsprintf(buf, T("%lu"), accessConfig.getReadBufferSize());
        node->setPropertyValue(PROPERTY_READ_BUFFER_SIZE, buf);  
        node->setPropertyValue(PROPERTY_USER_AGENT, accessConfig.getUserAgent());
        delete node; 
        node = NULL;
    }

    //
    // Ext properties (other misc props)
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_EXT));
    node = dmt->getManagementNode(nodeName);
    if (node) {
        bsprintf(buf, T("%lu"), accessConfig.getFirstTimeSyncMode());
        node->setPropertyValue(PROPERTY_FIRST_TIME_SYNC_MODE, buf);

        bsprintf(buf, T("%lu"), accessConfig.getMaxMsgSize());
        node->setPropertyValue(PROPERTY_MAX_MSG_SIZE, buf);

        node->setPropertyValue(PROPERTY_ENCRYPTION,
		                      (accessConfig.getEncryption() ? T("1") : T("0") ) ); 
        
        timestampToAnchor(accessConfig.getBeginSync(), buf);
        node->setPropertyValue(PROPERTY_SYNC_BEGIN, buf);

        timestampToAnchor(accessConfig.getEndSync(), buf);
        node->setPropertyValue(PROPERTY_SYNC_END, buf);

        bsprintf(buf, T("%lu"), accessConfig.getMaxModPerMsg());
        node->setPropertyValue(PROPERTY_MAX_MOD_PER_MSG, buf);
    
        delete node; 
        node = NULL;
    }
}


/*
 * Read Device Config properties stored in DMTree.
 * Device properties are placed in 3 nodes under syncML node
 * (DevInfo - DevDetail - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 * @return : TRUE if config is correctly read
 */
BOOL DMTClientConfig::readDeviceConfig(ManagementNode& n) {

    BOOL ret = TRUE;
    BCHAR *tmp;
    BCHAR nodeName[DIM_MANAGEMENT_PATH];
    nodeName[0] = 0;
    ManagementNode* node;

    BCHAR syncMLContext[DIM_MANAGEMENT_PATH];
    bsprintf(syncMLContext, T("%s"), n.getFullName());

    //
    // DevInfo properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_DEV_INFO));
    node = dmt->getManagementNode(nodeName);
    if (node) {

        tmp = node->getPropertyValue(PROPERTY_DEVICE_ID);
        deviceConfig.setDevID(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_MANUFACTURER);
        deviceConfig.setMan(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_MODEL);
        deviceConfig.setMod(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_DS_VERSION);
        deviceConfig.setDsV(tmp);
        delete [] tmp;

        delete node; 
        node = NULL;
    }
    else {
        ret = FALSE;
    }
    
    //
    // DevDetail properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_DEV_DETAIL));
    node = dmt->getManagementNode(nodeName);
    if (node) {

        tmp = node->getPropertyValue(PROPERTY_DEVICE_TYPE);
        deviceConfig.setDevType(tmp);
        delete [] tmp;
    
        tmp = node->getPropertyValue(PROPERTY_OEM);
        deviceConfig.setOem(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_FIRMWARE_VERSION);
        deviceConfig.setFwv(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SOFTWARE_VERSION);
        deviceConfig.setSwv(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_HARDWARE_VERSION);
        deviceConfig.setHwv(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_LARGE_OBJECT_SUPPORT);
        deviceConfig.setLoSupport((*tmp == '1') ? TRUE : FALSE);
        delete [] tmp;

        delete node; 
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    //
    // Ext properties (other misc props)
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_EXT));
    node = dmt->getManagementNode(nodeName);
    if (node) {

        tmp = node->getPropertyValue(PROPERTY_VER_DTD);
        deviceConfig.setVerDTD(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_UTC);
        deviceConfig.setUtc((*tmp == '1') ? TRUE : FALSE);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT);
        deviceConfig.setNocSupport((*tmp == '1') ? TRUE : FALSE);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_LOG_LEVEL);
        LogLevel l = (LogLevel)bstrtol(tmp, NULL, 10);
        deviceConfig.setLogLevel(l);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_MAX_OBJ_SIZE);
        deviceConfig.setMaxObjSize(bstrtol(tmp, NULL, 10));
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_DEVINF_HASH);
        deviceConfig.setDevInfHash(tmp);
        delete [] tmp;

        delete node; 
        node = NULL;
    }
    else {
        ret = FALSE;
    }

    return ret;
}


/*
 * Save Device Config properties in DMTree.
 * Device properties are placed in 3 nodes under syncML node
 * (DevInfo - DevDetail - Ext)
 *
 * @param n: the 'syncml' node (parent node)
 */
void DMTClientConfig::saveDeviceConfig(ManagementNode& n) {

    BCHAR buf[512];
    ManagementNode* node;
    BCHAR nodeName[DIM_MANAGEMENT_PATH];

    BCHAR syncMLContext[DIM_MANAGEMENT_PATH];
    bsprintf(syncMLContext, T("%s"), n.getFullName());

    //
    // DevInfo properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_DEV_INFO));
    node = dmt->getManagementNode(nodeName);
    if (node) {
        node->setPropertyValue(PROPERTY_DEVICE_ID, deviceConfig.getDevID());
        node->setPropertyValue(PROPERTY_MANUFACTURER, deviceConfig.getMan());
        node->setPropertyValue(PROPERTY_MODEL, deviceConfig.getMod());
        node->setPropertyValue(PROPERTY_DS_VERSION, deviceConfig.getDsV());
        delete node; 
        node = NULL;
    }

    //
    // DevDetail properties
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_DEV_DETAIL));
    node = dmt->getManagementNode(nodeName);
    if (node) {
        node->setPropertyValue(PROPERTY_DEVICE_TYPE, deviceConfig.getDevType());
        node->setPropertyValue(PROPERTY_OEM, deviceConfig.getOem());
        node->setPropertyValue(PROPERTY_FIRMWARE_VERSION, deviceConfig.getFwv());
        node->setPropertyValue(PROPERTY_SOFTWARE_VERSION, deviceConfig.getSwv());
        node->setPropertyValue(PROPERTY_HARDWARE_VERSION, deviceConfig.getHwv());
        node->setPropertyValue(PROPERTY_LARGE_OBJECT_SUPPORT, 
                              (deviceConfig.getLoSupport() ? T("1"): T("0")) );
        delete node; 
        node = NULL;
    }

    //
    // Ext properties (other misc props)
    //
    bsprintf(nodeName, T("%s%s"), syncMLContext, T(CONTEXT_EXT));
    node = dmt->getManagementNode(nodeName);
    if (node) {
        node->setPropertyValue(PROPERTY_VER_DTD, deviceConfig.getVerDTD());
        node->setPropertyValue(PROPERTY_DEVINF_HASH, deviceConfig.getDevInfHash());
        node->setPropertyValue(PROPERTY_UTC, 
                              (deviceConfig.getUtc() ? T("1"): T("0")) );
        node->setPropertyValue(PROPERTY_NUMBER_OF_CHANGES_SUPPORT, 
                              (deviceConfig.getNocSupport() ? T("1"): T("0")) );

        bsprintf(buf, T("%lu"), deviceConfig.getLogLevel());
        node->setPropertyValue(PROPERTY_LOG_LEVEL, buf);
        
        bsprintf(buf, T("%lu"), deviceConfig.getMaxObjSize());
        node->setPropertyValue(PROPERTY_MAX_OBJ_SIZE, buf);

        delete node; 
        node = NULL;
    }
}



/*
 * Read Source Config properties in DMTree for the desired Source.
 * Source properties are placed in specific node under sources node.
 *
 * @param i   : the index of SyncSource
 * @param n   : the sourceNode (parent node)
 */
BOOL DMTClientConfig::readSourceConfig(int i, ManagementNode& n) {    

    BCHAR *tmp;
    ManagementNode* node;

    node = n.getChild(i);

    if (node) {
        tmp = node->getPropertyValue(PROPERTY_SOURCE_NAME);    
        sourceConfigs[i].setName(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SOURCE_URI);    
        sourceConfigs[i].setURI(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SOURCE_SYNC_MODES);    
        sourceConfigs[i].setSyncModes(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SOURCE_SYNC);    
        sourceConfigs[i].setSync(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SOURCE_TYPE);    
        sourceConfigs[i].setType(tmp);
        delete [] tmp;

        tmp = node->getPropertyValue(PROPERTY_SOURCE_LAST_SYNC);    
        sourceConfigs[i].setLast( ((*tmp) ? bstrtol(tmp, NULL, 10) : 0) );
        delete [] tmp;
        
        tmp = node->getPropertyValue(PROPERTY_SOURCE_ENCODING);    
        sourceConfigs[i].setEncoding(tmp);
        delete [] tmp;

        // *** TBD ***
        // CTCap c = getCtCap that is stored somewhere...
        //sourceConfigs[i].setCtCap(c);
    }
    return TRUE;
}


/*
 * Save Source Config properties in DMTree for the desired Source.
 * Source properties are placed in specific node under sources node.
 * Note:
 * if the node for the current source is not found, it is created.
 *
 * @param i   : the index of SyncSource
 * @param n   : the sourceNode (parent node)
 */
void DMTClientConfig::saveSourceConfig(int i, ManagementNode& n) {

    BCHAR buf[512];
    ManagementNode* node;
    BCHAR nodeName[DIM_MANAGEMENT_PATH];

    if (n.getChild(i) == NULL) {
        // Create node from Source name.
        bsprintf(nodeName, T("%s/%s"), n.getFullName(), sourceConfigs[i].getName());
        node = dmt->getManagementNode(nodeName);
    }
    else {
        node = (ManagementNode*)n.getChild(i)->clone();
    }

    if (node) {
        node->setPropertyValue(PROPERTY_SOURCE_NAME, sourceConfigs[i].getName());    
        node->setPropertyValue(PROPERTY_SOURCE_URI, sourceConfigs[i].getURI());
        node->setPropertyValue(PROPERTY_SOURCE_TYPE, sourceConfigs[i].getType());
        node->setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, sourceConfigs[i].getSyncModes());
        node->setPropertyValue(PROPERTY_SOURCE_SYNC, sourceConfigs[i].getSync());    
        node->setPropertyValue(PROPERTY_SOURCE_ENCODING, sourceConfigs[i].getEncoding());    

        timestampToAnchor(sourceConfigs[i].getLast(), buf); 
        node->setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, buf);   

        // *** TBD ***
        // CTCap c = sourceConfigs[i].getCtCap();
        // saveCtCap() somewhere...

        delete node; 
    }
}

