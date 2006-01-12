/*
 * Copyright (C) 2005-2006 Funambol
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

#ifndef INCL_SYNC_SOURCE
#define INCL_SYNC_SOURCE

#include "base/fscapi.h"
#include "base/ErrorHandler.h"
#include "base/util/ArrayElement.h"
#include "filter/SourceFilter.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncStatus.h"

class SyncSource : public ArrayElement {

private:
    SyncMode      syncMode;
    unsigned long lastSync;
    unsigned long nextSync;
    wchar_t*      name;
    wchar_t*      remoteURI;

    wchar_t type[DIM_MIME_TYPE];
    wchar_t next[DIM_ANCHOR];
    wchar_t last[DIM_ANCHOR];

    ErrorHandler* errorHandler;

    SourceFilter* filter;

public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     */
    SyncSource(const wchar_t* name) EXTRA_SECTION_01;

    // Destructor
    virtual ~SyncSource() EXTRA_SECTION_01;

    /**
     * Get the source name.
     *
     * @return - the source name (a pointer to the object buffer,
     *           will be released at object destruction)
     *
     */
    const wchar_t *getName() EXTRA_SECTION_01;

    /*
     * Get & Set remoteURI attribute.
     */
    void setRemoteURI(const wchar_t* uri) EXTRA_SECTION_01;
    const wchar_t* getRemoteURI() EXTRA_SECTION_01;

    /*
     * Get & Set the preferred synchronization mode for the SyncSource.
     */
    SyncMode getPreferredSyncMode() EXTRA_SECTION_01;
    void setPreferredSyncMode(SyncMode syncMode) EXTRA_SECTION_01;

    /*
     * Get & Sets the server imposed synchronization mode for the SyncSource.
     */
    SyncMode getSyncMode() EXTRA_SECTION_01;
    void setSyncMode(SyncMode syncMode) EXTRA_SECTION_01;

    /*
     * Sets the mime type standard for the source items
     *
     * @param type the mime type
     */
    void setType(const wchar_t* type) EXTRA_SECTION_01;
    /*
     * Gets the standard mime type for the source items.
     *
     * @return - the source name (a pointer to the object buffer,
     *           will be released at object destruction)
     */
    const wchar_t* getType() EXTRA_SECTION_01;

    /*
     * Get & Set the timestamp in milliseconds of the last synchronization.
     * The reference time of the timestamp is platform specific.
     */
    unsigned long getLastSync() EXTRA_SECTION_01;
    void setLastSync(unsigned long timestamp) EXTRA_SECTION_01;

    /*
     * Gets & Sets the timestamp in milliseconds of the next synchronization.
     * The reference time of the timestamp is platform specific.
     */
    unsigned long getNextSync() EXTRA_SECTION_01;
    void setNextSync(unsigned long timestamp) EXTRA_SECTION_01;

    /*
     * Gets & Sets the last anchor associated to the source
     */
    void setLastAnchor(const wchar_t* last) EXTRA_SECTION_01;
    const wchar_t* getLastAnchor() EXTRA_SECTION_01;

    /*
     * Gets & Sets the next anchor associated to the source
     */
    const wchar_t* getNextAnchor() EXTRA_SECTION_01;
    void setNextAnchor(const wchar_t* next) EXTRA_SECTION_01;

    /*
    * Gets filter
    *
    * @return  the current filter's value
    *
    */
    SourceFilter* getFilter();

    /*
    * Sets filter
    *
    * @param filter the new value
    *
    */
    void setFilter(SourceFilter* f);


    /*
     * Called by the engine at the begin of the sync.
     * The SyncSource can do every initialization it needs.
     * The synchronization stops if this function return a non-zero value.
     * 
     * @return - 0 on success, an error otherwise
     */
    virtual int beginSync() = 0 EXTRA_SECTION_01;
    
    /*
     * Called by the engine at the begin of the sync.
     * The SyncSource can do any needed commit action to save the state of
     * the items.
     * The engine commits to the server the changes applied in the transaction 
     * only if this function return 0
     * 
     * @return - 0 on success, an error otherwise
     */
    virtual int endSync() = 0 EXTRA_SECTION_01;
        
    virtual void setItemStatus(const wchar_t* key, int status) = 0 EXTRA_SECTION_01;
    /*
     * Return the first SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    virtual SyncItem* getFirstItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    virtual SyncItem* getNextItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the first SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    virtual SyncItem* getFirstNewItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    virtual SyncItem* getNextNewItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    virtual SyncItem* getFirstUpdatedItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    virtual SyncItem* getNextUpdatedItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    virtual SyncItem* getFirstDeletedItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */

    virtual SyncItem* getNextDeletedItem() = 0 EXTRA_SECTION_01;

    
    virtual int addItem(SyncItem& item) = 0 EXTRA_SECTION_01;
   
    virtual int updateItem(SyncItem& item) = 0 EXTRA_SECTION_01;

    virtual int deleteItem(SyncItem& item) = 0 EXTRA_SECTION_01;

    /*
     * Gets the Error Handler for the SyncSource, if the attribute is set,
     * otherwise return the one of Sync4JClient as a default (using a static
     * method call).
     * 
     * @return  A reference to the ErrorHandler to be used by the SyncSource.
     */
    virtual ErrorHandler& getErrorHandler() EXTRA_SECTION_01;
    
    /*
     * Sets the Error Handler for the SyncSource
     * 
     * @param e: a reference to the ErrorHandler to be used by the SyncSource.
     */
    virtual void setErrorHandler(ErrorHandler& e) EXTRA_SECTION_01;

    /**
     * ArrayElement implementation
     */
    virtual ArrayElement* clone() EXTRA_SECTION_01 = 0;
                                         
};

#endif
