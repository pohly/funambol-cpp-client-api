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

#ifndef INCL_FILE_SYNC_SOURCE
#define INCL_FILE_SYNC_SOURCE

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSource.h"
#include "base/util/ItemContainer.h"


#define ERR_FILE_SYSTEM             1
#define ERR_NO_FILES_TO_SYNC        2
#define ERR_BAD_FILE_CONTENT        3


class FILESyncSource : public SyncSource {

protected:

    // The path where the application is running.
    char* path;

    // The dir in which the files are and that are to be synced.
    char* dir;

    // The container object that contain all, new, update and deleted items
    ItemContainer* c;
            
    // The copy is protected
    FILESyncSource(SyncSource& s);

    // Retur true if data correctly set.
    bool setItemData(SyncItem* syncItem);

public:
    
    void setPath(const char* p);
    const char* getPath();

    void setDir(const char* p);
    const char* getDir();


    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     */
    FILESyncSource(const wchar_t* name, SyncSourceConfig* sc);

    // Destructor
    virtual ~FILESyncSource();
    
    /*
     * Return the first SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    SyncItem* getFirstItem();

    /*
     * Return the next SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    SyncItem* getNextItem();

    /*
     * Return the first SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstNewItem();

    /*
     * Return the next SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextNewItem();

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstUpdatedItem() ;

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextUpdatedItem();

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstDeletedItem();

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextDeletedItem();
    
    SyncItem* getFirstItemKey();
    SyncItem* getNextItemKey();


    int addItem(SyncItem& item);
    int updateItem(SyncItem& item);
    int deleteItem(SyncItem& item);
   
    void setItemStatus(const wchar_t* key, int status);

    int beginSync();
    int endSync();

    void assign(FILESyncSource& s);
    ArrayElement* clone();
};

#endif
