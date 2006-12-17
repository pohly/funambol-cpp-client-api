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

#include "spds/FILESyncSource.h"


FILESyncSource::FILESyncSource(const WCHAR* name, SyncSourceConfig* sc) : SyncSource(name, sc) {
    dir  = NULL;
    fileNode = NULL;

    setDir(".");
}

FILESyncSource::~FILESyncSource() {
    if(dir) {
        delete [] dir;
        dir = NULL;
    }
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
    allItems.items.clear();
    deletedItems.items.clear();
    newItems.items.clear();
    updatedItems.items.clear();

    
    //
    // Get file list.
    //
    int count;
    char** fileNames = readDir(dir, &count);
    LOG.info("The client number of files to sync are %i", count);

    //
    // Create array list with empty data from file names.
    //
    for (int i=0; i<count; i++) {
        if (fileNames[i]) {
            WCHAR* wname = toWideChar(fileNames[i]);
            SyncItem* s = new SyncItem(wname);
            allItems.items.add(*s);

            if (fileNode) {
                char completeName[512];
                sprintf(completeName, "%s/%s", dir, fileNames[i]);
                unsigned long fileModTime = getFileModTime(completeName);
                unsigned long serverModTime = getServerModTime(fileNames[i]);

                if (!serverModTime) {
                    // added file
                    newItems.items.add(*s);
                } else if (serverModTime < fileModTime) {
                    // updated file
                    updatedItems.items.add(*s);
                }
            }

            delete s;
            delete [] wname;
            delete [] fileNames[i];
        }
    }

    if (fileNode) {
        // iterate over all files stored on server (i.e. those with non-zero time stamp property)
        // and check which of these have been deleted locally
        //
        // TODO: currently impossible with the ManagementNode interface, have to guess file names (works
        // for RawFILESyncSource)

        for (int key = 0; key < 1000; key++) {
            char keystr[80];
            sprintf(keystr, "%d", key);

            if (getServerModTime(keystr)) {
                char completeName[512];
                sprintf(completeName, "%s/%s", dir, keystr);
                if (!getFileModTime(completeName)) {
                    // file no longer exists locally
                    WCHAR* wname = toWideChar(keystr);
                    SyncItem* s = new SyncItem(wname);
                    deletedItems.items.add(*s);
                    delete s;
                    delete wname;
                }
            }
        }
    }
    
    if (fileNames) {
        delete [] fileNames;  
        fileNames = NULL;
    }
    return 0;    
}

SyncItem* FILESyncSource::getFirst(ItemIteratorContainer& container, BOOL getData) {
    container.index = 0;
    if (container.index >= container.items.size()) {
        return NULL;
    }
    SyncItem* syncItem = (SyncItem*)container.items.get(container.index)->clone();

    //
    // Set data from file content, return syncItem (freed by API)
    //
    if (!getData || setItemData(syncItem)){
        return syncItem;
    }
    else {
        delete syncItem;
        return NULL;
    }
}

SyncItem* FILESyncSource::getNext(ItemIteratorContainer& container, BOOL getData) {
    container.index++;
    if (container.index >= container.items.size()) {
        return NULL;
    }
    SyncItem* syncItem = (SyncItem*)container.items.get(container.index)->clone();

    // Set data from file content, return syncItem (freed by API)
    if (!getData || setItemData(syncItem)){
        return syncItem;
    }
    else {
        delete syncItem;
        return NULL;
    }
}

unsigned long FILESyncSource::getServerModTime(const char* keystr) {
    unsigned long modtime = 0;
    if (fileNode) {
        char* timestr = fileNode->readPropertyValue(keystr);
        modtime = anchorToTimestamp(timestr);
        delete [] timestr;
    }
    return modtime;
}

void FILESyncSource::setItemStatus(const WCHAR* key, int status) {
    LOG.debug("item key: %" WCHAR_PRINTF ", status: %i", key, status);    
}



