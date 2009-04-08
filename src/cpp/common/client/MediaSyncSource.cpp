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
                                 const StringBuffer& mediaDir, 
                                 const bool aRecursive)
                                 : FileSyncSource(name, sc, mediaDir, aRecursive) {
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
