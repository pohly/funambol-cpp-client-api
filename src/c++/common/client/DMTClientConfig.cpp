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


DMTClientConfig::DMTClientConfig() : SyncManagerConfig() {
    rootContext = 0;
}


DMTClientConfig::DMTClientConfig(const wchar_t* root): SyncManagerConfig() {
    rootContext = new wchar_t[wcslen(root)+1];
    wcscpy(rootContext, root);
}


DMTClientConfig::~DMTClientConfig() {
    if (rootContext) {
        delete [] rootContext;
    }
}

BOOL DMTClientConfig::getSyncSourceConfig(
						const wchar_t* name,
						SyncSourceConfig &sc,
						BOOL refresh)
{
    if ((name == NULL) || (wcslen(name) == 0)) {
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
        if (wcscmp(sourceConfigs[i].getName(), name) == 0) {
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
    wchar_t nodeName[DIM_MANAGEMENT_PATH];

    wmemset(nodeName, 0, DIM_MANAGEMENT_PATH);
    wcscpy(nodeName, rootContext); wcscat(nodeName, TEXT(CONTEXT_SPDS_SYNCML));

    DMTree* dmt = DMTreeFactory::getDMTree(rootContext);

    ManagementNode *node = dmt->getManagementNode(nodeName);
    
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        wsprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, nodeName);
        goto finally;
    }

    readAccessConfig(*node);
    delete node;
    node = 0;

    wcscpy(nodeName, rootContext); wcscat(nodeName, TEXT(CONTEXT_SPDS_SOURCES));

    node = dmt->getManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        wsprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, nodeName);
        goto finally;
    }
    n = node->getChildrenMaxCount();

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
        readSourceConfig(i, *(node->getChild(i)) );
    }

    ret = TRUE;

finally:

    if (dmt) {
        delete dmt;
    }
    if (node) {
        delete node;
    }

    return ret;
}


BOOL DMTClientConfig::save() {
    BOOL ret = FALSE;
    unsigned int i = 0;
    wchar_t nodeName[DIM_MANAGEMENT_PATH];
	ManagementNode *node = 0;

    DMTree* dmt = DMTreeFactory::getDMTree(rootContext);

    LOG.debug(DBG_WRITING_CONFIG_TO_DM);

    if (accessConfig.getDirty()) {
        resetError();

        //
        // SyncML management node
        //
        wcscpy(nodeName, rootContext); wcscat(nodeName, TEXT(CONTEXT_SPDS_SYNCML));
        
        node = dmt->getManagementNode(nodeName);
        if ( ! node ) {
            lastErrorCode = ERR_INVALID_CONTEXT;
            wsprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, nodeName);
            goto finally;
        }

        saveAccessConfig(*node);

        delete node;
        node = 0;
    }

    //
    // TBD: handle the dirty flag
    //

    //
    // Sources management node
    //
    wcscpy(nodeName, rootContext); wcscat(nodeName, TEXT(CONTEXT_SPDS_SOURCES));

    node = dmt->getManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        wsprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, nodeName);
        goto finally;
    }

    for(i=0; i<sourceConfigsCount; ++i) {
        saveSourceConfig(i, *(node->getChild(i)) );

        if (lastErrorCode != ERR_NONE) {
            goto finally;
        }
    }
    delete node;
    node = 0;

    ret = (lastErrorCode == ERR_NONE);

finally:

    if (dmt) {
        delete dmt;
    }
    if (node) {
        delete node;
    }

    return ret;
}

BOOL DMTClientConfig::readAccessConfig(ManagementNode& n) {

    wchar_t *tmp;
    
    tmp = n.getPropertyValue(PROPERTY_USERNAME);
    accessConfig.setUsername(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_PASSWORD);
    accessConfig.setPassword(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_DEVICE_ID);
    accessConfig.setDeviceId(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SYNC_URL);
    accessConfig.setSyncURL(tmp);
    delete [] tmp;
   
    tmp = n.getPropertyValue(PROPERTY_FIRST_TIME_SYNC_MODE);
    SyncMode i = (SyncMode)(*tmp ? wcstol(tmp, NULL, 10) : 0);
    accessConfig.setFirstTimeSyncMode(i);   
    delete [] tmp;
    
    tmp = n.getPropertyValue(PROPERTY_SYNC_BEGIN);     
    accessConfig.setBeginSync(wcstol(tmp, NULL, 10));
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SYNC_END);     
    accessConfig.setEndSync(wcstol(tmp, NULL, 10));
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_USE_PROXY);     
    accessConfig.setUseProxy((*tmp == 'T') ? TRUE : FALSE);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_PROXY_HOST);     
    accessConfig.setProxyHost(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SERVER_NONCE);     
    accessConfig.setServerNonce(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_CLIENT_NONCE);     
    accessConfig.setClientNonce(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SERVER_ID);     
    accessConfig.setServerID(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SERVER_PWD);     
    accessConfig.setServerPWD(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_CLIENT_AUTH_TYPE);     
    accessConfig.setClientAuthType(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SERVER_AUTH_TYPE);     
    accessConfig.setServerAuthType(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_IS_SERVER_REQUIRED);     
    accessConfig.setServerAuthRequired((*tmp == 'T') ? TRUE : FALSE);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_MAX_MSG_SIZE);     
    accessConfig.setMaxMsgSize(wcstol(tmp, NULL, 10));
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_MAX_MOD_PER_MSG);     
    accessConfig.setMaxModPerMsg(wcstol(tmp, NULL, 10));
    delete [] tmp;
    
    tmp = n.getPropertyValue(PROPERTY_ENCRYPTION);     
    accessConfig.setEncryption((*tmp == 'T') ? TRUE : FALSE);
    delete [] tmp;

    return TRUE;
}

