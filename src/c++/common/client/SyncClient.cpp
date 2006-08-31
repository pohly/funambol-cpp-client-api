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


#include "client/SyncClient.h"
#include "spds/SyncManager.h"
#include "spds/spdsutils.h"
#include "base/LogErrorHandler.h"
#include "syncml/core/CTTypeSupported.h"
#include "syncml/core/CTCap.h"
#include "syncml/core/CTPropParam.h"

//--------------------------------------------------- Constructor & Destructor
SyncClient::SyncClient(SyncManagerConfig& c) : config(c) {
}

SyncClient::~SyncClient() {
}

//------------------------------------------------------------- Public Methods

/*
 * Set config from user defined Config
 */
void SyncClient::setConfig(SyncManagerConfig& c) {
    config = c;
}


/*
* Used to start the sync process. The argument is an array of SyncSources
* that have to be synched with the sync process
*/
int SyncClient::sync(SyncSource** sources) {

    resetError();
    int ret = 0;

    SyncManager syncManager(config);

    if ((ret = syncManager.prepareSync(sources))) {
        BCHAR dbg[256];
        bsprintf(dbg, T("Error in preparing sync: %s"), lastErrorMsg);//
        LOG.error(dbg);
        
        goto finally;
    }
       
    if ((ret = syncManager.sync())) {   
        BCHAR dbg[256];
        bsprintf(dbg, T("Error in syncing: %s"), lastErrorMsg);
        LOG.error(dbg);
        goto finally;
    }

    if ((ret = syncManager.endSync())) {       
        BCHAR dbg[256];
        bsprintf(dbg, T("Error in ending sync: %s"), lastErrorMsg);
        LOG.error(dbg);
        goto finally;
    }
       
  finally:

    return ret;
}


/*
 * Start the sync process.
 * SyncSources are managed (created, initialized, deleted) inside 
 * this method. When SyncSource array is ready, the method 'sync(sources**)'
 * is called to start the sync process.
 *
 * @param sourceNames: optional, a NULL terminated array of source names that
 *                     we want to sync. If NULL, sources to sync are chosen
 *                     from the configuration object (config).
 * @return:            0 on success, an error otherwise
 */
int SyncClient::sync(BCHAR** sourceNames) {
    SyncSource **sources = NULL;
    const BCHAR* currName;
    int currSource = 0, numActive = 0, numSources = 0;
    int ret = 0;

    ret = prepareSync(config);
    if (ret) {
        LOG.error("SyncClient: prepareSync returned error code: %d.", ret);
        goto finally;
    }

    // Get number of sources: from passed parameter or (if NULL) from config.
    numSources = 0;
    if (sourceNames) {
        while (sourceNames[numSources]) {
            numSources ++;
        }
    }
    else {
        numSources = config.getSyncSourceConfigsCount();
    }

    // make room for all potential sync sources
    sources = new SyncSource* [numSources + 1];

    // iterate over all configs and add those which the client
    // wants to have synchronized
    while (currSource < numSources) {
        
        SyncSourceConfig sc;
        // use only sources indicated in 'sourceNames' param
        if (sourceNames) {
            currName = sourceNames[currSource];
            if (!config.getSyncSourceConfig(currName, sc)) {
                ret = lastErrorCode;
                goto finally;
            }
        }
        // use all available sources from config
        else {
            if (!config.getSyncSourceConfig(currSource, sc)) {
                ret = lastErrorCode;
                goto finally;
            }
            currName = sc.getName();
        }

        ret = createSyncSource(currName, currSource, sc, sources + numActive);
        if (ret) {
            LOG.error("SyncClient: createSyncSource returned error code: %d.", ret);
            goto finally;
        }

        if (sources[numActive]) {
            numActive++;
        }
        currSource++;
    }
    sources[numActive] = NULL;


    ret = beginSync(sources);
    if (ret) {
        LOG.error("SyncClient: beginSync returned error code: %d.", ret);
        goto finally;
    }

    //
    // ready to synchronize
    //
    ret = sync(sources);
    if (ret) {
        goto finally;
    }

    ret = endSync(sources);
    if (ret) {
        LOG.error("SyncClient: endSync returned error code: %d.", ret);
    }

finally:
    if (sources) {
        for (int i=0; sources[i]; i++) {
            delete sources[i];
        }
        delete [] sources;
    }

    return ret;
}

