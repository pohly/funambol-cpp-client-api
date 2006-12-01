/*
 * Copyright (C) 2005-2006 Funambol
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

#ifndef INCL_TESTSYNCCLIENT
#define INCL_TESTSYNCCLIENT

#include "spds/SyncSource.h"

/**
 * This is the interface expected by the testing framework for sync
 * clients.  It defines several methods that a derived class must
 * implement if it wants to use that framework. Note that this class
 * itself is not derived from SyncClient. This gives a user of this
 * framework the freedom to implement it in two different ways:
 * - implement a class derived from both SyncClient and ClientTest
 * - add testing to an existing subclass of SyncClient by implementing
 *   a ClientTest which uses that subclass
 *
 * The client is expected to support change tracking for multiple
 * servers. Although the framework always always tests against the
 * same server, for most tests it is necessary to access the database
 * without affecting the next synchronization with the server. This is
 * done by asking the client for another sync source which it must
 * create in a suitable way - pretty much as if the client was
 * synchronized against different server.
 *
 * Furthermore the client is expected to support multiple data sources
 * of the same kind, f.i. two different address books. This is used to
 * test full client A <-> server <-> client B synchronizations in some
 * tests or to check server modifications done by client A with a
 * synchronization against client B. In those tests client A is mapped
 * to the first data source and client B to the second one.
 *
 * Handling configuration and creating classes is entirely done by the
 * subclass of ClientTest, the frameworks makes no assumptions
 * about how this is done. Instead it queries the ClientTest for
 * properties (like available sync sources) and then creates several
 * tests.
 */
class ClientTest {
  public:
    ClientTest();
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
         */
        typedef SyncSource *(*createsource_t)(ClientTest &client);

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
         * Time the server needs before it correctly handles another
         * connection. Some servers use time stamps to track modifications
         * and get confused when connections are made to quickly after
         * another.
         */
        int serverDelaySeconds;

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
     * Execute a synchronization with the selected sync sources
     * and the selected synchronization options. The log file
     * in LOG has been set up already for the synchronization run
     * and should not be changed by the client.
     *
     * @param sources      a -1 terminated array of sync source indices
     * @param syncMode     the synchronization mode to be used
     * @param maxMsgSize   >0: enable the maximum message size, else disable it
     * @param maxObjSize   same as maxMsgSize for maximum object size
     * @param encoding     if non-NULL, then let client library transform all items
     *                     into this format
     *
     * @return - 0 on success, an error otherwise
     */
    virtual int sync(
        const int *sources,
        SyncMode syncMode,
        long maxMsgSize = 0,
        long maxObjSize = 0,
        bool loSupport = false,
        const char *encoding = NULL) = 0;

  private:
    /**
     * really a CppUnit::TestFactory, but declared as void * to avoid
     * dependencies on the CPPUnit header files: created by
     * registerTests() and remains valid until the client is deleted
     */
    void *factory;
};

#endif