//////////////////////////////////////////////////////////////////////////////////////////


int FILESyncSource::addItem(SyncItem& item) {

    // format is the custom XML format understood by FileData
    int ret = STC_COMMAND_FAILED;
    FileData file;
    char* data = (char*)item.getData();
    size_t len = item.getDataSize();

    if (file.parse(data, len)) {
        sprintf(lastErrorMsg, "Error parsing item from server");
        report->setLastErrorCode(ERR_BAD_FILE_CONTENT);
        report->setLastErrorMsg(lastErrorMsg);
        report->setState(SOURCE_ERROR);
        return STC_COMMAND_FAILED;
    }


    if (file.getSize() >= 0) {
        //
        // Save item on FS
        //
        char completeName[512];
        sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, file.getName());
        if (!saveFile(completeName, file.getBody(), file.getSize(), true)) {
            sprintf(lastErrorMsg, "Error saving file %" WCHAR_PRINTF, file.getName());
            report->setLastErrorCode(ERR_FILE_SYSTEM);
            report->setLastErrorMsg(lastErrorMsg);
            report->setState(SOURCE_ERROR);
            return STC_COMMAND_FAILED;
        }       
        ret = addedItem(item, file.getName());
        LOG.debug("Added item: %" WCHAR_PRINTF, file.getName());    
    }
    return ret;
}

int FILESyncSource::addedItem(SyncItem& item, const WCHAR* key) {
    item.setKey(key);

    // remember this item so that endSync() can store its time stamp
    SyncItem smallitem;
    smallitem.setKey(key);
    allItems.items.add(smallitem);

    return STC_ITEM_ADDED;
}

int FILESyncSource::updateItem(SyncItem& item) {
    ////// TBD ////////
    return STC_COMMAND_FAILED;
    ///////////////////
    
    int ret = STC_COMMAND_FAILED;

    FileData file;
    char* data      = NULL;
    long h          = 0;
    WCHAR* encod  = NULL;
    int size = 0;
    int res = 0;
    size = item.getDataSize();    
    data = new char[size + 1];
    memset(data, 0, size + 1);
    memcpy(data, item.getData(), size);
    res = file.parse((data));
    encod = (WCHAR*)file.getEnc();
    delete [] data;

    if (wcslen(encod) > 0) {        
        item.setData(file.getBody(), file.getSize());
        //
        // Replace item on FS (res=h)
        //
    }  

    if (h == 0) {
        ret = STC_OK;
        LOG.debug("updated item: %S", item.getKey());
    }
    return ret;
}

int FILESyncSource::deleteItem(SyncItem& item) {
    int ret = STC_COMMAND_FAILED;

    char completeName[512];
    sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, item.getKey());
    if (!unlink(completeName)) {
        ret = STC_OK;
    }

    return ret;
}


int FILESyncSource::endSync() {
    if (fileNode) {
        SyncItem* item;

        // reset information about deleted items
        for (item = getFirst(deletedItems, FALSE); item; item = getNext(deletedItems, FALSE)) {
            fileNode->setPropertyValue(item->getKey(), "");
            delete item;
        }

        // update information about each file that currently exists on the server
        for (item = getFirst(allItems, FALSE); item; item = getNext(allItems, FALSE)) {
            char completeName[512];
            sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, item->getKey());
            unsigned long modTime = getFileModTime(completeName);
            char anchor[30];
            timestampToAnchor(modTime, anchor);
            fileNode->setPropertyValue(item->getKey(), anchor);
            delete item;
        }
    }

    return 0;
}

void FILESyncSource::assign(FILESyncSource& s) {
    SyncSource::assign(s);
    setDir(getDir());
}

ArrayElement* FILESyncSource::clone() {
    FILESyncSource* s = new FILESyncSource(getName(), &(getConfig()));

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
    sprintf(fileName, "%s/%" WCHAR_PRINTF, dir, syncItem->getKey());
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
