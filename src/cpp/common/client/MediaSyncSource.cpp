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
#include "base/util/PropertyFile.h"

BEGIN_NAMESPACE

#define MEDIA_CACHE_FILE_NAME   "funambol_cache.dat"

#define CACHE_PROPERTY_URL      "_SERVER_URL_"
#define CACHE_PROPERTY_USERNAME "_USERNAME_"
#define CACHE_PROPERTY_SWV      "_CLIENT_SWV_"


/**
 * Reads the media cache file.
 * The media cache file is located inside the folder to sync (dir).
 * @param mediaDir  the absolute path where cache file is found
 * @return          new allocated cache, as a KeyValueStore*
 */
static KeyValueStore* getMediaCache(const StringBuffer& dir) 
{
    //LOG.debug("getMediaCache");
    
    StringBuffer cacheFileName(dir);
    if (dir.endsWith("\\") || dir.endsWith("/")) {
        cacheFileName = dir.substr(0, dir.length()-1);
    }
    cacheFileName += "/";
    cacheFileName += MEDIA_CACHE_FILE_NAME;

    LOG.debug("MediaSyncSource: cache file is %s", cacheFileName.c_str());
    
    return new PropertyFile(cacheFileName);
}



MediaSyncSource::MediaSyncSource(const WCHAR* wname, 
                                 AbstractSyncSourceConfig* sc,
                                 const StringBuffer& aDir, 
                                 MediaSyncSourceParams mediaParams) 
                                 : FileSyncSource(wname, sc, aDir, getMediaCache(aDir)), 
                                 params(mediaParams) {}


int MediaSyncSource::beginSync() {
    
    // If URL or username stored in cache are wrong, reset the cache!
    if (checkCacheValidity() == false) {
        LOG.debug("Resetting cache file");
        clearCache();
    }
    
    // Saves the cache: it's updated with right special props in case
    // something goes wrong during the sync.
    saveCache();
    
    
    // NO: if sync crashes, next time these props would be removed from
    //     the cache because of the journal file
    // Remove those values from cache, so the CacheSyncSource 
    // won't see them during sync.
//    KeyValuePair url, user, swv;
//    
//    url.setKey (CACHE_PROPERTY_URL);
//    user.setKey(CACHE_PROPERTY_USERNAME);
//    swv.setKey (CACHE_PROPERTY_SWV);
//    
//    removeFromCache(url);
//    removeFromCache(user);
//    removeFromCache(swv);
    
    
    return FileSyncSource::beginSync();
}



bool MediaSyncSource::checkCacheValidity()
{
    const StringBuffer& url      = params.getUrl();
    const StringBuffer& username = params.getUsername();
    const StringBuffer& swv      = params.getSwv();
    
    StringBuffer cacheUrl        = readCachePropertyValue(CACHE_PROPERTY_URL);
    StringBuffer cacheUsername   = readCachePropertyValue(CACHE_PROPERTY_USERNAME);
    StringBuffer cacheSwv        = readCachePropertyValue(CACHE_PROPERTY_SWV);
    
    //LOG.debug("Current params (%s %s %s)", url.c_str(), username.c_str(), swv.c_str());
    //LOG.debug("Cache params   (%s %s %s)", cacheUrl.c_str(), cacheUsername.c_str(), cacheSwv.c_str());
    
    if (url != cacheUrl || username != cacheUsername) {
        LOG.info("Media cache file is not valid (%s %s %s)", cacheUrl.c_str(), cacheUsername.c_str(), cacheSwv.c_str());
        return false;
    }
    
    //
    // Add here checks about client swv, for backward compatibility.
    //
    return true;
}


int MediaSyncSource::saveCache()
{
    
    // TODO: save the LUIDMap file
    
    
    // Update cache with the right values of special props.
    KeyValuePair url, user, swv;
    
    url.setKey (CACHE_PROPERTY_URL);        url.setValue (params.getUrl());
    user.setKey(CACHE_PROPERTY_USERNAME);   user.setValue(params.getUsername());
    swv.setKey (CACHE_PROPERTY_SWV);        swv.setValue (params.getSwv());

    updateInCache(url,  REPLACE);
    updateInCache(user, REPLACE);
    updateInCache(swv,  REPLACE);
    
    
    // Will persist the cache in memory.
    return FileSyncSource::saveCache();
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


bool MediaSyncSource::filterOutgoingItem(const StringBuffer& fullName, struct stat& st)
{
    
    if (!S_ISDIR(st.st_mode)) 
    {
        // skip the '.dat' and '.jour' files (cache files)
        if (checkFileExtension(fullName, "dat")) {
            LOG.debug("skipping cache file '%s'", fullName.c_str());
            return true;
        }
        if (checkFileExtension(fullName, "jour")) {
            LOG.debug("skipping cache journal file '%s'", fullName.c_str());
            return true;
        }
        
        // TODO: filter media extensions? (jpg, wav, avi...)
    }

    return FileSyncSource::filterOutgoingItem(fullName, st);
}



StringBuffer MediaSyncSource::getItemSignature(StringBuffer& key) {

    if (key.length() <= 0) {
        return NULL;
    }
    
    LOG.debug("[%s] MediaSyncSource - getting signature for item with key %s", getConfig().getName(), key.c_str());       
    StringBuffer s;
    
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    if (stat(key, &st) < 0) {
        LOG.error("can't stat file '%s' [%d]", key.c_str(), errno);
        return NULL;
    }
    
    //LOG.debug("signature is %d", st.st_mtime);
    s.sprintf("%d", st.st_mtime);
    return s;
}


bool MediaSyncSource::fillItemModifications()
{
    // Will fill new/mod/del item keys lists.
    bool ret = CacheSyncSource::fillItemModifications();
    
    
    // Remove the special props (url, username, swv) from the deleteKeys
    // (those are fake items, so they can't be found on filesystem)
    ArrayListEnumeration* keys = (ArrayListEnumeration*)deletedKeys;
    
    for (int i=0; i<keys->size(); ) {
        StringBuffer* key = (StringBuffer*)keys->get(i);
        
        if ( *key == CACHE_PROPERTY_URL ||
             *key == CACHE_PROPERTY_USERNAME ||
             *key == CACHE_PROPERTY_SWV ) {
            keys->removeElementAt(i);
            continue;
        }
        i++;
    }

    return ret;
}


END_NAMESPACE
