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
#include "spds/SyncSourceConfig.h"
#include "spds/SyncSourceReport.h"

class SyncSource : public ArrayElement {

private:
    SyncMode      syncMode;
    unsigned long lastSync;
    unsigned long nextSync;
    WCHAR*      name;

    char next[DIM_ANCHOR];
    char last[DIM_ANCHOR];

    //ErrorHandler* errorHandler;

    SourceFilter* filter;

protected:
    SyncSourceConfig& config;
    SyncSourceReport* report;

public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name   the name of the SyncSource
     * @param sc     configuration for the sync source: the instance
     *               must remain valid throughout the lifetime of the
     *               sync source because it keeps a reference to it
     *               and uses it as its own. A NULL pointer is allowed
     *               for unit testing outside of the sync framework;
     *               the sync source then references a global config
     *               instance to avoid crashes, but modifying that config
     *               will not make much sense.
     */
    SyncSource(const WCHAR* name, SyncSourceConfig* sc) EXTRA_SECTION_01;

    // Destructor
    virtual ~SyncSource() EXTRA_SECTION_01;

    /**
     * Get the source name.
     *
     * @return - the source name (a pointer to the object buffer,
     *           will be released at object destruction)
     *
     */
    const WCHAR *getName() EXTRA_SECTION_01;

    /**********************************************************
     * Most of the configurable properties are read
     * by the client library from the config (in
     * SyncClient::setDMConfig()) and then copied into the
     * sync source.
     *
     * These properties are stored in a local copy which will not be
     * written back into the permanent config, with a few exceptions:
     * properties related to mananging sync sessions like lastAnchor
     * are written back into the config by the library afer a
     * successful synchronization.
     *
     * A client developer is not required to modify these calls,
     * but he can use and/or update the properties before the
     * synchronization starts.
     *********************************************************/

    // read-only access to configuration
    const SyncSourceConfig& getConfig() const EXTRA_SECTION_01 {
        return config;
    }
    // read-write access to configuration
    SyncSourceConfig& getConfig() EXTRA_SECTION_01 {
        return config;
    }
    // initialize sync source from complete configuration:
    // copies all settings into the config referenced by the
    // sync source
    void setConfig(const SyncSourceConfig& sc) EXTRA_SECTION_01;


    // Return pointer to report object
    SyncSourceReport* getReport() EXTRA_SECTION_01;

    // Set the report pointer with the given one 
    // (no copy, only assign the pointer to the external one)
    void setReport(SyncSourceReport* sr) EXTRA_SECTION_01;

    /*
     * Get & Set the preferred synchronization mode for the SyncSource.
     *
     * Taken initially from the configuration by setConfig(), it can then
     * be modified by the client. The code synchronization code itself
     * reads this value, but it doesn't modify it.
     */
    SyncMode getPreferredSyncMode() EXTRA_SECTION_01;
    void setPreferredSyncMode(SyncMode syncMode) EXTRA_SECTION_01;

    /*
     * Get & Sets the server imposed synchronization mode for the SyncSource.
     *
     * Agreed upon with the server during the initial exchange with the server.
     * The SyncSource can react to it in beginSync(), in particular it must wipe
     * its local data during a refresh from server.
     */
    SyncMode getSyncMode() EXTRA_SECTION_01;
    void setSyncMode(SyncMode syncMode) EXTRA_SECTION_01;

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
    void setLastAnchor(const char*  last) EXTRA_SECTION_01;
    const char*  getLastAnchor() EXTRA_SECTION_01;

    /*
     * Gets & Sets the next anchor associated to the source
     */
    const char*  getNextAnchor() EXTRA_SECTION_01;
    void setNextAnchor(const char*  next) EXTRA_SECTION_01;
    
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

    /******************************************************
     * The following methods are virtual because a
     * derived SyncSource is expected to override or
     * implement them. Only the pure virtual methods
     * really have to be implemented, the others have
     * reasonable defaults.
     *****************************************************/
    
    /*
     * Called by the engine from inside SyncClient::sync()
     * at the begin of the sync.
     *
     * The SyncSource can do every initialization it needs.
     * The server has been contacted, so in particular
     * getSyncMode() can now be used to find out what
     * the sync mode for this synchronization run will be.
     * After this call the iterators for SyncItems must return
     * valid results for the current sync mode.
     *
     * The synchronization stops if this function return a non-zero value.
     * 
     * @return - 0 on success, an error otherwise
     */
    virtual int beginSync() EXTRA_SECTION_01;
    
    /*
     * Called by the engine from inside SyncClient::sync()
     * at the end of the sync.
     *
     * The SyncSource can do any needed commit action to save
     * the state of the items. The engine commits to the server
     * the changes applied in the transaction only if this function
     * return 0.
     *
     * FIXME: the return code is currently ignored by the sync engine
     * 
     * @return - 0 on success, an error otherwise
     */
    virtual int endSync() EXTRA_SECTION_01;
        
    virtual void setItemStatus(const WCHAR* key, int status) = 0 EXTRA_SECTION_01;

    /*
     * Return the key of the first SyncItem of all.
     * It is used in case of refresh sync 
     * and retrieve all the keys of the data source.
     */
    virtual SyncItem* getFirstItemKey() = 0 EXTRA_SECTION_01;

    /*
     * Return the key of the next SyncItem of all.
     * It is used in case of refresh sync 
     * and retrieve all the keys of the data source.
     */
    virtual SyncItem* getNextItemKey() = 0 EXTRA_SECTION_01;

    /*
     * Return the first SyncItem of all.
     * It is used in case of slow sync
     * and retrieve the entire data source content.
     */
    virtual SyncItem* getFirstItem() = 0 EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of all.
     * It is used in case of slow sync
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
    //virtual ErrorHandler& getErrorHandler() EXTRA_SECTION_01;
    
    /*
     * Sets the Error Handler for the SyncSource
     * 
     * @param e: a reference to the ErrorHandler to be used by the SyncSource.
     */
    //virtual void setErrorHandler(ErrorHandler& e) EXTRA_SECTION_01;

    /**
     * ArrayElement implementation
     */
    virtual ArrayElement* clone() EXTRA_SECTION_01 = 0;

};

#endif
