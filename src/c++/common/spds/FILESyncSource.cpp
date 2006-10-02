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
#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "spds/FileData.h"
#include <direct.h>

#include "spds/FILESyncSource.h"


static int call;
static int cnew;
static int cupdated;
static int cdeleted;


FILESyncSource::FILESyncSource(const wchar_t* name, const SyncSourceConfig* sc) : SyncSource(name, sc) {
    c    = NULL; 
    path = NULL;
    dir  = NULL;

    char* tmp = _getcwd(NULL, 0);
    setPath(tmp);
    delete [] tmp;

    setDir(".");
}

FILESyncSource::~FILESyncSource() {
    if (c) {
        delete c; 
        c = NULL;
    }
    if (path) {
        delete [] path;
        path = NULL;
    }
    if(dir) {
        delete [] dir;
        dir = NULL;
    }
}



void FILESyncSource::setPath(const char* p) {
    if (path)
        delete [] path;

    path = (p) ? stringdup(p) : stringdup("\\");
}

const char* FILESyncSource::getPath() {
    return path;
}

void FILESyncSource::setDir(const char* p) {
    if (dir)
        delete [] dir;

    dir = (p) ? stringdup(p) : stringdup("\\");
}

const char* FILESyncSource::getDir() {
    return dir;
}



/////////////////////////////////////////////////////////////////////////////////////////


int FILESyncSource::beginSync() {

    //
    // Get file list.
    //
    int count;
    char** fileNames = readDir(dir, &count);
    if (!fileNames || count==0) {
        LOG.debug("No files in dir '%s'", dir);
        return 0;
    }
    LOG.info("The client number of files to sync are %i", count);

    if (c == NULL) {
        c = new ItemContainer();
    }

    //
    // Create ArrayList ALL (empty data) from file names.
    //
    for (int i=0; i<count; i++) {
        if (fileNames[i]) {
            WCHAR* wname = toWideChar(fileNames[i]);
            SyncItem* s = new SyncItem(wname);
            c->addItemToAllItems(s);
            delete s;
            delete [] wname;
            delete [] fileNames[i];
        }
    }
    if (fileNames) {
        delete [] fileNames;  
        fileNames = NULL;
    }
    return 0;    
}


/*
* Return the first SyncItem of all. It is used in case of slow or refresh sync 
* and retrieve the entire data source content.
*/
SyncItem* FILESyncSource::getFirstItem() {
    if (c == NULL) {
        return NULL;
    }
    if (c->getAllItemsSize() == 0) {
        return NULL;
    }  

    call = 0;
    SyncItem* syncItem = (SyncItem*)c->getAllItems()->get(call)->clone();

    //
    // Set data from file content, return syncItem (freed by API)
    //
    if (setItemData(syncItem)){
        return syncItem;
    }
    else {
        delete syncItem;
        return NULL;
    }
}


/*
* Return the next SyncItem of all. It is used in case of slow or refresh sync 
* and retrieve the entire data source content.
*/
SyncItem* FILESyncSource::getNextItem() {
    if (c == NULL) {
        return NULL;
    }
    if (c->getAllItemsSize() == 0) {
        return NULL;
    }

    call++;
    if (call >= c->getAllItemsSize()) {
        return NULL;
    }
    SyncItem* syncItem = (SyncItem*)c->getAllItems()->get(call)->clone();

    // Set data from file content, return syncItem (freed by API)
    if (setItemData(syncItem)){
        return syncItem;
    }
    else {
        delete syncItem;
        return NULL;
    }
}



SyncItem* FILESyncSource::getFirstItemKey() {
    return NULL;
}
SyncItem* FILESyncSource::getNextItemKey() {
    return NULL;
}


SyncItem* FILESyncSource::getFirstNewItem() {
    ///// TBD //////
    return NULL;
    ////////////////

    if (c == NULL) {
        c = new ItemContainer();
    }

    //
    // get new/mod/del list -> c
    //
   
     //Log the number of item to sync from client
    LOG.info("The client number of new files to sync are %i", c->getNewItemsSize());
    LOG.info("The client number of updated files to sync are %i", c->getUpdatedItemsSize());
    LOG.info("The client number of deleted files to sync are %i", c->getDeletedItemsSize());

   
    if (c->getNewItemsSize() == 0) {
        return NULL;
    }    
    cnew = 0;
    ArrayList* list = c->getNewItems(); 
	return (SyncItem*)list->get(cnew)->clone();    
}

SyncItem* FILESyncSource::getNextNewItem() { 
    ///// TBD //////
    return NULL;
    ////////////////

    if (c == NULL)
        return NULL;

    cnew++;
    if (cnew >= c->getNewItemsSize()) {
        return NULL;
    }        
	return (SyncItem*)c->getNewItems()->get(cnew)->clone();  
}

SyncItem* FILESyncSource::getFirstUpdatedItem() {
    ///// TBD //////
    return NULL;
    ////////////////

    if (c == NULL) {
        c = new ItemContainer();
    }

    if (c->getUpdatedItemsSize() == 0) {
        return NULL;
    }    
    cupdated = 0;
    ArrayList* list = c->getUpdatedItems();         
	return (SyncItem*)list->get(cupdated)->clone();          
}

SyncItem* FILESyncSource::getNextUpdatedItem() {
    ///// TBD //////
    return NULL;
    ////////////////

    if (c == NULL)
        return NULL;

    cupdated++;
    if (cupdated >= c->getUpdatedItemsSize()) {
        return NULL;
    }          
	return (SyncItem*)c->getUpdatedItems()->get(cupdated)->clone();  
}

