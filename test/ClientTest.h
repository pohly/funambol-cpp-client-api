/*
 * Copyright (C) 2005-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

#ifndef INCL_TESTSYNCCLIENT
#define INCL_TESTSYNCCLIENT
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include <string>
#include "spds/SyncSource.h"
#include "spds/SyncReport.h"

#ifdef ENABLE_INTEGRATION_TESTS

/**
 * This class encapsulates logging and checking of a SyncReport.
 * When constructed with default parameters, no checking will be done.
 * Otherwise the sync report has to contain exactly the expected result.
 * When multiple sync sources are active, @b all of them have to behave
 * alike (which is how the tests are constructed).
 *
 * No item is ever supposed to fail.
 */
class CheckSyncReport {
  public:
    CheckSyncReport(int clAdded = -1, int clUpdated = -1, int clDeleted = -1,
                    int srAdded = -1, int srUpdated = -1, int srDeleted = -1) :
        clientAdded(clAdded),
        clientUpdated(clUpdated),
        clientDeleted(clDeleted),
        serverAdded(srAdded),
        serverUpdated(srUpdated),
        serverDeleted(srDeleted)
        {}

    const int clientAdded, clientUpdated, clientDeleted,
        serverAdded, serverUpdated, serverDeleted;

    /**
     * checks that the sync completed as expected and throws
     * CPPUnit exceptions if something is wrong
     *
     * @param res     return code from SyncClient::sync()
     * @param report  the sync report stored in the SyncClient
     */
    virtual void check(int res, SyncReport &report) const;
};

/**
 * This is the interface expected by the testing framework for sync
 * clients.  It defines several methods that a derived class must
 * implement if it wants to use that framework. Note that this class
 * itself is not derived from SyncClient. This gives a user of this
 * framework the freedom to implement it in two different ways:
 * - implement a class derived from both SyncClient and ClientTest
 * - add testing of an existing subclass of SyncClient by implementing
 *   a ClientTest which uses that subclass
 *
 * The client is expected to support change tracking for multiple
 * servers. Although the framework always always tests against the
 * same server, for most tests it is necessary to access the database
 * without affecting the next synchronization with the server. This is
 * done by asking the client for two different sync sources via
 * Config::createSourceA and Config::createSourceB which have to
 * create them in a suitable way - pretty much as if the client was
 * synchronized against different server. A third, different change
 * tracking is needed for real synchronizations of the data.
 *
 * Furthermore the client is expected to support multiple data sources
 * of the same kind, f.i. two different address books. This is used to
 * test full client A <-> server <-> client B synchronizations in some
 * tests or to check server modifications done by client A with a
 * synchronization against client B. In those tests client A is mapped
 * to the first data source and client B to the second one.
 *
 * Finally the SyncSource API is used in slightly different ways which
 * go beyond what is normally expected from a SyncSource implementation:
 * - beginSync() may be called without setting a sync mode:
 *   when SyncSource::getSyncMode() returns SYNC_NONE the source is
 *   expected to make itself ready to iterate over all, new, updated and
 *   deleted items
 * - items may be added via SyncSource::addItem() with a type of "raw":
 *   this implies that the type is the one used for items in the
 *   ClientTest::Config below
 *
 * Handling configuration and creating classes is entirely done by the
 * subclass of ClientTest, the frameworks makes no assumptions
 * about how this is done. Instead it queries the ClientTest for
 * properties (like available sync sources) and then creates several
 * tests.
 */
class ClientTest {
  public:
    ClientTest(int serverSleepSec = 0, const std::string &serverLog= "");
    virtual ~ClientTest();

    /**
     * This is the only function provided by ClientTest itself:
     * it registers tests using this instance of ClientTest for
     * later use during a test run.
     *
     * Theinstance must remain valid until after the tests were
     * run. To run them use a separate test runner, like the one from
     * client-test-main.cpp.
     */
    virtual void registerTests();

    /**
     * utility function for dumping items which are C strings with blank lines as separator
     */
    static int dump(ClientTest &client, SyncSource &source, const char *file);

    /**
     * utility function for importing items with blank lines as separator
     */
    static int import(ClientTest &client, SyncSource &source, const char *file);

    /**
     * utility function for comparing vCard and iCal files with the external
     * synccompare.pl Perl script
     */
    static bool compare(ClientTest &client, const char *fileA, const char *fileB);

    struct Config;