void DMTClientConfig::saveAccessConfig(ManagementNode& n) {

    wchar_t buf[512];

    n.setPropertyValue(PROPERTY_USERNAME, accessConfig.getUsername());
    n.setPropertyValue(PROPERTY_PASSWORD, accessConfig.getPassword());
    n.setPropertyValue(PROPERTY_DEVICE_ID, accessConfig.getDeviceId());
    n.setPropertyValue(PROPERTY_SYNC_URL, accessConfig.getSyncURL());

    wsprintf(buf, TEXT("%lu"), accessConfig.getFirstTimeSyncMode());
    n.setPropertyValue(PROPERTY_FIRST_TIME_SYNC_MODE, buf);
    
    timestampToAnchor(accessConfig.getBeginSync(), buf);
    n.setPropertyValue(PROPERTY_SYNC_BEGIN, buf);

    timestampToAnchor(accessConfig.getEndSync(), buf);
    n.setPropertyValue(PROPERTY_SYNC_END, buf);

    n.setPropertyValue(PROPERTY_USE_PROXY,
		(accessConfig.getUseProxy() ? TEXT("T"): TEXT("F")) );     
    n.setPropertyValue(PROPERTY_PROXY_HOST, accessConfig.getProxyHost());     
    n.setPropertyValue(PROPERTY_SERVER_NONCE, accessConfig.getServerNonce());
    n.setPropertyValue(PROPERTY_CLIENT_NONCE, accessConfig.getClientNonce());
    n.setPropertyValue(PROPERTY_SERVER_ID, accessConfig.getServerID());
    n.setPropertyValue(PROPERTY_SERVER_PWD, accessConfig.getServerPWD());
    n.setPropertyValue(PROPERTY_CLIENT_AUTH_TYPE, accessConfig.getClientAuthType());
    n.setPropertyValue(PROPERTY_SERVER_AUTH_TYPE, accessConfig.getServerAuthType());
    n.setPropertyValue(PROPERTY_IS_SERVER_REQUIRED,
		(accessConfig.getServerAuthRequired() ? TEXT("T") : TEXT("F") ) ); 

    wcsprintf(buf, TEXT("%lu"), accessConfig.getMaxMsgSize());
    n.setPropertyValue(PROPERTY_MAX_MSG_SIZE, buf);     

    wsprintf(buf, TEXT("%lu"), accessConfig.getMaxModPerMsg());
    n.setPropertyValue(PROPERTY_MAX_MOD_PER_MSG, buf);
    
    n.setPropertyValue(PROPERTY_ENCRYPTION,
		(accessConfig.getEncryption() ? TEXT("T") : TEXT("F") ) ); 
}

BOOL DMTClientConfig::readSourceConfig(int i, ManagementNode& n) {    

    wchar_t *tmp;

    tmp = n.getPropertyValue(PROPERTY_SOURCE_NAME);    
    sourceConfigs[i].setName(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SOURCE_URI);    
    sourceConfigs[i].setURI(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SOURCE_SYNC_MODES);    
    sourceConfigs[i].setSyncModes(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SOURCE_SYNC);    
    sourceConfigs[i].setSync(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SOURCE_TYPE);    
    sourceConfigs[i].setType(tmp);
    delete [] tmp;

    tmp = n.getPropertyValue(PROPERTY_SOURCE_LAST_SYNC);    
    sourceConfigs[i].setLast( ((*tmp) ? wcstol(tmp, NULL, 10) : 0) );
    delete [] tmp;
    
    tmp = n.getPropertyValue(PROPERTY_SOURCE_ENCODING);    
    sourceConfigs[i].setEncoding(tmp);
    delete [] tmp;

    
    return TRUE;
}

void DMTClientConfig::saveSourceConfig(int i, ManagementNode& n) {

    wchar_t buf[512];

    n.setPropertyValue(PROPERTY_SOURCE_NAME, sourceConfigs[i].getName());    
    n.setPropertyValue(PROPERTY_SOURCE_URI, sourceConfigs[i].getURI());
    n.setPropertyValue(PROPERTY_SOURCE_TYPE, sourceConfigs[i].getType());
    n.setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, sourceConfigs[i].getSyncModes());
    n.setPropertyValue(PROPERTY_SOURCE_SYNC, sourceConfigs[i].getSync());    
    n.setPropertyValue(PROPERTY_SOURCE_ENCODING, sourceConfigs[i].getEncoding());    

    timestampToAnchor(sourceConfigs[i].getLast(), buf); 
    n.setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, buf);    

}

