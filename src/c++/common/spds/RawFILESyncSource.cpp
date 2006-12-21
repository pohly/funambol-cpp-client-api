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

#include "spds/RawFILESyncSource.h"


RawFILESyncSource::RawFILESyncSource(const WCHAR* name, SyncSourceConfig* sc) : FILESyncSource(name, sc) {
}

int RawFILESyncSource::addItem(SyncItem& item) {
    char completeName[512];
    int key = 0;

    while(1) {
        sprintf(completeName, "%s/%d", dir, key);
            
        FILE *fh = fopen(completeName, "r");
        if (!fh) {
            if (!saveFile(completeName, (const char *)item.getData(), item.getDataSize(), TRUE)) {
                sprintf(lastErrorMsg, "Error saving file %s", completeName);
                report->setLastErrorCode(ERR_FILE_SYSTEM);
                report->setLastErrorMsg(lastErrorMsg);
                report->setState(SOURCE_ERROR);
                return STC_COMMAND_FAILED;
            } else {
                WCHAR keystr[80];
                swprintf(keystr, TEXT("%d"), key);
                item.setKey(keystr);
                return addedItem(item, keystr);
            }
        } else {
            fclose(fh);
            key++;
        }
    }
}

int RawFILESyncSource::updateItem(SyncItem& item) {
    char completeName[512];
    sprintf(completeName, "%s/%" WCHAR_PRINTF, dir, item.getKey());
    if (!saveFile(completeName, (const char *)item.getData(), item.getDataSize(), TRUE)) {
        sprintf(lastErrorMsg, "Error saving file %s", completeName);
        report->setLastErrorCode(ERR_FILE_SYSTEM);
        report->setLastErrorMsg(lastErrorMsg);
        report->setState(SOURCE_ERROR);
        return STC_COMMAND_FAILED;
    } else {
        return STC_OK;
    }
}

bool RawFILESyncSource::setItemData(SyncItem* syncItem) {

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
        syncItem->setData(content, (long)len);
		WCHAR *tmp = toWideChar(config.getType());
        syncItem->setDataType(tmp);
		delete [] tmp;
        delete [] content; 
        content = NULL;
    }
    return true;
}
