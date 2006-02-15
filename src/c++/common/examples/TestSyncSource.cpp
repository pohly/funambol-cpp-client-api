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

#include "examples/TestSyncSource.h"
#include "base/util/utils.h"
#include "base/Log.h"

static int all      = -1;
static int cnew     = -1;
static int cupdated = -1;
static int cdeleted = -1;
ArrayList items;
ArrayList newItems;
ArrayList updatedItems;
ArrayList deletedItems;


int setAllItems() {
    
    SyncItem item;
    wchar_t name[64];
    char data[128];   
        
    for (int i = 0; i < 4; ++i) {
    
        wsprintf(name, TEXT("%s%d"), TEXT("item"), i);
        sprintf(data, "This is item %d", i);

        item.setKey(name);
        item.setData(data  , (strlen(data))*sizeof(char));
        items.add(item);              
    }           
    return 0;
}

int setAllItemsEmpty() {
    return 0;
}

int setModifiedItems() {
    
    SyncItem item;

    for (int i = 0; i < 4; ++i) {
        
        switch (i) {

            case 0:
                item.setKey(TEXT("item5"));
                item.setData("This is a new item Four"  , 23*sizeof(char));
                newItems.add(item);
                break;
            
            case 1:
                item.setKey(TEXT("item1"));
                item.setData("This is the updated item one"  , 28*sizeof(char));
                updatedItems.add(item);
                break;

            case 2:
                item.setKey(TEXT("item3"));
                item.setData("This is the updated item Three", 30*sizeof(char));
                updatedItems.add(item);
                break;

            case 3:

                item.setKey(TEXT("item4"));                 
                deletedItems.add(item);
                break;

        }        
    }    
        
    return 0;
}

int setModifiedItemsEmpty() {
    return 0;    
}


TestSyncSource::TestSyncSource(const wchar_t* name) : SyncSource(name) {
}

TestSyncSource::~TestSyncSource() {
}

/*
* Return the first SyncItem of all. It is used in case of slow or refresh sync 
* and retrieve the entire data source content.
*/

SyncItem* TestSyncSource::getFirstItem() {
    
    setAllItems();
    if (items.size() == 0) {
        return NULL;
    }    
    all = 0;
    return (SyncItem*)items[all];    
}

/*
* Return the next SyncItem of all. It is used in case of slow or refresh sync 
* and retrieve the entire data source content.
*/

SyncItem* TestSyncSource::getNextItem() {    
    all++;
    if (all == items.size()) {
        return NULL;
    }        
    return (SyncItem*)items.get(all);  
}

SyncItem* TestSyncSource::getFirstNewItem() {
    setModifiedItems();
    if (newItems.size() == 0) {
        return NULL;
    }    
    cnew = 0;
    return (SyncItem*)newItems.get(cnew);    
}

SyncItem* TestSyncSource::getNextNewItem() {    
    cnew++;
    if (cnew == newItems.size()) {
        return NULL;
    }        
    return (SyncItem*)newItems.get(cnew);
}

SyncItem* TestSyncSource::getFirstUpdatedItem() {
   
    if (updatedItems.size() == 0) {
        return NULL;
    }    
    cupdated = 0;
    return (SyncItem*)updatedItems.get(cupdated);    
}

SyncItem* TestSyncSource::getNextUpdatedItem() {    
    cupdated++;
    if (cupdated == updatedItems.size()) {
        return NULL;
    }        
    return (SyncItem*)updatedItems.get(cupdated);
}

SyncItem* TestSyncSource::getFirstDeletedItem() {
   
    if (deletedItems.size() == 0) {
        return NULL;
    }    
    cdeleted = 0;
    return (SyncItem*)deletedItems.get(cdeleted);    
}

SyncItem* TestSyncSource::getNextDeletedItem() {    
    cdeleted++;
    if (cdeleted == deletedItems.size()) {
        return NULL;
    }        
    return (SyncItem*)deletedItems.get(cdeleted);
}

void TestSyncSource::setItemStatus(const wchar_t* key, int status) {
    wchar_t tmp[256];
    wsprintf(tmp, TEXT("key: %s, status: %i"), key, status);
    LOG.debug(tmp);
}

int TestSyncSource::addItem(SyncItem& item) {
    wchar_t tmp[512];
    wsprintf(tmp, TEXT("added item: %s"), item.getKey());
    LOG.info(tmp);
    
    LOG.info(TEXT("Data:"));
    char tmp2[1024];  
    memset(tmp2, 0, 1024);
    strncpy(tmp2, (char*)item.getData(), item.getDataSize());
    wchar_t* tmp3 = toWideChar(tmp2);
    LOG.info(tmp3);
    
    wsprintf(tmp, TEXT("%s-luid"), item.getKey());
    item.setKey(tmp);
    
    delete [] tmp3;
    return 200;
}

int TestSyncSource::updateItem(SyncItem& item) {
    wchar_t tmp[256];
    wsprintf(tmp, TEXT("updated item: %s"), item.getKey());
    LOG.info(tmp);
    
    LOG.info(TEXT("Data:"));
    char tmp2[1024];  
    memset(tmp2, 0, 1024);    
    strncpy(tmp2, (char*)item.getData(), item.getDataSize());
    wchar_t* tmp3 = toWideChar(tmp2);
    LOG.info(tmp3);
    
    delete [] tmp3;
    return 200;
}

int TestSyncSource::deleteItem(SyncItem& item) {
    wchar_t tmp[256];
    wsprintf(tmp, TEXT("deleted item: %s"), item.getKey());
    LOG.debug(tmp);
    return 200;
}

int TestSyncSource::beginSync() {
    wchar_t tmp[256];
    wsprintf(tmp, TEXT("Begin sync TestSyncSource"));
    LOG.debug(tmp);
    return 0;
}
int TestSyncSource::endSync() {
    wchar_t tmp[256];
    wsprintf(tmp, TEXT("End sync TestSyncSource"));
    LOG.debug(tmp);
    return 0;
}

SyncItem* TestSyncSource::getFirstItemKey() {
    return NULL;
}
    
SyncItem* TestSyncSource::getNextItemKey() {
    return NULL;
}
