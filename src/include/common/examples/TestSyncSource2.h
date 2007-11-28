/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
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

#ifndef INCL_TEST_SYNC_SOURCE2
#define INCL_TEST_SYNC_SOURCE2
/** @cond DEV */

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncMap.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSource.h"

class TestSyncSource2 : public SyncSource {

public:

    /**
     * Constructor: create a SyncSource with the specified name
     *
     * @param name - the name of the SyncSource
     * @param sc   - the SyncSourceConfig
     */
    TestSyncSource2(const WCHAR* name, SyncSourceConfig *sc);

    // TestSyncSource
    ~TestSyncSource2();

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
     /*
     * Return the next SyncItem of updated one. It is used in case of fast sync
     * and retrieve the new data source content.
     */

    SyncItem* getFirstItemKey();

    /*
     * Return the key of the next SyncItem of all.
     * It is used in case of refresh sync
     * and retrieve all the keys of the data source.
     */
    SyncItem* getNextItemKey();

    SyncItem* getNextDeletedItem();

    void setItemStatus(const WCHAR* key, int status);

    int addItem(SyncItem& item);

    int updateItem(SyncItem& item);

    int deleteItem(SyncItem& item);

    int beginSync();

    int endSync();

    ArrayElement* clone() { return 0; }
};

/** @endcond */
#endif