    /**
     * A derived class can use this call to get default test
     * cases, but still has to add callbacks which create sources
     * and execute a sync session.
     *
     * Some of the test cases are compiled into the library, other
     * depend on the auxiliary files from the "test" directory.
     * Currently supported types:
     * - vcard30 = vCard 3.0 contacts
     * - vcard21 = vCard 2.1 contacts
     * - ical20 = iCal 2.0 events
     * - vcal10 = vCal 1.0 events
     * - itodo20 = iCal 2.0 tasks
     */
    static void getTestData(const char *type, Config &config);

    /**
     * Information about a data source. For the sake of simplicity all
     * items pointed to are owned by the ClientTest and must
     * remain valid throughout a test session. Not setting a pointer
     * is okay, but it will disable all tests that need the
     * information.
     */
    struct Config {
        /**
         * The name is used in test names and has to be set.
         */
        const char *sourceName;

        /**
         * A default URI to be used when creating a client config.
         */
        const char *uri;

        /**
         * A member function of a subclass which is called to create a
         * sync source referencing the data. This is used in tests of
         * the SyncSource API itself as well as in tests which need to
         * modify or check the data sources used during synchronization.
         *
         * The test framework will call beginSync() and then some of
         * the functions it wants to test. After a successful test it
         * will call endSync() which is then expected to store all
         * changes persistently. Creating a sync source again
         * with the same call should not report any
         * new/updated/deleted items until such changes are made via
         * another sync source.
         *
         * The instance will be deleted by the caller. Because this
         * may be in the error case or in an exception handler,
         * the sync source's desctructor should not thow exceptions.
         *
         * @param client    the same instance to which this config belongs
         * @param source    index of the data source (from 0 to ClientTest::getNumSources() - 1)
         * @param isSourceA true if the requested SyncSource is the first one accessing that
         *                  data, otherwise the second
         */
        typedef SyncSource *(*createsource_t)(ClientTest &client, int source, bool isSourceA);

        /**
         * Creates a sync source which references the primary database;
         * it may report the same changes as the sync source used during
         * sync tests.
         */
        createsource_t createSourceA;

        /**
         * A second sync source also referencing the primary data
         * source, but configured so that it tracks changes
         * independently from the the primary sync source.
         *
         * In local tests the usage is like this:
         * - add item via first SyncSource
         * - iterate over new items in second SyncSource
         * - check that it lists the added item
         *
         * In tests with a server the usage is:
         * - do a synchronization with the server
         * - iterate over items in second SyncSource
         * - check that the total number and number of
         *   added/updated/deleted items is as expected
         */
        createsource_t createSourceB;

        /**
         * The framework can generate vCard and vCalendar/iCalendar items
         * automatically by copying a template item and modifying certain
         * properties.
         *
         * This is the template for these automatically generated items.
         */
        const char *templateItem;

         /**
         * This is a colon (:) separated list of properties which need
         * to be modified in templateItem.
         */
        const char *uniqueProperties;

        /**
         * the number of items to create during stress tests
         */
        int numItems;

        /**
         * This is a single property in templateItem which can be extended
         * to increase the size of generated items.
         */
        const char *sizeProperty;

        /**
         * A very simple item that is inserted during basic tests. Ideally
         * it only contains properties supported by all servers.
         */
        const char *insertItem;

        /**
         * A slightly modified version of insertItem. If the source has UIDs
         * embedded into the item data, then both must have the same UID.
         * Again all servers should better support these modified properties.
         */
        const char *updateItem;

        /**
         * A more heavily modified version of insertItem. Same UID if necessary,
         * but can test changes to items only supported by more advanced
         * servers.
         */
        const char *complexUpdateItem;

        /**
         * To test merge conflicts two different updates of insertItem are
         * needed. This is the first such update.
         */
        const char *mergeItem1;

        /**
         * The second merge update item. To avoid true conflicts it should
         * update different properties than mergeItem1, but even then servers
         * usually have problems perfectly merging items. Therefore the
         * test is run without expecting a certain merge result.
         */
        const char *mergeItem2;

        /**
         * called to dump all items into a file, required by tests which need
         * to compare items
         *
         * ClientTest::dump can be used: it will simply dump all items of the source
         * with a blank line as separator.
         *
         * @param source     sync source A already created and with beginSync() called
         * @param file       a file name
         * @return error code, 0 for success
         */
        int (*dump)(ClientTest &client, SyncSource &source, const char *file);

