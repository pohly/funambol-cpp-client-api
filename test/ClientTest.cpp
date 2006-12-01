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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ClientTest.h"
#include "base/test.h"

#ifdef ENABLE_INTEGRATION_TESTS

#include <memory>
#include <vector>
#include <utility>

/** execute _x and the status of _source afterwards */
#define SOURCE_ASSERT_NO_FAILURE(_source, _x) \
{ \
    CPPUNIT_ASSERT_NO_THROW(_x); \
    /* TODO: check _source */ \
}

/** check _x and the status of _source afterwards */
#define SOURCE_ASSERT(_source, _x) \
{ \
    CPPUNIT_ASSERT(_x); \
    /* TODO: check _source */ \
}

/** same as SOURCE_ASSERT() with a specific failure message */
#define SOURCE_ASSERT_MESSAGE(_message, _source, _x)     \
{ \
    CPPUNIT_ASSERT((_message), (_x)); \
    /* TODO: check _source */ \
}

/** utility function to iterate over different kinds of items in a sync source */
static int countAnyItems(
    SyncSource &source,
    SyncItem * (SyncSource::*first)(),
    SyncItem * (SyncSource::*next)() )
{
    SyncItem *item;
    int count = 0;
#if 0
    CPPUNIT_ASSERT( !source.hasFailed() );
#endif
    SOURCE_ASSERT_NO_FAILURE( source, item = (source.*first)() );
    while ( item ) {
        count++;
        delete item;
        SOURCE_ASSERT_NO_FAILURE( source, item = (source.*next)() );
    }

    return count;
}
    
static int countNewItems( SyncSource &source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstNewItem,
        &SyncSource::getNextNewItem );
    return res;
}

static int countUpdatedItems( SyncSource &source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstUpdatedItem,
        &SyncSource::getNextUpdatedItem );
    return res;
}

static int countDeletedItems( SyncSource &source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstDeletedItem,
        &SyncSource::getNextDeletedItem );
    return res;
}

static int countItems( SyncSource &source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstItem,
        &SyncSource::getNextItem );
    return res;
}

/**
 * convenience macro for adding a test name like a function,
 * to be used inside an instance of that class
 *
 * @param _class      class which contains the function
 * @param _function   a function without parameters in that class
 */
