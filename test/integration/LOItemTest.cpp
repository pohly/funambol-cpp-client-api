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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef ENABLE_INTEGRATION_TESTS

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/globalsdef.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/StringBuffer.h"
#include "base/adapter/PlatformAdapter.h"
#include "client/DMTClientConfig.h"
#include "client/SyncClient.h"
#include "MappingTestSyncSource.h"
#include "spds/MappingsManager.h"
#include "spds/MappingStoreBuilder.h"
#include "LOItemTest.h"
#include "spds/SyncManager.h"
#include "testUtils.h"

USE_NAMESPACE


LOItemTest::LOItemTest() {
    LOG.setLogName("LOItemTest.log");
    LOG.reset();
}

static DMTClientConfig* getConfiguration(const char* name) {

    const char *serverUrl = getenv("CLIENT_TEST_SERVER_URL");
    const char *username = getenv("CLIENT_TEST_USERNAME");
    const char *password = getenv("CLIENT_TEST_PASSWORD");
    
    initAdapter(name);
    DMTClientConfig* config = new DMTClientConfig();
    config->read();
    DeviceConfig &dc(config->getDeviceConfig());
    if (!strlen(dc.getDevID())) {
        config->setClientDefaults();
        config->setSourceDefaults("calendar");
        StringBuffer devid("sc-pim-"); devid += name;
        dc.setDevID(devid);
        SyncSourceConfig* s = config->getSyncSourceConfig("calendar");
        s->setEncoding("b64");
        s->setURI("scal");
        s->setType("text/x-s4j-sife");
    }

    //set custom configuration
    if(serverUrl) {
        config->getAccessConfig().setSyncURL(serverUrl);
    }
    if(username) {
        config->getAccessConfig().setUsername(username);
    }
    if(password) {
        config->getAccessConfig().setPassword(password);
    }
    
    return config;
}

    // Add 3 contacts from the source first to the server

void LOItemTest::testLOItem() {
    
    ArrayList asources;
    StringBuffer contact("contact");
    asources.add(contact);
    DMTClientConfig* config = getNewDMTClientConfig("funambol_LOItem", true, &asources);
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    CPPUNIT_ASSERT(config);
    conf->setEncoding("b64");
    conf->setType("text/x-s4j-sifc");        
    conf->setSync("two-way");
    conf->setURI("scard");
    conf->setVersion("1.0");
    
    config->getAccessConfig().setMaxMsgSize(10000);                       
    MappingTestSyncSource* scontact1 = new MappingTestSyncSource(TEXT("contact"),  conf);

    SyncSource* sources[2];
    sources[0] = scontact1;
    sources[1] = NULL;

    SyncClient client;
    int ret = 0;
    ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);
    config->save();

    delete scontact1;

    config->read();
    conf = config->getSyncSourceConfig("contact");
    scontact1 = new MappingTestSyncSource(TEXT("contact"),  conf);    
    // it uses the same config    
    ret = client.sync(*config, sources);



}

#endif // ENABLE_INTEGRATION_TESTS
