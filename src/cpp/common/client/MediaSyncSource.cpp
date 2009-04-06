/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2009 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#include "spds/SyncItem.h"
#include "spds/SyncItemStatus.h"
#include "base/util/utils.h"
#include "base/Log.h"
#include "syncml/core/TagNames.h"
#include "base/util/ArrayListEnumeration.h"
#include "client/FileSyncSource.h"
#include "client/MediaSyncSource.h"

BEGIN_NAMESPACE

MediaSyncSource::MediaSyncSource(const WCHAR* name, 
                                 AbstractSyncSourceConfig* sc, 
                                 const StringBuffer& mediaDir)
                                 : FileSyncSource(name, sc, mediaDir) {
    
    
    // Cut the last "\" or "/"
    if (mediaDir.endsWith("\\") || mediaDir.endsWith("/")) {
        dir = mediaDir.substr(0, mediaDir.length()-1);
    }
}


Enumeration* MediaSyncSource::getAllItemList()
{
    ArrayList currentKeys;
    Enumeration* allKeys = NULL;        
    
    if (scanFolder("", currentKeys) == false) {
        LOG.error("error reading current folder: %s", dir.c_str());
    }

    LOG.info("The Client number of files read is %i", currentKeys.size());
    allKeys = new ArrayListEnumeration(currentKeys);

    return allKeys;
}



// read recursively directory contents
bool MediaSyncSource::scanFolder(const StringBuffer& aRelativePath, ArrayList& currentKeys)
{
    int count = 0;
    struct stat st;
    StringBuffer name, fullName;
    
    // Compose full path: 'dir\relativePath'
    StringBuffer fullPath(dir);
    if (!dir.endsWith("\\")) { 
        fullPath.append("\\"); 
    }
    fullPath.append(aRelativePath);
    //LOG.debug("dir = %s, aRelativePath = %s, fullPath = %s", dir.c_str(), aRelativePath.c_str(), fullPath.c_str());
    
    char** fileNames = readDir(fullPath, &count, false);
    if (fileNames == NULL) {
        LOG.debug("Directory '%s' has no items", fullPath.c_str());
        return true;
    }

    for (int i = 0; i < count; i++) {
        if (fileNames[i]) {
            memset(&st, 0, sizeof(struct stat));

            if (aRelativePath.empty() || aRelativePath.endsWith("\\")) {
                name.sprintf("%s%s", aRelativePath.c_str(), fileNames[i]);
                fullName.sprintf("%s%s", fullPath.c_str(), fileNames[i]);
            }
            else {
                name.sprintf("%s\\%s", aRelativePath.c_str(), fileNames[i]);
                fullName.sprintf("%s\\%s", fullPath.c_str(), fileNames[i]);
            }
            //LOG.debug("name = %s, fullName = %s", name.c_str(), fullName.c_str());
            

            if (stat(fullName, &st) < 0) {
                LOG.error("can't stat file '%s' [%d]", fullName.c_str(), errno);
                continue;
            }
            
            // Filter outgoing items
            if (filterOutgoingItem(fullName, st)) {
                //LOG.debug("Skipping item '%s'", fullName.c_str());
                continue;
            }

            if (S_ISDIR(st.st_mode)) {
                // It's a directory -> recurse into it.
                if (scanFolder(name, currentKeys) == false) {
                    LOG.error("Error reading '%s' folder", fullName.c_str());
                }
            } 
            else {
                //
                // It's a file -> add it (the item key is its full path)
                //
                currentKeys.add(fullName);
            }
        }
    }
    
    // delete fileNames
    for (int i = 0; i < count; i++) {
        delete [] fileNames[i]; fileNames[i] = NULL; 
    }
    if (fileNames != NULL) {
        delete [] fileNames; fileNames = NULL;
    }
    
    return true;
}


int MediaSyncSource::removeAllItems()
{
    if (cleanUpFolder(dir.c_str()) == false) {
        LOG.error("Error removing folder: %s", dir.c_str());
        return 1;
    }
    
    return 0;
}

// cleans up directories recursively
bool MediaSyncSource::cleanUpFolder(const char* aDirPath)
{
    int count = 0;
    struct stat st;
    char path[KMaxPath + 1];

    if (aDirPath == NULL) {
        LOG.error("invalid folder path");
        
        return false;
    }
    
    char** fileNames = readDir(aDirPath, &count, false);
    if (fileNames == NULL) {
        LOG.debug("directory '%s' has no items", aDirPath);
        
        return true;
    }
    
    for (int i = 0; i < count; i++) {
        memset(path, 0, KMaxPath + 1);
        
        if (aDirPath[strlen(aDirPath) - 1] == '\\') {
            sprintf(path, "%s%s", aDirPath, fileNames[i]);
        } else {
            sprintf(path, "%s\\%s", aDirPath, fileNames[i]);
        }
        
        if (stat(path, &st) < 0) {
            LOG.error("can't stat file '%s'", path);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            if (cleanUpFolder(path) == false) {
                LOG.error("error cleaning up '%s' folder", path);
            }
        } else {
            LOG.debug("removing file %s", fileNames[i]);
            removeFileInDir(aDirPath, fileNames[i]);
        }
    }
    
    // delete fileNames
    for (int i = 0; i < count; i++) {
        delete [] fileNames[i]; fileNames[i] = NULL; 
    }
    if (fileNames != NULL) {
        delete [] fileNames; fileNames = NULL;
    }
    
    return true;
}



int MediaSyncSource::insertItem(SyncItem& item) 
{
    // Items are not added from Server to Client.
    // So just log a warning and return OK.
    StringBuffer key;
    key.convert(item.getKey());
    LOG.debug("Warning: unexpected call MediaSyncSource::insertItem() for item key = %s", key.c_str());
    
    return STC_OK;
}

int MediaSyncSource::modifyItem(SyncItem& item) 
{
    // Items are not modified from Server to Client.
    // So just log a warning and return OK.
    StringBuffer key;
    key.convert(item.getKey());
    LOG.debug("Warning: unexpected call MediaSyncSource::modifyItem() for item key = %s", key.c_str());
    
    return STC_OK;
}

int MediaSyncSource::removeItem(SyncItem& item) 
{
    // Items are not deleted from Server to Client.
    // So just log a warning and return OK.
    StringBuffer key;
    key.convert(item.getKey());
    LOG.debug("Warning: unexpected call MediaSyncSource::removeItem() for item key = %s", key.c_str());
    
    return STC_OK;
}


END_NAMESPACE