        /**
         * import test items: which these are is determined entirely by
         * the implementor, but tests work best if several complex items are
         * imported
         *
         * ClientTest::import can be used if the file contains items separated by
         * empty lines.
         *
         * @param source     sync source A already created and with beginSync() called
         * @param file       the name of the file to import
         * @return error code, 0 for success
         */
        int (*import)(ClientTest &client, SyncSource &source, const char *file);

        /**
         * a function which compares two files with items in the format used by "dump"
         *
         * @param fileA      first file name
         * @param fileB      second file name
         * @return true if the content of the files is considered equal
         */
        bool (*compare)(ClientTest &client, const char *fileA, const char *fileB);

        /**
         * a file with test cases in the format expected by import and compare
         */
        const char *testcases;

        /**
         * the item type normally used by the source (not used by the tests
         * themselves; client-test.cpp uses it to initialize source configs)
         */
        const char *type;
    };

    /**
     * Data sources are enumbered from 0 to n-1 for the purpose of
     * testing. This call returns n.
     */
    virtual int getNumSources() = 0;

    /**
     * Called to fill the given test source config with information
     * about a sync source identified by its index. It's okay to only
     * fill in the available pieces of information and set everything
     * else to zero.
     */
    virtual void getSourceConfig(int source, Config &config) = 0;

    /**
     * The instance to use as second client. Returning NULL disables
     * all checks which require a second client. The returned pointer
     * must remain valid throughout the life time of the tests.
     *
     * The second client must be configured to access the same server
     * and have data sources which match the ones from the primary
     * client.
     */
    virtual ClientTest *getClientB() = 0;

    /**
     * Returning true enables tests which only work if the server is
     * a Funambol server which supports the "b64" encoding of items
     * on the transport level.
     */
    virtual bool isB64Enabled() = 0;

    /**
     * Execute a synchronization with the selected sync sources
     * and the selected synchronization options. The log file
     * in LOG has been set up already for the synchronization run
     * and should not be changed by the client.
     *
     * @param activeSources a -1 terminated array of sync source indices
     * @param syncMode     the synchronization mode to be used
     * @param checkReport  has to be called after a successful or unsuccessful sync,
     *                     will dump the report and (optionally) check the result;
     *                     beware, the later may throw exceptions inside CPPUNIT macros
     * @param maxMsgSize   >0: enable the maximum message size, else disable it
     * @param maxObjSize   same as maxMsgSize for maximum object size
     * @param loSupport    if TRUE, then the sync is expected to enable Large Object support
     * @param encoding     if non-empty, then let client library transform all items
     *                     into this format (guaranteed to be not NULL)
     *
     * @return return code of SyncClient::sync()
     */
    virtual int sync(
        const int *activeSources,
        SyncMode syncMode,
        const CheckSyncReport &checkReport,
        long maxMsgSize = 0,
        long maxObjSize = 0,
        bool loSupport = false,
        const char *encoding = "") = 0;

    /**
     * This is called after successful sync() calls (res == 0) as well
     * as after unsuccessful ones (res != 1). The default implementation
     * sleeps for the number of seconds specified when constructing this
     * instance and copies the server log if one was named.
     *
     * @param res       result of sync()
     * @param logname   base name of the current sync log (without ".client.[AB].log" suffix)
     */
    virtual void postSync(int res, const std::string &logname);

  protected:
    /**
     * time to sleep in postSync()
     */
    int serverSleepSeconds;

    /**
     * server log file which is copied by postSync() and then
     * truncated (Unix only, Windows does not allow such access
     * to an open file)
     */
    std::string serverLogFileName;

  private:
    /**
     * really a CppUnit::TestFactory, but declared as void * to avoid
     * dependencies on the CPPUnit header files: created by
     * registerTests() and remains valid until the client is deleted
     */
    void *factory;
};

/** assert equality, include string in message if unequal */
#define CLIENT_TEST_EQUAL( _prefix, \
                           _expected, \
                           _actual ) \
    CPPUNIT_ASSERT_EQUAL_MESSAGE( std::string(_prefix) + ": " + #_expected + " == " + #_actual, \
                                  _expected, \
                                  _actual )

#endif // ENABLE_INTEGRATION_TESTS

/** @} */
/** @endcond */
#endif // INCL_TESTSYNCCLIENT