SyncItem* FILESyncSource::getFirstDeletedItem() {
    ///// TBD //////
    return NULL;
    ////////////////

    if (c == NULL) {
        c = new ItemContainer();
    }

    if (c->getDeletedItemsSize() == 0) {
        return NULL;
    }    
    cdeleted = 0;
    ArrayList* list = c->getDeletedItems();    
	return (SyncItem*)list->get(cdeleted)->clone();      
}

SyncItem* FILESyncSource::getNextDeletedItem() {
    ///// TBD //////
    return NULL;
    ////////////////

    if (c == NULL)
        return NULL;

    cdeleted++;
    if (cdeleted >= c->getDeletedItemsSize()) {
        return NULL;
    }
	return (SyncItem*)c->getDeletedItems()->get(cdeleted)->clone();    
}



void FILESyncSource::setItemStatus(const wchar_t* key, int status) {
    LOG.debug(T("item key: %ls, status: %i"), key, status);    
}



//////////////////////////////////////////////////////////////////////////////////////////


int FILESyncSource::addItem(SyncItem& item) {
    
    FileData file;
    int ret = STC_COMMAND_FAILED;
    char* data = (char*)item.getData();
    size_t len = item.getDataSize();

    if (!file.parse(data, len)) {
        sprintf(lastErrorMsg, "Error parsing item from server");
        report->setLastErrorCode(ERR_BAD_FILE_CONTENT);
        report->setLastErrorMsg(lastErrorMsg);
        report->setState(SOURCE_ERROR);
        return STC_COMMAND_FAILED;
    }


    if (file.getSize() > 0) {
        //
        // Save item on FS
        //
        char completeName[512];
        sprintf(completeName, "%s/%ls", dir, file.getName());
        if (!saveFile(completeName, file.getBody(), file.getSize(), true)) {
            sprintf(lastErrorMsg, "Error saving file %ls", file.getName());
            report->setLastErrorCode(ERR_FILE_SYSTEM);
            report->setLastErrorMsg(lastErrorMsg);
            report->setState(SOURCE_ERROR);
            return STC_COMMAND_FAILED;
        }       
        ret = STC_ITEM_ADDED;
        LOG.debug(T("Added item: %ls"), file.getName());    

    }
    return ret;
}

int FILESyncSource::updateItem(SyncItem& item) {
    ////// TBD ////////
    return STC_COMMAND_FAILED;
    ///////////////////
    
    int ret = STC_COMMAND_FAILED;

    FileData file;
    char* data      = NULL;
    long h          = 0;
    wchar_t* encod  = NULL;
    int size = 0;
    int res = 0;
    size = item.getDataSize();    
    data = new char[size + 1];
    memset(data, 0, size + 1);
    memcpy(data, item.getData(), size);
    res = file.parse((data));
    encod = (wchar_t*)file.getEnc();
    delete [] data;

    if (wcslen(encod) > 0) {        
        item.setData(file.getBody(), file.getSize());
        //
        // Replace item on FS (res=h)
        //
    }  

    if (h == 0) {
        ret = STC_OK;
        LOG.debug(T("updated item: %S"), item.getKey());
    }
    return ret;
}

int FILESyncSource::deleteItem(SyncItem& item) {
    ////// TBD ////////
    return STC_COMMAND_FAILED;
    ///////////////////

    int ret = STC_COMMAND_FAILED;
    long h = 1;
    //
    // h = delete item from FS
    //
    
    if (h == 0) {
        ret = STC_OK;
        LOG.debug(T("deleted item: %S"), item.getKey());
    }
    return ret;    
}


int FILESyncSource::endSync() {
    //
    // write current file name and time of writing in file.db
    //
    return 0;
}

void FILESyncSource::assign(FILESyncSource& s) {
    setSyncMode(s.getSyncMode());
    setLastSync(s.getLastSync());
    setNextSync(s.getNextSync());
    setLastAnchor(s.getLastAnchor());
    setNextAnchor(s.getNextAnchor());
    setFilter(s.getFilter());
    setPath(getPath());
    setDir(getDir());
}

ArrayElement* FILESyncSource::clone() {
    FILESyncSource* s = new FILESyncSource(getName(), &(getConfig()));
    s->setPath(getPath());

    s->assign(*this);

    return s;
}


bool FILESyncSource::setItemData(SyncItem* syncItem) {

    bool ret = true;
    size_t len;
    char* content;
    char fileName[512];

    //
    // Get file content.
    //
    sprintf(fileName, "%s/%ls", dir, syncItem->getKey());
    if (!readFile(fileName, &content, &len, true)) {
        sprintf(lastErrorMsg, "Error opening the file '%s'", fileName);
        report->setLastErrorCode(ERR_FILE_SYSTEM);
        report->setLastErrorMsg(lastErrorMsg);
        report->setState(SOURCE_ERROR);
        return false;
    }
    
    //
    // Set data
    //
    if (content) {
        FileData file;
        file.setName(syncItem->getKey());
        file.setSize(len);
        file.setEnc(TEXT("base64"));
        file.setBody(content, (int)len);
        char* encContent = file.format();
        syncItem->setData(encContent, strlen(encContent));
        delete [] encContent;
        encContent = NULL;
        //syncItem->setData(content, (long)len);
        delete [] content; 
        content = NULL;
        return true;
    }
    else {
        sprintf(lastErrorMsg, "Error bad file content: '%s'", fileName);
        report->setLastErrorCode(ERR_BAD_FILE_CONTENT);
        report->setLastErrorMsg(lastErrorMsg);
        report->setState(SOURCE_ERROR);
        return false;
    }
}
