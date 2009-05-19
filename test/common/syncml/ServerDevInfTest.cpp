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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

#include "spds/SyncManagerConfig.h"
#include "spds/SyncReport.h"
#include "spds/SyncManager.h"
#include "spds/spdsutils.h"
#include "spds/SyncMLBuilder.h"

USE_NAMESPACE



class ServerDevInfTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ServerDevInfTest);
        CPPUNIT_TEST(testAskServerDevInf);
        CPPUNIT_TEST(testGetServerDevInf);
        CPPUNIT_TEST(testFormatServerDevInf);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(){

    }

    void tearDown(){

    }

private:

    //////////////////////////////////////////////////////// Test /////
    void testAskServerDevInf() {

        bool ret = false;
        SyncManagerConfig config;
        SyncReport report;
        
        report.setSyncSourceReports(config);
        SyncManager syncManager(config, report);

        // No Server swv available in config: expect true
        config.setForceServerDevInfo(false);
        config.setServerSwv("");
        ret = syncManager.askServerDevInf();
        CPPUNIT_ASSERT(ret);

        // Server swv available in config: expect false
        config.setForceServerDevInfo(false);
        config.setServerSwv("8.0.0");
        ret = syncManager.askServerDevInf();
        CPPUNIT_ASSERT(!ret);

        // Force it via the config flag: expect true
        config.setForceServerDevInfo(true);
        config.setServerSwv("8.0.0");
        ret = syncManager.askServerDevInf();
        CPPUNIT_ASSERT(ret);
    }


    //////////////////////////////////////////////////////// Test /////
    void testGetServerDevInf() {

        SyncMLBuilder syncMLBuilder;
        AbstractCommand* command = syncMLBuilder.prepareServerDevInf();
        CPPUNIT_ASSERT(command);

        // Check it's a Get
        StringBuffer name = command->getName();
        CPPUNIT_ASSERT(name == GET);
        Get* get = (Get*)command;
        CPPUNIT_ASSERT(get);


        // Check the meta type = "application/vnd.syncml-devinf+xml"
        Meta* meta = get->getMeta();
        CPPUNIT_ASSERT(meta);
        StringBuffer type = meta->getType();
        CPPUNIT_ASSERT(type == DEVINF_FORMAT);

        ArrayList* items = get->getItems();
        CPPUNIT_ASSERT(items);

        // Expect only 1 item
        CPPUNIT_ASSERT_EQUAL(1, items->size());
        Item* item = (Item*)items->get(0);
        CPPUNIT_ASSERT(item);

        // Check the target locURI = "./devinf12"
        Target* target = item->getTarget();
        CPPUNIT_ASSERT(target);

        StringBuffer locURI = target->getLocURI();
        CPPUNIT_ASSERT(locURI == DEVINF_URI);


        delete command;
    }

    //////////////////////////////////////////////////////// Test /////
    void testFormatServerDevInf() {

        SyncMLBuilder syncMLBuilder;
        ArrayList commands;
        AbstractCommand* get = syncMLBuilder.prepareServerDevInf();
        CPPUNIT_ASSERT(get);
            
        commands.add(*get);
        delete get;
        SyncML* syncml = syncMLBuilder.prepareInitObject(NULL, NULL, &commands);
        CPPUNIT_ASSERT(syncml);

        char* msg = syncMLBuilder.prepareMsg(syncml);
        CPPUNIT_ASSERT(msg);


        // Check if this part of XML is included in the SyncML msg
        int cmdID = 1;
        StringBuffer expected;
        expected.sprintf("<Get><CmdID>%d</CmdID><Meta><Type xmlns=\"syncml:metinf\">%s</Type></Meta><Item><Target><LocURI>%s</LocURI></Target></Item></Get>", 
                         cmdID, DEVINF_FORMAT, DEVINF_URI);
        
        StringBuffer syncmlMsg(msg);
        syncmlMsg.replaceAll("\n","");
        size_t pos = syncmlMsg.find(expected);
        CPPUNIT_ASSERT(pos != StringBuffer::npos);

        delete [] msg;
        deleteSyncML(&syncml);
    }


};

CPPUNIT_TEST_SUITE_REGISTRATION( ServerDevInfTest );