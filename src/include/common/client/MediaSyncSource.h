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

#ifndef MEDIASOURCESYNC_H_
#define MEDIASOURCESYNC_H_

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "base/util/ItemContainer.h"
#include "spds/FileData.h"
#include "client/CacheSyncSource.h"
#include "client/FileSyncSource.h"

// for stat 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


BEGIN_NAMESPACE

/**
 * This class extends the FileSyncSource class, to define a special behavior for generic
 * "media files" to be synchronized between the Server and a mobile Client.
 * 
 * - getAllItemList() reads files recursively under "dir" folder, scanning all subfolders
 * - removeAllItems() deletes files recursively under "dir" folder
 * - in case of slow sync, the cache file is not cleared and new/mod/del items are sent (as if it was a fast sync). 
 *   Delete items are sent as empty update items (the Server doesn't expect deletes during slow syncs)
 * - manages a mapping between the LUID and the item's full name
 * - cache file is stored inside each folder
 */
class MediaSyncSource : public FileSyncSource
{

public:
    MediaSyncSource(const WCHAR* name,
                   AbstractSyncSourceConfig* sc,
                   const StringBuffer& mediaDir);

    ~MediaSyncSource() {};

    /**
     * Populates an ArrayListEnumeration with all the files found.
     * Will call scanFolder() to find files recursively in the current dir.
     */
    Enumeration* getAllItemList();
    
    /**
     * Cleans up all files under dir, recursively.
     * Calls cleanUpFolder().
     */
    int removeAllItems();
    
    
    /// From FileSyncSource - implemented empty.
    int insertItem(SyncItem& item);
    
    /// From FileSyncSource - implemented empty.
    int modifyItem(SyncItem& item);
    
    /// From FileSyncSource - implemented empty.
    int removeItem(SyncItem& item);
    

protected:
    
    /// max size in kilo bytes, of each item to send
    int maxFileSize;
    
    /**
     * Used to filter outgoing items. Current implementation 
     * doesn't filter any item: override this method for specific filtering.
     * @param fullName  the full path + name of the file to check
     * @param st        reference to struct stat for current file
     * @return          true if the item has to be filtered out (skipped)
     *                  false if the item is ok
     */ 
    virtual bool filterOutgoingItem(const StringBuffer& fullName, struct stat& st) { return false; }
    
    
private:

    /**
     * Get the list of the files that are in the directory, recursively.
     * Will call filterOutgoingItem() for each item found.
     */
    bool scanFolder(const StringBuffer&, ArrayList& currentKeys);
    
    /**
     * Cleanup all the files that are in the directory, recursively.
     */
    bool cleanUpFolder(const char *aDirPath);

};

END_NAMESPACE

#endif /*MEDIASOURCESYNC_H_*/
