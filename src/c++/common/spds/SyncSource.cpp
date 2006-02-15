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
#include "client/Sync4jClient.h"

SyncSource::SyncSource(const wchar_t* sourceName) {
    if ((sourceName == NULL) || (*sourceName == 0)) {
        lastErrorCode = ERR_PARAMETER_IS_EMPTY;
        wcsprintf(lastErrorMsg, TEXT("name cannot be empty (NULL or 0-length)"));
        goto finally;
    }

    name = new wchar_t[wcslen(sourceName)+1];
    wcscpy(name, sourceName);
    remoteURI = new wchar_t[wcslen(sourceName)+1];
    wcscpy(remoteURI, sourceName);

    *type = *next = *last = 0;
    finally:

    syncMode = SYNC_NONE;
    lastSync = 0;
	nextSync = 0;
    
    errorHandler = NULL;
    filter       = NULL;
}

/**
 * Release dynamically allocated resources
 */
SyncSource::~SyncSource() {
    if (name) {
        delete [] name;
    }

    if (remoteURI) {
        delete [] remoteURI;
    }

    if (filter) {
       delete filter;
    }
}

/*
 * Gets the Error Handler for the SyncSource, if the attribute is set,
 * otherwise return the one of Sync4JClient as a default (using a static
 * method call).
 * 
 * @return  A reference to the ErrorHandler to be used by the SyncSource.
 */
ErrorHandler& SyncSource::getErrorHandler() {
    if (errorHandler) 
        return *errorHandler;
    
    return Sync4jClient::getInstance().getErrorHandler();
}

/*
 * Sets the Error Handler for the SyncSource
 * 
 * @param e: a reference to the ErrorHandler to be used by the SyncSource.
 */
void SyncSource::setErrorHandler(ErrorHandler& e) {
    errorHandler = &e;
}

/*
 * Returns the source name. If sourceName is <> NULL, the source name is copied
 * in it. If sourceName is <> NULL the returned value is sourceName. Otherwise,
 * the returned value is the internal buffer pointer. Note that this will be
 * released at object automatic destruction.
 *
 * @param sourceName - the buffer where the name will be copied (if != NULL) - NULL
 * @param dim - buffer size
 *
 */
const  wchar_t *SyncSource::getName() {
     return name;
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
 * Sets the synchronization mode required for the
 * SyncSource.
 *
 * @param syncMode - sync synchronization mode
 */
void SyncSource::setRemoteURI(const wchar_t* uri) {
    if (remoteURI) {
        delete [] remoteURI;
    }

    if (uri) {
        remoteURI = new wchar_t[wcslen(uri)+1];
        wcscpy(remoteURI, uri);
    } else {
        wcscpy(remoteURI, TEXT(""));
    }
}

/*
 * Returns the preferred synchronization mode for the SyncSource
 */
const wchar_t* SyncSource::getRemoteURI() {   
    return remoteURI;   
}


/*
 * Sets the encoding parameter
 *
 * @param ecnc - encoding
 */
void SyncSource::setEncoding(const wchar_t* enc) {
    if (encoding) {
        delete [] encoding;
    }

    if (enc) {
        encoding = new wchar_t[wcslen(enc)+1];
        wcscpy(encoding, enc);
    } else {
        encoding = stringdup(TEXT(""));
    }
}

/*
 * Returns the preferred synchronization mode for the SyncSource
 */
const wchar_t* SyncSource::getEncoding() {   
    return encoding;   
}


/*
 * Sets the mime type standard for the source items
 *
 * @param mimeType the mime type
 */
void SyncSource::setType(const wchar_t* mimeType) {
    wcsncpy(type, (mimeType == NULL) ? TEXT("") : mimeType, DIM_MIME_TYPE);
    type[DIM_MIME_TYPE-1] = 0;
}

/*
 * Returns the items data mime type. If type is NULL, the pointer to the
 * internal buffer is returned, otherwise the value is copied in the
 * given buffer, which is also returned to the caller.
 *
 * @param mimeType the buffer where to copy the mime type value
 */
const wchar_t* SyncSource::getType() {
        return type;
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

/*
 * Ends the synchronization of the specified source.
 *
 * @param source - the SyncSource to sync
 
int SyncSource::endSync(SyncSource& source) {
    //
    // TBD
    //
    return 0;
}
*/

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
void SyncSource::setLastAnchor(const wchar_t* lastAnchor) {
    wcsncpy(last, (lastAnchor != NULL) ? lastAnchor : TEXT(""), DIM_ANCHOR);
}

/*
 * Gets the last anchor associated to the source. If last is NULL the
 * internal buffer address is returned, otherwise the value is copied
 * in the given buffer and the buffer address is returned.
 */
const wchar_t* SyncSource::getLastAnchor() {
    return last;
}

/*
 * Sets the next anchor associated to the source
 *
 * @param next next anchor
 */
void SyncSource::setNextAnchor(const wchar_t* nextAnchor) {
    wcsncpy(next, (nextAnchor != NULL) ? nextAnchor : TEXT(""), DIM_ANCHOR);
    next[DIM_ANCHOR-1] = 0;
}

/*
 * Gets the next anchor associated to the source. If last is NULL the
 * internal buffer address is returned, otherwise the value is copied
 * in the given buffer and the buffer address is returned.
 */
const wchar_t* SyncSource::getNextAnchor() {
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
