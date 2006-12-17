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
#include "spdm/ManagementNode.h"
#include "base/util/ItemContainer.h"


#define ERR_FILE_SYSTEM             1
#define ERR_NO_FILES_TO_SYNC        2
#define ERR_BAD_FILE_CONTENT        3


class FILESyncSource : public SyncSource {

protected:

    // The dir in which the files are and that are to be synced.
    char* dir;

    // The copy is protected
    FILESyncSource(SyncSource& s);

    // Return true if data correctly set: syncItem->getKey() contains
    // the file name relative to dir, copying its content into
    // the items data can be overriden by derived classes.
    virtual bool setItemData(SyncItem* syncItem);

    /**
     * must be called for each successfully added item
     *
     * @param item     the added item
     * @param key      the key of that item
     * @return SyncML status code, STC_ITEM_ADDED on success
     */
    int addedItem(SyncItem& item, const WCHAR* key);

public:
    
    void setDir(const char* p);
    const char* getDir();


    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     */
    FILESyncSource(const WCHAR* name, SyncSourceConfig* sc);

    // Destructor
    virtual ~FILESyncSource();
    
    /*
     * Return the first SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    SyncItem* getFirstItem() { return getFirst(allItems); }

    /*
     * Return the next SyncItem of all.
     * It is used in case of slow or refresh sync 
     * and retrieve the entire data source content.
     */
    SyncItem* getNextItem() { return getNext(allItems); }

    /*
     * Return the first SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstNewItem() { return getFirst(newItems); }

    /*
     * Return the next SyncItem of new one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextNewItem() { return getNext(newItems); }

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstUpdatedItem() { return getFirst(updatedItems); }

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextUpdatedItem() { return getNext(updatedItems); }

    /*
     * Return the first SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getFirstDeletedItem() { return getFirst(deletedItems, FALSE); }

    /*
     * Return the next SyncItem of updated one. It is used in case of fast sync 
     * and retrieve the new data source content.
     */
    SyncItem* getNextDeletedItem() { return getNext(deletedItems, FALSE); }
    
    SyncItem* getFirstItemKey() { return getFirst(allItems, FALSE); }
    SyncItem* getNextItemKey() { return getNext(allItems, FALSE); }


    int addItem(SyncItem& item);
    int updateItem(SyncItem& item);
    int deleteItem(SyncItem& item);
   
    void setItemStatus(const WCHAR* key, int status);

    int beginSync();
    int endSync();

    void assign(FILESyncSource& s);
    ArrayElement* clone();

    /**
     * Tracking changes requires persistent storage: for each item sent
     * to the server a property is set to the item's modification time.
     *
     * The caller is responsible for storing these properties after
     * a successful sync and continues to own the node instance itself.
     *
     * During the next beginSync() the information will be used to
     * identify added, updated and deleted items.
     */
    void setFileNode(ManagementNode *mn) { fileNode = mn; }
    ManagementNode *getFileNode() { return fileNode; }

  private:
    // Lists of all, new, update and deleted items
    // together with the current index.
    struct ItemIteratorContainer {
        ArrayList items;
        int index;
    } allItems, newItems, updatedItems, deletedItems;

    // an optional node in which file dates are stored to track changes
    ManagementNode* fileNode;

    /** returns time stored in fileNode for the given key, 0 if not found */
    unsigned long getServerModTime(const char* keystr);
            
    SyncItem* getFirst(ItemIteratorContainer& container, BOOL getData = TRUE);
    SyncItem* getNext(ItemIteratorContainer& container, BOOL getData = TRUE);
};

#endif
