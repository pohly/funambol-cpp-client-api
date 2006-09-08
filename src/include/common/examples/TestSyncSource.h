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

#ifndef INCL_TEST_SYNC_SOURCE
#define INCL_TEST_SYNC_SOURCE

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSource.h"

class  TestSyncSource : public SyncSource {
       
public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     */
    TestSyncSource(const wchar_t* name, const SyncSourceConfig *sc)  EXTRA_SECTION_01;

    // TestSyncSource
    ~TestSyncSource() EXTRA_SECTION_01;
    
    /*
     * Return the first SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    SyncItem* getFirstItem() EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    SyncItem* getNextItem() EXTRA_SECTION_01;

    /*
     * Return the first SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstNewItem() EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextNewItem() EXTRA_SECTION_01;

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstUpdatedItem()  EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextUpdatedItem() EXTRA_SECTION_01;

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstDeletedItem() EXTRA_SECTION_01;

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    
    SyncItem* getFirstItemKey() EXTRA_SECTION_01;

    /*
     * Return the key of the next SyncItem of all.
     * It is used in case of refresh sync 
     * and retrieve all the keys of the data source.
     */
    SyncItem* getNextItemKey() EXTRA_SECTION_01;


    SyncItem* getNextDeletedItem() EXTRA_SECTION_01;
    
    void setItemStatus(const wchar_t* key, int status) EXTRA_SECTION_01;
    
    int addItem(SyncItem& item) EXTRA_SECTION_01;
   
    int updateItem(SyncItem& item) EXTRA_SECTION_01;

    int deleteItem(SyncItem& item) EXTRA_SECTION_01;
    
    int beginSync() EXTRA_SECTION_01;
    
    int endSync() EXTRA_SECTION_01;

    ArrayElement* clone() { return 0; }
};

#endif
