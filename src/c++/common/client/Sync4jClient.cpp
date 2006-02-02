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


#include "client/Sync4jClient.h"
#include "base/LogErrorHandler.h"
#include "spds/SyncManager.h"

//----------------------------------------------------------- Static variables
static DMTClientConfig *defaultConfig = 0;
static LogErrorHandler *defaultErrorHandler = 0;

Sync4jClient* Sync4jClient::instance = 0;

//--------------------------------------------------- Constructor & Destructor
Sync4jClient::Sync4jClient() {
    syncManager  = NULL;
    defaultErrorHandler = new LogErrorHandler();
    errorHandler = defaultErrorHandler;
}

Sync4jClient::~Sync4jClient() {
    if(syncManager)
        delete syncManager;
    if(defaultErrorHandler)
        delete defaultErrorHandler;
}

//------------------------------------------------------------- Public Methods

/*
 * Get, or create, Sync4jClient instance 
 */
Sync4jClient& Sync4jClient::getInstance() {
    
        LOG.reset();  
        LOG.setLevel(LOG_LEVEL_DEBUG);            

    if (instance == NULL) {
        instance = new Sync4jClient();
    }
    return *instance;
}

void Sync4jClient::dispose() {
    if (instance) {
        delete instance;         
    }        
    instance = NULL;
}

/*
 * Set config from user defined Config
 */
void Sync4jClient::setDMConfig(DMTClientConfig& config) {
    if (syncManager)
        delete syncManager;

    if (defaultConfig) {
        delete defaultConfig; 
        defaultConfig = NULL;
    }
    syncManager = new SyncManager(config);
}

/*
 * Set config from DM using an application URI
 */
void Sync4jClient::setDMConfig(const wchar_t *root) {
    if (defaultConfig)
        delete defaultConfig;
    if (syncManager)
        delete syncManager;

    defaultConfig = new DMTClientConfig(root);
    defaultConfig->read();    
    syncManager = new SyncManager(*defaultConfig);

}

ErrorHandler& Sync4jClient::getErrorHandler() {
    return *errorHandler;
}

void Sync4jClient::setErrorHandler(ErrorHandler& e) {
    if (defaultErrorHandler) {
        delete defaultErrorHandler; defaultErrorHandler = NULL;
    }    
    errorHandler = &e;
}


/*
* Used to start the sync process. The argument is an array of SyncSources
* that have to be synched with the sync process
*/
int Sync4jClient::sync(SyncSource** sources) {

    resetError();
    int ret = 0;
    if (!syncManager)
        return -1;

    if ((ret = syncManager->prepareSync(sources))) {
        wchar_t dbg[256];
        wsprintf(dbg, TEXT("ERROR prepareSync: %s"), lastErrorMsg);
        LOG.error(dbg);
        
        goto finally;
    }
       
    if ((ret = syncManager->sync())) {   
        wchar_t dbg[256];
        wsprintf(dbg, TEXT("ERROR sync: %s"), lastErrorMsg);
        LOG.error(dbg);
        goto finally;
    }

    if ((ret = syncManager->endSync())) {       
        wchar_t dbg[256];
        wsprintf(dbg, TEXT("ERROR endSync: %s"), lastErrorMsg);
        LOG.error(dbg);
        goto finally;
    }

    if (defaultConfig) {
        defaultConfig->save();
    }
       
finally:    

//    if (syncManager != NULL) {
//        delete syncManager;
//    }
   
    return ret;
}


#if 0
/*
 * Used to start the sync process. The argument is an array of SyncSource
 * that have to be synched
 *
 * @param sources the SyncSource& array
 */
int Sync4jClient::sync(ArrayList& sources) {
    const int ml = 10;
    int l = sources.size();

    SyncSource* array[ml];
    
    for (int i=0; (i<l) && (i<ml-1); ++i) {
        array[i] = (SyncSource*)sources[i];
    }

    array[l] = NULL;

    return sync(array);
}

#endif
