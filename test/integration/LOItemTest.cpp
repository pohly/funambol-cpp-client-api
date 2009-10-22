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
#include "LOSyncSource.h"
#include "LOItemTest.h"
#include "spds/SyncManager.h"
#include "testUtils.h"

USE_NAMESPACE


LOItemTest::LOItemTest() {
    LOG.setLogName("LOItemTest.log");
    LOG.reset();
}

bool isSuccessful(const int status) {
    if (status == 201 || status == 200)
        return true;
    else
        return false;
}

int getOperationSuccessful(SyncSourceReport *ssr, const char* target, const char* command) {
    ArrayList* list = ssr->getList(target, command);
    ItemReport* e;
    
    // Scan for successful codes
    int good = 0;
    if (list->size() > 0) {
        e = (ItemReport*)list->front();
        if ( isSuccessful(e->getStatus()) ) {
            good++;            
        }
        for (int i=1; i<list->size(); i++) {
            e = (ItemReport*)list->next();            
            if ( isSuccessful(e->getStatus())) {
                good++;
            }
        }
    }
    return good;
}
int getSuccessfullyAdded(SyncSourceReport *ssr) {
    return getOperationSuccessful(ssr, SERVER, COMMAND_ADD);
}
int getSuccessfullyReplaced(SyncSourceReport *ssr) {
    return getOperationSuccessful(ssr, SERVER, COMMAND_REPLACE);
}

void LOItemTest::testLOItem() {
    
    DMTClientConfig* config = resetItemOnServer("card");
    CPPUNIT_ASSERT(config);
    config->read();    
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf); 
    scontact->setUseAdd(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, added);
}

void LOItemTest::testLOItemDES() {
    
    DMTClientConfig* config = resetItemOnServer("scard");
    CPPUNIT_ASSERT(config);
    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");     
    conf->setEncoding("b64");
    conf->setEncryption("des");
    conf->setType("text/x-s4j-sifc");        
    conf->setURI("scard");
    conf->setVersion("1.0");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    
    scontact->setUseSif(true);
    scontact->setUseAdd(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
    
    CPPUNIT_ASSERT_EQUAL(2, added);
 
}

void LOItemTest::testLOItemWithItemEncoding() {
    
    DMTClientConfig* config = resetItemOnServer("card");
    CPPUNIT_ASSERT(config);
    config->read();    
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf); 
    scontact->setUseAdd(true);
    scontact->setUseDataEncoding(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, added);
}


void LOItemTest::testLOItemb64() {
           
    DMTClientConfig* config = resetItemOnServer("scard");
    CPPUNIT_ASSERT(config);
    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");     
    conf->setEncoding("b64");
    conf->setType("text/x-s4j-sifc");        
    conf->setURI("scard");
    conf->setVersion("1.0");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    
    scontact->setUseSif(true);
    scontact->setUseAdd(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyAdded(ssr);
    
    CPPUNIT_ASSERT_EQUAL(2, added);
    config->save();

}

void LOItemTest::testLOItemReplaceb64() {
    
    testLOItemb64();
    initAdapter("funambol_LOItem");    
    DMTClientConfig* config = new DMTClientConfig();    
    CPPUNIT_ASSERT(config);
    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");     
    conf->setEncoding("b64");
    conf->setType("text/x-s4j-sifc");        
    conf->setURI("scard");
    conf->setVersion("1.0");
    conf->setSync("two-way");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    
    scontact->setUseSif(true);
    scontact->setUseAdd(false);
    scontact->setUseUpdate(true);
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int added = getSuccessfullyReplaced(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, added);

}

DMTClientConfig* LOItemTest::resetItemOnServer(const char* sourceURI) {
    
    ArrayList asources;
    StringBuffer contact("contact");
    asources.add(contact);
    DMTClientConfig* config = getNewDMTClientConfig("funambol_LOItem", true, &asources);
    CPPUNIT_ASSERT(config);
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("refresh-from-client");
    conf->setURI(sourceURI);

    config->getAccessConfig().setMaxMsgSize(5500);                       
    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);    

    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;

    SyncClient client;
    int ret = 0;
    ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);
    config->save();

    delete scontact;
    return config;

}

void LOItemTest::testLOItemSlowSync() {

    DMTClientConfig* config = resetItemOnServer("card");

    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("slow");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);   
    scontact->setUseSlowSync(true);
    
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int replaced = getSuccessfullyReplaced(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, replaced);
}

void LOItemTest::testLOItemSlowSyncb64() {

    DMTClientConfig* config = resetItemOnServer("scard");

    config->read();
    SyncSourceConfig *conf = config->getSyncSourceConfig("contact");
    conf->setSync("slow");

    LOSyncSource* scontact = new LOSyncSource(TEXT("contact"),  conf);   
    scontact->setUseSlowSync(true);
    
    SyncSource* sources[2];
    sources[0] = scontact;
    sources[1] = NULL;
    
    SyncClient client;
    int ret = client.sync(*config, sources);
    CPPUNIT_ASSERT(!ret);

    SyncSourceReport *ssr = scontact->getReport();
    int replaced = getSuccessfullyReplaced(ssr);
 
    CPPUNIT_ASSERT_EQUAL(2, replaced);
}
#endif // ENABLE_INTEGRATION_TESTS