#define ADD_TEST(_class, _function) \
    addTest(new CppUnit::TestCaller<_class>(getName() + "::" #_function, &_class::_function, *this))

/**
 * local test of one sync source and utility functions also used by
 * sync tests
 */
class LocalTests : public CppUnit::TestSuite, public CppUnit::TestFixture {
public:
    /** the client we are testing */
    ClientTest &client;

    /** configuration for the source that we are testing in that client */
    ClientTest::Config config;

    LocalTests(const std::string &name, ClientTest &cl, ClientTest::Config &co) :
        CppUnit::TestSuite(name),
        client(cl),
        config(co) {}

    /** adds the supported tests to the instance itself */
    void addTests() {
        if (config.createSourceA) {
            ADD_TEST(LocalTests, testOpen);
            if (config.insertItem) {
                ADD_TEST(LocalTests, testSimpleInsert);
            }
        }
    }
    
    /**
     * opens source and inserts the given item; can be called
     * regardless whether the data source already contains items or not
     */
    void insert(ClientTest::Config::createsource_t createSource, const char *data) {
        CPPUNIT_ASSERT(createSource);
        std::auto_ptr<SyncSource> source((*createSource)(client));
        CPPUNIT_ASSERT(source.get() != NULL);
        SOURCE_ASSERT(source, source->beginSync() == 0);
        int numItems;
        CPPUNIT_ASSERT_NO_THROW(numItems = countItems(*source));
        SyncItem item;
        item.setData(data, strlen(data));
        int status;
        SOURCE_ASSERT_NO_FAILURE(source, status = source->addItem(item));
        CPPUNIT_ASSERT(item.getKey() != NULL);
        CPPUNIT_ASSERT(strlen(item.getKey()) > 0);
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // two possible results:
        // - a new item was added
        // - the item was matched against an existing one
        CPPUNIT_ASSERT_NO_THROW(source.reset((*createSource)(client)));
        CPPUNIT_ASSERT(source.get() != NULL);
        SOURCE_ASSERT(source, source->beginSync() == 0);
        CPPUNIT_ASSERT(status == STC_OK || status == STC_CONFLICT_RESOLVED_WITH_MERGE);
        CPPUNIT_ASSERT(countItems(*source) == numItems +
                       (status == STC_CONFLICT_RESOLVED_WITH_MERGE ? 0 : 1));
        CPPUNIT_ASSERT(countNewItems(*source.get()) == 0);
        CPPUNIT_ASSERT(countUpdatedItems(*source.get()) == 0);
        CPPUNIT_ASSERT(countDeletedItems(*source.get()) == 0);
        CPPUNIT_ASSERT_NO_THROW(source.reset());
#if 0
        SyncItem *sameItem;
        SOURCE_ASSERT_NO_FAILURE(
            source,
            sameItem = source.createItem(item.getKey(), item.getState()));
        CPPUNIT_ASSERT(sameItem != NULL);
        CPPUNIT_ASSERT(!strcmp( sameItem->getKey(), item.getKey()));
        delete sameItem;
#endif
    }

    /** assumes that one element is currently inserted and updates it with the given item */
    void update(SyncSource &source, const char *data) {
    }

    /** deletes all items locally via sync source */
    void deleteAllLocal(SyncSource &source);

    // creating sync source
    void testOpen() {
        // check requirements
        CPPUNIT_ASSERT(config.createSourceA);

        std::auto_ptr<SyncSource> source((*config.createSourceA)(client));
        CPPUNIT_ASSERT(source.get() != NULL);
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

    // insert one contact without clearing the source first
    void testSimpleInsert() {
        // check requirements
        CPPUNIT_ASSERT(config.insertItem);
        CPPUNIT_ASSERT(config.createSourceA);

        insert(config.createSourceA, config.insertItem);
    }

    // delete all items
    void testLocalDeleteAll();
    // restart scanning of items
    void testIterateTwice();
    // clean database, then insert
    void testComplexInsert();
    // clean database, insert item, update it
    void testLocalUpdate();
    // complex sequence of changes
    void testChanges();
    // clean database, import file, then export again and compare
    void testImport();
    // same as testImport() with immediate delete
    void testImportDelete();
    // test change tracking with large number of items
    void testManyChanges();
};

/**
 * Tests synchronization with one or more sync sources enabled.
 * When testing multiple sources at once only the first config
 * is checked to see which tests can be executed.
 */
class SyncTests : public CppUnit::TestSuite, public CppUnit::TestFixture {
public:
    /** the client we are testing */
    ClientTest &client;

    SyncTests(const std::string &name, ClientTest &cl, std::vector<int> sourceIndices) :
        CppUnit::TestSuite(name),
        client(cl) {
        sourceArray = new int[sourceIndices.size() + 1];
        for (std::vector<int>::iterator it = sourceIndices.begin();
             it != sourceIndices.end();
             ++it) {
            ClientTest::Config config;
            client.getSourceConfig(*it, config);

            if (config.sourceName) {
                sourceArray[sources.size()] = *it;
                sources.push_back(std::pair<int,LocalTests *>(*it, new LocalTests(config.sourceName, cl, config)));
            }
        }
        sourceArray[sources.size()] = -1;
    }

    ~SyncTests() {
        for (source_it it = sources.begin();
             it != sources.end();
             ++it) {
            delete it->second;
        }
        delete [] sourceArray;
    }

    /** adds the supported tests to the instance itself */
    void addTests() {
        if (sources.size()) {
            ClientTest::Config &config(sources[0].second->config);

            ADD_TEST(SyncTests, testTwoWaySync);
            ADD_TEST(SyncTests, testSlowSync);
            ADD_TEST(SyncTests, testRefreshFromServerSync);
            ADD_TEST(SyncTests, testRefreshFromClientSync);
        }
    }

private:
    /** list with all local test classes for manipulating the sources and their index in the client */
    std::vector< std::pair<int, LocalTests *> > sources;
    typedef std::vector< std::pair<int, LocalTests *> >::iterator source_it;

    /** the indices from sources, terminated by -1 (for sync()) */
    int *sourceArray;

    enum DeleteAllMode {
        DELETE_ALL_SYNC,   /**< make sure client and server are in sync,
                              delete locally,
                              sync again */
        DELETE_ALL_REFRESH /**< delete locally, refresh server */
    };
    
    /** deletes all items locally and on server */
    void deleteAllSync(ClientTest &client, SyncSource &source, DeleteAllMode mode = DELETE_ALL_SYNC);

    // do a two-way sync without additional checks
    void testTwoWaySync() {
        sync(SYNC_TWO_WAY);
    }
    // do a slow sync without additional checks
    void testSlowSync() {
        sync(SYNC_SLOW);
    }
    // do a refresh from server sync without additional checks
    void testRefreshFromServerSync() {
        sync(SYNC_REFRESH_FROM_SERVER);
    }
    // do a refresh from client sync without additional checks
    void testRefreshFromClientSync() {
        sync(SYNC_REFRESH_FROM_CLIENT);
    }
    
    // delete all items, locally and on server using two-way sync
    void testDeleteAllSync();
    // delete all items, locally and on server using refresh-from-client sync
    void testDeleteAllRefresh();
    // test that a refresh sync of an empty server leads to an empty datatbase
    void testRefreshSemantic();
    // tests the following sequence of events:
    // - insert item
    // - delete all items
    // - insert one other item
    // - refresh from client
    // => no items should now be listed as new, updated or deleted for this client
    void testRefreshStatus();
    // test that a two-way sync copies an item from one address book into the other
    void testCopy();
    // test that a two-way sync copies updates from database to the other client,
    // using simple data commonly supported by servers
    void testUpdate();
    // test that a two-way sync copies updates from database to the other client,
    // using data that some, but not all servers support, like adding a second
    // phone number to a contact
    void testComplexUpdate();
    // test that a two-way sync deletes the copy of an item in the other database
    void testDelete();
    // test what the server does when it finds that different
    // fields of the same item have been modified
    void testMerge();
    // test what the server does when it has to execute a slow sync
    // with identical data on client and server:
    // expected behaviour is that nothing changes
    void testTwinning();
    // tests one-way sync from server:
    // - get both clients and server in sync with no items anywhere
    // - add one item on first client, copy to server
    // - add a different item on second client, one-way-from-server
    // - two-way sync with first client
    // => one item on first client, two on second
    // - delete on first client, sync that to second client
    //   via two-way sync + one-way-from-server
    // => one item left on second client (the one inserted locally)
    void testOneWayFromServer();
    // tests one-way sync from client:
    // - get both clients and server in sync with no items anywhere
    // - add one item on first client, copy to server
    // - add a different item on second client, one-way-from-client
    // - two-way sync with first client
    // => two items on first client, one on second
    // - delete on second client, sync that to first client
    //   via one-way-from-client, two-way
    // => one item left on first client (the one inserted locally)
    void testOneWayFromClient();
    // creates several items, transmits them back and forth and
    // then compares which of them have been preserved
    void testItems();
    // tests the following sequence of events:
    // - both clients in sync with server
    // - client 1 adds item
    // - client 1 updates the same item
    // - client 2 gets item (depending on server, might be flagged as update)
    // See http://forge.objectweb.org/tracker/?func=detail&atid=100096&aid=305018&group_id=96
    void testAddUpdate();
    // test copying with maxMsg and no large object support
    void testMaxMsg() {
        doVarSizes(true, false, NULL);
    }
    // test copying with maxMsg and large object support
    void testLargeObject() {
        doVarSizes(true, true, NULL);
    }
    // test copying with maxMsg and large object support using explicit "bin" encoding
    void testLargeObjectBin() {
        doVarSizes(true, true, "bin");
    }
    // test copying with maxMsg and large object support using B64 encoding
    void testLargeObjectEncoded() {
        doVarSizes(true, true, "b64");
    }
    
    //
    // stress tests: execute some of the normal operations,
    // but with large number of artificially generated items
    //

    // two-way sync with clean client/server,
    // followed by slow sync and comparison
    // via second client
    void testManyItems();


    /**
     * implements testMaxMsg(), testLargeObject(), testLargeObjectEncoded()
     * using a sequence of items with varying sizes
     */
    void doVarSizes(bool withMaxMsgSize,
                    bool withLargeObject,
                    const char *encoding);


    void sync(SyncMode syncMode,
              long maxMsgSize = 0,
              long maxObjSize = 0,
              bool loSupport = false,
              const char *encoding = NULL) {
        int res = 0;
        std::string logfile = getCurrentTest() + ".client.log";
        simplifyFilename(logfile);

        remove(logfile.c_str());
        LOG.setLogName(logfile.c_str());
        LOG.reset();

        try {
            res = client.sync(sourceArray,
                              syncMode,
                              maxMsgSize,
                              loSupport,
                              encoding);
        }  catch(...) {
            /* TODO: EvolutionSyncSource::handleException(); */
            res = 1;
        }

#if 0
        // make a copy of the server's log (if found), then truncate it
        if (m_serverLog.size()) {
            int fd = open( m_serverLog.c_str(), O_RDWR );
            
            if (fd >= 0) {
                // let the server finish
                sleep(m_syncDelay);

                string serverLog = logfile;
                size_t pos = serverLog.find( "client" );
                if (pos != serverLog.npos ) {
                    serverLog.replace( pos, 6, "server" );
                } else {
                    serverLog += ".server.log";
                }
                string cmd = string("cp ") + m_serverLog + " " + serverLog;
                system( cmd.c_str() );
                ftruncate( fd, 0 );
            } else {
                perror( m_serverLog.c_str() );
            }
        } else {
            // let the server finish
            sleep(m_syncDelay);
        }
#endif

        CPPUNIT_ASSERT( !res );
    }
};


/** generates tests on demand based on what the client supports */
class ClientTestFactory : public CppUnit::TestFactory {
public:
    ClientTestFactory(ClientTest &c) :
        client(c) {}

    virtual CppUnit::Test *makeTest() {
        int source;
        CppUnit::TestSuite *alltests = new CppUnit::TestSuite("Client");
        CppUnit::TestSuite *tests;
        
        // create local source tests
        tests = new CppUnit::TestSuite(alltests->getName() + "::Source");
        for (source=0; source < client.getNumSources(); source++) {
            ClientTest::Config config;
            client.getSourceConfig(source, config);
            if (config.sourceName) {
                LocalTests *sourcetests =
                    new LocalTests(tests->getName() + "::" + config.sourceName, client, config);
                sourcetests->addTests();
                tests->addTest(sourcetests);
            }
        }
        alltests->addTest(tests);
        tests = NULL;

        // create sync tests with just one source
        tests = new CppUnit::TestSuite(alltests->getName() + "::Sync");
        for (source=0; source < client.getNumSources(); source++) {
            ClientTest::Config config;
            client.getSourceConfig(source, config);
            if (config.sourceName) {
                std::vector<int> sources;
                sources.push_back(source);
                SyncTests *synctests =
                    new SyncTests(tests->getName() + "::" + config.sourceName, client, sources);
                synctests->addTests();
                tests->addTest(synctests);
            }
        }
        alltests->addTest(tests);
        tests = NULL;
        
        return alltests;
    }

private:
    ClientTest &client;
};

#endif // ENABLE_INTEGRATION_TESTS

void ClientTest::registerTests()
{
#ifdef ENABLE_INTEGRATION_TESTS
    factory = (void *)new ClientTestFactory(*this);
    CppUnit::TestFactoryRegistry::getRegistry().registerFactory((CppUnit::TestFactory *)factory);
#endif
}

ClientTest::ClientTest() :
    factory(NULL)
{
}

ClientTest::~ClientTest()
{
    if(factory) {
#ifdef ENABLE_INTEGRATION_TESTS
        CppUnit::TestFactoryRegistry::getRegistry().unregisterFactory((CppUnit::TestFactory *)factory);
        delete (CppUnit::TestFactory *)factory;
        factory = NULL;
#endif
    }
}
