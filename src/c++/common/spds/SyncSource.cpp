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

#include "spds/SyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "client/SyncClient.h"
#include "spds/spdsutils.h"

SyncSource::SyncSource(const WCHAR* sourceName, const SyncSourceConfig *sc) {
    name = NULL;

    if (sc) {
        config.assign(*sc);
        setPreferredSyncMode(syncModeCode(sc->getSync()));
    }
    else {
        syncMode = SYNC_NONE;
    }

    if ((sourceName == NULL) || (*sourceName == 0)) {
        lastErrorCode = ERR_PARAMETER_IS_EMPTY;
        sprintf(lastErrorMsg, T("name cannot be empty (NULL or 0-length)"));
        goto finally;
    }
    name = wstrdup(sourceName);

  finally:

    lastSync = 0;
	nextSync = 0;
    
    //errorHandler = NULL;
    filter       = NULL;
}

/**
 * Release dynamically allocated resources
 */
SyncSource::~SyncSource() {
    if (name) {
        delete [] name;
    }
    if (filter) {
       delete filter;
    }
}


/*
 * Returns the source name.
 */
const WCHAR *SyncSource::getName() {
     return name;
 }

void SyncSource::setConfig(const SyncSourceConfig& sc) {
    config.assign(sc);
    setPreferredSyncMode(syncModeCode(sc.getSync()));
}

/*
 * Sets the synchronization mode required for the
 * SyncSource.
 *
 * @param syncMode - sync synchronization mode
 */
void SyncSource::setPreferredSyncMode(SyncMode sourceSyncMode) {
    syncMode = sourceSyncMode;
}

/*
 * Returns the preferred synchronization mode for the SyncSource
 */
SyncMode SyncSource::getPreferredSyncMode() {
    return syncMode;
}

/*
 * Sets the server imposed synchronization mode for the SyncSource.
 *
 * @param syncMode - sync synchronization mode
 */
void SyncSource::setSyncMode(SyncMode mode) {
    syncMode = mode;
}

/*
 * Returns the synchronization mode.
 */
SyncMode SyncSource::getSyncMode() {
    return syncMode;
}

int SyncSource::beginSync() {
    return 0;
}

int SyncSource::endSync() {
    return 0;
}

/*
 * Returns the timestamp in milliseconds of the last synchronization.
 * The reference time of the timestamp is platform specific.
 */
unsigned long SyncSource::getLastSync() {
    return lastSync;
}

/*
 * Sets the timestamp in millisencods of the last synchronization.
 * The reference time of the timestamp is platform specific.
 */
void SyncSource::setLastSync(unsigned long timestamp) {
    lastSync = timestamp;
}

/*
 * Returns the timestamp in milliseconds of the next synchronization.
 * The reference time of the timestamp is platform specific.
 */
unsigned long SyncSource::getNextSync() {
    return nextSync;
}

/*
 * Sets the timestamp in millisencods of the next synchronization.
 * The reference time of the timestamp is platform specific.
 */
void SyncSource::setNextSync(unsigned long timestamp) {
    nextSync = timestamp;
}

/*
 * Sets the last anchor associated to the source
 *
 * @param lastAnchor last anchor
 */
void SyncSource::setLastAnchor(const char* lastAnchor) {
    strncpy(last, (lastAnchor != NULL) ? lastAnchor : T(""), DIM_ANCHOR);
    next[DIM_ANCHOR-1] = 0;
}

/*
 * Gets the last anchor associated to the source. If last is NULL the
 * internal buffer address is returned, otherwise the value is copied
 * in the given buffer and the buffer address is returned.
 */
const char* SyncSource::getLastAnchor() {
    return last;
}

/*
 * Sets the next anchor associated to the source
 *
 * @param next next anchor
 */
void SyncSource::setNextAnchor(const char* nextAnchor) {
    strncpy(next, (nextAnchor != NULL) ? nextAnchor : T(""), DIM_ANCHOR);
    next[DIM_ANCHOR-1] = 0;
}

/*
 * Gets the next anchor associated to the source. If last is NULL the
 * internal buffer address is returned, otherwise the value is copied
 * in the given buffer and the buffer address is returned.
 */
const char* SyncSource::getNextAnchor() {
    return next;
}


/*
 * Gets filter
 *
 * @return  the current filter's value
 *
 */
SourceFilter* SyncSource::getFilter() {
    return filter;
}

/*
 * Sets filter
 *
 * @param filter the new value
 *
 */
void SyncSource::setFilter(SourceFilter* f) {
    if (filter) {
        delete filter; filter = NULL;
    }

    if (f) {
        filter = new SourceFilter();
        filter->setInclusive(f->isInclusive());
        filter->setClause(f->getClause());
    }
}
