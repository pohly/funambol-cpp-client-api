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
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iostream>

/** execute _x and then check the status of the _source pointer */
#define SOURCE_ASSERT_NO_FAILURE(_source, _x) \
{ \
    CPPUNIT_ASSERT_NO_THROW(_x); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}

/** check _x for true and then the status of the _source pointer */
#define SOURCE_ASSERT(_source, _x) \
{ \
    CPPUNIT_ASSERT(_x); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}

/** check that _x evaluates to a specific value and then the status of the _source pointer */
#define SOURCE_ASSERT_EQUAL(_source, _value, _x) \
{ \
    CPPUNIT_ASSERT_EQUAL(_value, _x); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}

/** same as SOURCE_ASSERT() with a specific failure message */
#define SOURCE_ASSERT_MESSAGE(_message, _source, _x)     \
{ \
    CPPUNIT_ASSERT_MESSAGE((_message), (_x)); \
    CPPUNIT_ASSERT((_source) && (!(_source)->getReport() || (_source)->getReport()->getState() != SOURCE_ERROR)); \
}

/** utility function to iterate over different kinds of items in a sync source */
static int countAnyItems(
    SyncSource *source,
    SyncItem * (SyncSource::*first)(),
    SyncItem * (SyncSource::*next)() )
{
    SyncItem *item;
    int count = 0;

    CPPUNIT_ASSERT(source);
    CPPUNIT_ASSERT(!source->getReport() || source->getReport()->getState() != SOURCE_ERROR);
    SOURCE_ASSERT_NO_FAILURE(source, item = (source->*first)());
    while ( item ) {
        count++;
        delete item;
        SOURCE_ASSERT_NO_FAILURE(source, item = (source->*next)());
    }

    return count;
}
    
static int countNewItems( SyncSource *source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstNewItem,
        &SyncSource::getNextNewItem );
    return res;
}

static int countUpdatedItems( SyncSource *source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstUpdatedItem,
        &SyncSource::getNextUpdatedItem );
    return res;
}

static int countDeletedItems( SyncSource *source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstDeletedItem,
        &SyncSource::getNextDeletedItem );
    return res;
}

static int countItems( SyncSource *source )
{
    int res = countAnyItems(
        source,
        &SyncSource::getFirstItem,
        &SyncSource::getNextItem );
    return res;
}

static void importItem(SyncSource *source, std::string &data)
{
    CPPUNIT_ASSERT(source);
    if (data.size()) {
        SyncItem item;
        item.setData( data.c_str(), (long)data.size() + 1 );
        item.setDataType( TEXT("raw") );
        int status;
        SOURCE_ASSERT_NO_FAILURE(source, status = source->addItem(item));
        CPPUNIT_ASSERT(status == STC_OK || status == STC_ITEM_ADDED);
        CPPUNIT_ASSERT(item.getKey() != 0);
        CPPUNIT_ASSERT(wcslen(item.getKey()) > 0);
    }
}

/**
 * helper class to encapsulate ClientTest::Config::createsource_t
 * pointer and the corresponding parameters
 */
class CreateSource {
public:
    CreateSource(ClientTest::Config::createsource_t createSourceParam, ClientTest &clientParam, int sourceParam, bool isSourceAParam) :
        createSource(createSourceParam),
        client(clientParam),
        source(sourceParam),
        isSourceA(isSourceAParam) {}

    SyncSource *operator() () {
        CPPUNIT_ASSERT(createSource);
        return createSource(client, source, isSourceA);
    }

    const ClientTest::Config::createsource_t createSource;
    ClientTest &client;
    const int source;
    const bool isSourceA;
};


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

    /** number of the source we are testing in that client */
    const int source;
    
    /** configuration that corresponds to source */
    const ClientTest::Config config;

    /** helper funclets to create sources */
    CreateSource createSourceA, createSourceB;
    
    LocalTests(const std::string &name, ClientTest &cl, int sourceParam, ClientTest::Config &co) :
        CppUnit::TestSuite(name),
        client(cl),
        source(sourceParam),
        config(co),
        createSourceA(co.createSourceA, cl, sourceParam, true),
        createSourceB(co.createSourceB, cl, sourceParam, false)
        {}

    /** adds the supported tests to the instance itself */
    void addTests() {
        if (config.createSourceA) {
            ADD_TEST(LocalTests, testOpen);
            ADD_TEST(LocalTests, testIterateTwice);
            if (config.insertItem) {
                ADD_TEST(LocalTests, testSimpleInsert);
                ADD_TEST(LocalTests, testLocalDeleteAll);
                ADD_TEST(LocalTests, testComplexInsert);
                
                if (config.updateItem) {
                    ADD_TEST(LocalTests, testLocalUpdate);
 
                    if (config.createSourceB) {
                        ADD_TEST(LocalTests, testChanges);
                    }
                }
 
                if (config.import &&
                    config.dump &&
                    config.compare &&
                    config.testcases) {
                    ADD_TEST(LocalTests, testImport);
                    ADD_TEST(LocalTests, testImportDelete);
                }
                
                if (config.templateItem &&
                    config.uniqueProperties) {
                    ADD_TEST(LocalTests, testManyChanges);
                }
            }
        }
    }
    
    /**
     * opens source and inserts the given item; can be called
     * regardless whether the data source already contains items or not
     */
    void insert(CreateSource createSource, const char *data) {
        // create source
        std::auto_ptr<SyncSource> source(createSource());
        CPPUNIT_ASSERT(source.get() != 0);

        // count number of already existing items
        SOURCE_ASSERT(source.get(), source->beginSync() == 0);
        int numItems;
        CPPUNIT_ASSERT_NO_THROW(numItems = countItems(source.get()));
        SyncItem item;
        item.setData(data, (long)strlen(data));
        int status;
        SOURCE_ASSERT_NO_FAILURE(source.get(), status = source->addItem(item));
        CPPUNIT_ASSERT(item.getKey() != 0);
        CPPUNIT_ASSERT(wcslen(item.getKey()) > 0);
        SOURCE_ASSERT(source.get(), source->endSync() == 0);

        // delete source again
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // two possible results:
        // - a new item was added
        // - the item was matched against an existing one
        CPPUNIT_ASSERT_NO_THROW(source.reset(createSource()));
        CPPUNIT_ASSERT(source.get() != 0);
        SOURCE_ASSERT(source.get(), source->beginSync() == 0);
        CPPUNIT_ASSERT(status == STC_OK || status == STC_ITEM_ADDED || status == STC_CONFLICT_RESOLVED_WITH_MERGE);
        CPPUNIT_ASSERT_EQUAL(numItems + (status == STC_CONFLICT_RESOLVED_WITH_MERGE ? 0 : 1),
                             countItems(source.get()));
        CPPUNIT_ASSERT(countNewItems(source.get()) == 0);
        CPPUNIT_ASSERT(countUpdatedItems(source.get()) == 0);
        CPPUNIT_ASSERT(countDeletedItems(source.get()) == 0);
        SOURCE_ASSERT(source.get(), source->endSync() == 0 );
        CPPUNIT_ASSERT_NO_THROW(source.reset());

#if 0
        /* source.createItem() is a SyncEvolution extension which cannot be used here */
        SyncItem *sameItem;
        SOURCE_ASSERT_NO_FAILURE(
            source,
            sameItem = source.createItem(item.getKey(), item.getState()));
        CPPUNIT_ASSERT(sameItem != 0);
        CPPUNIT_ASSERT(!strcmp( sameItem->getKey(), item.getKey()));
        delete sameItem;
#endif
    }

    /** assumes that exactly one element is currently inserted and updates it with the given item */
    void update(CreateSource createSource, const char *data) {
        CPPUNIT_ASSERT(createSource.createSource);
        CPPUNIT_ASSERT(data);

        // create source
        std::auto_ptr<SyncSource> source(createSource());
        CPPUNIT_ASSERT(source.get() != 0);
        SOURCE_ASSERT(source.get(), source->beginSync() == 0);

        // get existing item, then update it
        SOURCE_ASSERT(source.get(), source->beginSync() == 0 );
        std::auto_ptr<SyncItem> item;
        SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getFirstItem()) );
        CPPUNIT_ASSERT(item.get());
        item->setData(data, (long)strlen(data) + 1);
        SOURCE_ASSERT_EQUAL(source.get(), (int)STC_OK, source->updateItem(*item));
        SOURCE_ASSERT(source.get(), source->endSync() == 0);
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // check that the right changes are reported when reopening the source
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSource()));
        SOURCE_ASSERT(source.get(), source->beginSync() == 0 );
        CPPUNIT_ASSERT_EQUAL(1, countItems(source.get()));
        CPPUNIT_ASSERT_EQUAL(0, countNewItems(source.get()));
        CPPUNIT_ASSERT_EQUAL(0, countUpdatedItems(source.get()));
        CPPUNIT_ASSERT_EQUAL(0, countDeletedItems(source.get()));
        std::auto_ptr<SyncItem> modifiedItem;
        SOURCE_ASSERT_NO_FAILURE(source.get(), modifiedItem.reset(source->getFirstItem()) );
        CPPUNIT_ASSERT(modifiedItem.get());
        CPPUNIT_ASSERT( wcslen( item->getKey() ) );
        CPPUNIT_ASSERT( !wcscmp( item->getKey(), modifiedItem->getKey() ) );
    }

    /** deletes all items locally via sync source */
    void deleteAll(CreateSource createSource) {
        CPPUNIT_ASSERT(createSource.createSource);

        // create source
        std::auto_ptr<SyncSource> source(createSource());
        CPPUNIT_ASSERT(source.get() != 0);
        SOURCE_ASSERT(source.get(), source->beginSync() == 0);

        // delete all items
        std::auto_ptr<SyncItem> item;
        SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getFirstItem()));
        while (item.get()) {
            SOURCE_ASSERT_EQUAL(source.get(), (int)STC_OK, source->deleteItem(*item));
            SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getNextItem()));
        }
        SOURCE_ASSERT(source.get(), source->endSync() == 0);
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // check that all items are gone
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSource()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_MESSAGE(
            "should be empty now",
            source.get(),
            countItems(source.get()) == 0);
        CPPUNIT_ASSERT_EQUAL( 0, countNewItems(source.get()) );
        CPPUNIT_ASSERT_EQUAL( 0, countUpdatedItems(source.get()) );
        CPPUNIT_ASSERT_EQUAL( 0, countDeletedItems(source.get()) );
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

    /**
     * takes two databases, exports them,
     * then compares them using synccompare
     *
     * @param refFile      existing file with source reference items, NULL uses a dump of sync source A instead
     * @param copy         a sync source which contains the copied items, begin/endSync will be called
     * @param raiseAssertion raise assertion if comparison yields differences (defaults to true)
     */
    void compareDatabases(const char *refFile, SyncSource &copy, bool raiseAssert = true) {
        CPPUNIT_ASSERT(config.dump);

        std::string sourceFile, copyFile;
        
        if (refFile) {
            sourceFile = refFile;
        } else {
            sourceFile = getCurrentTest() + ".source.test.dat";
            simplifyFilename(sourceFile);
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceA()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, config.dump(client, *source.get(), sourceFile.c_str()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }

        copyFile = getCurrentTest() + ".copy.test.dat";
        simplifyFilename(copyFile);
        SOURCE_ASSERT_EQUAL(&copy, 0, copy.beginSync());
        SOURCE_ASSERT_EQUAL(&copy, 0, config.dump(client, copy, copyFile.c_str()));
        SOURCE_ASSERT_EQUAL(&copy, 0, copy.endSync());

        CPPUNIT_ASSERT(config.compare(client, sourceFile.c_str(), copyFile.c_str()));
    }
    
    /**
     * insert artificial items, number of them determined by TEST_EVOLUTION_NUM_ITEMS
     * unless passed explicitly
     *
     * @param config          determines which client is modified
     * @param startIndex      IDs are generated starting with this value
     * @param numItems        number of items to be inserted if non-null, otherwise TEST_EVOLUTION_NUM_ITEMS is used
     * @param size            minimum size for new items
     * @return number of items inserted
     */
    int insertManyItems(CreateSource createSource, int startIndex = 1, int numItems = 0, int size = -1) {
        CPPUNIT_ASSERT(config.templateItem);
        CPPUNIT_ASSERT(config.uniqueProperties);

        std::auto_ptr<SyncSource> source;
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceA()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        CPPUNIT_ASSERT(startIndex > 1 || !countItems(source.get()));

        int firstIndex = startIndex;
        if (firstIndex < 0) {
            firstIndex = 1;
        }
        int lastIndex = firstIndex + (numItems >= 1 ? numItems : config.numItems) - 1;
        for (int item = firstIndex; item <= lastIndex; item++) {
            std::string data = config.templateItem;
            std::stringstream prefix;

            prefix << std::setfill('0') << std::setw(3) << item << " ";

            
            const char *prop = config.uniqueProperties;
            const char *nextProp;
            while (*prop) {
                std::string curProp;
                nextProp = strchr(prop, ':');
                if (!nextProp) {
                    curProp = prop;
                } else {
                    curProp = std::string(prop, 0, nextProp - prop);
                }

                std::string property;
                // property is expected to not start directly at the
                // beginning
                property = "\n";
                property += curProp;
                property += ":";
                size_t off = data.find(property);
                if (off != data.npos) {
                    data.insert(off + property.size(), prefix.str());
                }

                if (!nextProp) {
                    break;
                }
                prop = nextProp + 1;
            }
            if (size > 0 && (int)data.size() < size) {
                int additionalBytes = size - (int)data.size();
                int added = 0;

                CPPUNIT_ASSERT(config.sizeProperty);
                
                /* stuff the item so that it reaches at least that size */
                size_t off = data.find(config.sizeProperty);
                CPPUNIT_ASSERT(off != data.npos);
                std::stringstream stuffing;
                while(added < additionalBytes) {
                    int linelen = 0;
                   
                    while(added + 4 < additionalBytes &&
                          linelen < 60) {
                        stuffing << 'x';
                        added++;
                        linelen++;
                    }
                    stuffing << "x\\nx";
                    added += 4;
                }
                data.insert(off + 1 + strlen(config.sizeProperty), stuffing.str());
            }
            
            importItem(source.get(), data);
            data = "";
        }

        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
        return lastIndex - firstIndex + 1;
    }
    
    // creating sync source
    void testOpen() {
        // check requirements
        CPPUNIT_ASSERT(config.createSourceA);

        std::auto_ptr<SyncSource> source(createSourceA());
        CPPUNIT_ASSERT(source.get() != 0);
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

    // restart scanning of items
    void testIterateTwice() {
        // check requirements
        CPPUNIT_ASSERT(config.createSourceA);

        // open source
        std::auto_ptr<SyncSource> source(createSourceA());
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_MESSAGE(
            "iterating twice should produce identical results",
            source.get(),
            countItems(source.get()) == countItems(source.get()));
    }

    // insert one contact without clearing the source first
    void testSimpleInsert() {
        // check requirements
        CPPUNIT_ASSERT(config.insertItem);
        CPPUNIT_ASSERT(config.createSourceA);

        insert(createSourceA, config.insertItem);
    }

    // delete all items
    void testLocalDeleteAll() {
        // check requirements
        CPPUNIT_ASSERT(config.insertItem);
        CPPUNIT_ASSERT(config.createSourceA);

        // make sure there is something to delete, then delete again
        insert(createSourceA, config.insertItem);
        deleteAll(createSourceA);
    }

    // clean database, then insert
    void testComplexInsert() {
        testLocalDeleteAll();
        testSimpleInsert();
        testIterateTwice();
    }

    // clean database, insert item, update it
    void testLocalUpdate() {
        // check additional requirements
        CPPUNIT_ASSERT(config.updateItem);
        
        testLocalDeleteAll();
        testSimpleInsert();
        update(createSourceA, config.updateItem);
    }
        
    // complex sequence of changes
    void testChanges() {
        // check additional requirements
        CPPUNIT_ASSERT(config.createSourceB);

        testLocalDeleteAll();
        testSimpleInsert();

        // clean changes in sync source B by creating and closing it
        std::auto_ptr<SyncSource> source;
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // no new changes now
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
        std::auto_ptr<SyncItem> item;
        SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getFirstItem()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // delete item again via sync source A
        deleteAll(createSourceA);
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
        std::auto_ptr<SyncItem> deletedItem;
        SOURCE_ASSERT_NO_FAILURE(source.get(), deletedItem.reset(source->getFirstDeletedItem()));
        CPPUNIT_ASSERT( wcslen( item->getKey() ) );
        CPPUNIT_ASSERT( wcslen( deletedItem->getKey() ) );
        CPPUNIT_ASSERT( !wcscmp( item->getKey(), deletedItem->getKey() ) );
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // insert another item via sync source A
        testSimpleInsert();
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
        SOURCE_ASSERT_NO_FAILURE(source.get(), item.reset(source->getFirstItem()));
        std::auto_ptr<SyncItem> newItem;
        SOURCE_ASSERT_NO_FAILURE(source.get(), newItem.reset(source->getFirstNewItem()));
        CPPUNIT_ASSERT( wcslen( item->getKey() ) );
        CPPUNIT_ASSERT( wcslen( newItem->getKey() ) );
        CPPUNIT_ASSERT( !wcscmp( item->getKey(), newItem->getKey() ) );
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // update item via sync source A
        update(createSourceA, config.updateItem);
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 1, countUpdatedItems(source.get()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
        std::auto_ptr<SyncItem> updatedItem;
        SOURCE_ASSERT_NO_FAILURE(source.get(), updatedItem.reset(source->getFirstUpdatedItem()));
        CPPUNIT_ASSERT( wcslen( item->getKey() ) );
        CPPUNIT_ASSERT( wcslen( updatedItem->getKey() ) );
        CPPUNIT_ASSERT( !wcscmp( item->getKey(), updatedItem->getKey() ) );
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

    // clean database, import file, then export again and compare
    void testImport() {
        // check additional requirements
        CPPUNIT_ASSERT(config.import);
        CPPUNIT_ASSERT(config.dump);
        CPPUNIT_ASSERT(config.compare);
        CPPUNIT_ASSERT(config.testcases);
        
        testLocalDeleteAll();

        // import via sync source A
        std::auto_ptr<SyncSource> source;
        SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(createSourceA()));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
        SOURCE_ASSERT_EQUAL(source.get(), 0, config.import(client, *source.get(), config.testcases));
        SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
        CPPUNIT_ASSERT_NO_THROW(source.reset());

        // export again and compare against original file
        std::auto_ptr<SyncSource> copy;
        SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceA()));
        compareDatabases(config.testcases, *copy.get());
        CPPUNIT_ASSERT_NO_THROW(source.reset());
    }

    // same as testImport() with immediate delete
    void testImportDelete() {
        testImport();

        // delete again, because it was observed that this did not
        // work right with calendars in SyncEvolution
        testLocalDeleteAll();
    }

    // test change tracking with large number of items
    void testManyChanges() {
        // check additional requirements
        CPPUNIT_ASSERT(config.templateItem);
        CPPUNIT_ASSERT(config.uniqueProperties);
        
        deleteAll(createSourceA);

        // check that everything is empty, also resets change counter of sync source B
        std::auto_ptr<SyncSource> copy;
        SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
        SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
        CPPUNIT_ASSERT_NO_THROW(copy.reset());

        // now insert plenty of items
        int numItems = insertManyItems(createSourceA);

        // check that exactly this number of items is listed as new
        SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
        SOURCE_ASSERT_EQUAL(copy.get(), numItems, countItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), numItems, countNewItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, countDeletedItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
        CPPUNIT_ASSERT_NO_THROW(copy.reset());

        // delete all items
        deleteAll(createSourceA);

        // verify again
        SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(createSourceB()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, countNewItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, countUpdatedItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), numItems, countDeletedItems(copy.get()));
        SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
        CPPUNIT_ASSERT_NO_THROW(copy.reset());
    }
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

    SyncTests(const std::string &name, ClientTest &cl, std::vector<int> sourceIndices, bool isClientA = true) :
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
                sources.push_back(std::pair<int,LocalTests *>(*it, new LocalTests(config.sourceName, cl, *it, config)));
            }
        }
        sourceArray[sources.size()] = -1;

        // check whether we have a second client
        ClientTest *clientB = cl.getClientB();
        if (clientB) {
            accessClientB = new SyncTests(name, *clientB, sourceIndices, false);
        } else {
            accessClientB = 0;
        }
    }

    ~SyncTests() {
        for (source_it it = sources.begin();
             it != sources.end();
             ++it) {
            delete it->second;
        }
        delete [] sourceArray;
        if (accessClientB) {
            delete accessClientB;
        }
    }

    /** adds the supported tests to the instance itself */
    void addTests() {
        if (sources.size()) {
            const ClientTest::Config &config(sources[0].second->config);

            ADD_TEST(SyncTests, testTwoWaySync);
            ADD_TEST(SyncTests, testSlowSync);
            ADD_TEST(SyncTests, testRefreshFromServerSync);
            ADD_TEST(SyncTests, testRefreshFromClientSync);

            if (config.createSourceA) {
                if (config.insertItem) {
                    ADD_TEST(SyncTests, testDeleteAllRefresh);
                    ADD_TEST(SyncTests, testRefreshSemantic);
                    ADD_TEST(SyncTests, testRefreshStatus);

                    if (accessClientB &&
                        config.dump &&
                        config.compare) {
                        ADD_TEST(SyncTests, testCopy);
                        ADD_TEST(SyncTests, testDelete);
                        ADD_TEST(SyncTests, testAddUpdate);
                        ADD_TEST(SyncTests, testManyItems);

                        if (config.updateItem) {
                            ADD_TEST(SyncTests, testUpdate);
                        }
                        if (config.complexUpdateItem) {
                            ADD_TEST(SyncTests, testComplexUpdate);
                        }
                        if (config.mergeItem1 && config.mergeItem2) {
                            ADD_TEST(SyncTests, testMerge);
                        }
                        if (config.import) {
                            ADD_TEST(SyncTests, testTwinning);
                            ADD_TEST(SyncTests, testItems);
                        }
                        if (config.templateItem) {
                            ADD_TEST(SyncTests, testMaxMsg);
                            ADD_TEST(SyncTests, testLargeObject);
                            ADD_TEST(SyncTests, testLargeObjectBin);
                            if (client.isB64Enabled()) {
                                ADD_TEST(SyncTests, testLargeObjectEncoded);
                            }
                            ADD_TEST(SyncTests, testOneWayFromServer);
                            ADD_TEST(SyncTests, testOneWayFromClient);
                        }
                    }
                }
            }
        }
    }

private:
    /** list with all local test classes for manipulating the sources and their index in the client */
    std::vector< std::pair<int, LocalTests *> > sources;
    typedef std::vector< std::pair<int, LocalTests *> >::iterator source_it;

    /** the indices from sources, terminated by -1 (for sync()) */
    int *sourceArray;

    /** utility functions for second client */
    SyncTests *accessClientB;

    enum DeleteAllMode {
        DELETE_ALL_SYNC,   /**< make sure client and server are in sync,
                              delete locally,
                              sync again */
        DELETE_ALL_REFRESH /**< delete locally, refresh server */
    };

    /** compare databases of first and second client */
    void compareDatabases() {
        source_it it1;
        source_it it2;

        CPPUNIT_ASSERT(accessClientB);
        for (it1 = sources.begin(), it2 = accessClientB->sources.begin();
             it1 != sources.end() && it2 != accessClientB->sources.end();
             ++it1, ++it2) {
            std::auto_ptr<SyncSource> copy;
            SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(it2->second->createSourceB()));
            it1->second->compareDatabases(NULL, *copy.get());
            CPPUNIT_ASSERT_NO_THROW(copy.reset());
        }
        CPPUNIT_ASSERT(it1 == sources.end());
        CPPUNIT_ASSERT(it2 == accessClientB->sources.end());
    }
    
    /** deletes all items locally and on server */
    void deleteAll(DeleteAllMode mode = DELETE_ALL_SYNC) {
        source_it it;
        
        switch(mode) {
         case DELETE_ALL_SYNC:
            // a refresh from server would slightly reduce the amount of data exchanged, but not all servers support it
            for (it = sources.begin(); it != sources.end(); ++it) {
                it->second->deleteAll(it->second->createSourceA);
            }
            sync(SYNC_TWO_WAY, ".deleteall.init");
            // now that client and server are in sync, delete locally and sync again
            for (it = sources.begin(); it != sources.end(); ++it) {
                it->second->deleteAll(it->second->createSourceA);
            }
            sync(SYNC_TWO_WAY, ".deleteall.twoway");
            break;
         case DELETE_ALL_REFRESH:
            // delete locally and then tell the server to "copy" the empty databases
            for (it = sources.begin(); it != sources.end(); ++it) {
                it->second->deleteAll(it->second->createSourceA);
            }
            sync(SYNC_REFRESH_FROM_CLIENT, ".deleteall.refreshserver");
            break;
        }
    }

    /** get both clients in sync with empty server, then copy one item from client A to B */
    void doCopy() {
        // check requirements
        CPPUNIT_ASSERT(accessClientB);
        
        deleteAll();
        accessClientB->deleteAll();

        // insert into first database, copy to server
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->testSimpleInsert();
        }
        sync(SYNC_TWO_WAY, ".send");

        // copy into second database
        accessClientB->sync(SYNC_TWO_WAY, ".recv");

        compareDatabases();
    }

    /**
     * replicate server database locally: same as SYNC_REFRESH_FROM_SERVER,
     * but done with explicit local delete and then a SYNC_SLOW because some
     * servers do no support SYNC_REFRESH_FROM_SERVER
     */
    void refreshClient() {
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);
        }
        sync(SYNC_SLOW, ".refresh");
    }

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
    void testDeleteAllSync() {
        deleteAll(DELETE_ALL_SYNC);
    }
    
    // delete all items, locally and on server using refresh-from-client sync
    void testDeleteAllRefresh() {
        source_it it;

        // copy something to server first
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->testSimpleInsert();
        }
        sync(SYNC_SLOW, ".insert.client.log");

        // now ensure we can delete it
        deleteAll(DELETE_ALL_SYNC);
    
        // nothing stored locally?
        for (it = sources.begin(); it != sources.end(); ++it) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceA()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }

        // make sure server really deleted everything
        sync(SYNC_SLOW, ".check.client.log");
        for (it = sources.begin(); it != sources.end(); ++it) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceA()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // test that a refresh sync of an empty server leads to an empty datatbase
    void testRefreshSemantic() {
        source_it it;

        // clean client and server
        deleteAll();

        // insert item, then refresh from empty server
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->testSimpleInsert();
        }
        sync(SYNC_REFRESH_FROM_SERVER, "");

        // check
        for (it = sources.begin(); it != sources.end(); ++it) {
            std::auto_ptr<SyncSource> source;
            SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceA()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
            SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
            SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
            CPPUNIT_ASSERT_NO_THROW(source.reset());
        }
    }

    // tests the following sequence of events:
    // - insert item
    // - delete all items
    // - insert one other item
    // - refresh from client
    // => no items should now be listed as new, updated or deleted for this client during another sync
    void testRefreshStatus() {
        source_it it;

        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->testSimpleInsert();
        }
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);
        }
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->testSimpleInsert();
        }
        sync(SYNC_REFRESH_FROM_CLIENT, ".refresh-from-client");
        sync(SYNC_TWO_WAY, ".two-way");
        // TODO: check sync report, no items should have been exchanged
    }

    // test that a two-way sync copies an item from one address book into the other
    void testCopy() {
        doCopy();
        compareDatabases();
    }

    // test that a two-way sync copies updates from database to the other client,
    // using simple data commonly supported by servers
    void testUpdate() {
        CPPUNIT_ASSERT(sources.begin() != sources.end());
        CPPUNIT_ASSERT(sources.begin()->second->config.updateItem);

        // setup client A, B and server so that they all contain the same item
        doCopy();

        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->update(it->second->createSourceA, it->second->config.updateItem);
        }

        sync(SYNC_TWO_WAY, ".update");
        accessClientB->sync(SYNC_TWO_WAY, ".update");

        compareDatabases();
    }

    // test that a two-way sync copies updates from database to the other client,
    // using data that some, but not all servers support, like adding a second
    // phone number to a contact
    void testComplexUpdate() {
        // setup client A, B and server so that they all contain the same item
        doCopy();

        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->update(it->second->createSourceA, it->second->config.complexUpdateItem);
        }

        sync(SYNC_TWO_WAY, ".update");
        accessClientB->sync(SYNC_TWO_WAY, ".update");

        compareDatabases();
    }

        
    // test that a two-way sync deletes the copy of an item in the other database
    void testDelete() {
        // setup client A, B and server so that they all contain the same item
        doCopy();

        // delete it on A
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);
        }

        // transfer change from A to server to B
        sync(SYNC_TWO_WAY, ".delete");
        accessClientB->sync(SYNC_TWO_WAY, ".delete");

        // check client B: shouldn't have any items now
        for (it = sources.begin(); it != sources.end(); ++it) {
            std::auto_ptr<SyncSource> copy;
            SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(it->second->createSourceA()));
            SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
            SOURCE_ASSERT_EQUAL(copy.get(), 0, countItems(copy.get()));
            SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
            CPPUNIT_ASSERT_NO_THROW(copy.reset());
        }
    }
    
    // test what the server does when it finds that different
    // fields of the same item have been modified
    void testMerge() {
        // setup client A, B and server so that they all contain the same item
        doCopy();

        // update in client A
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->update(it->second->createSourceA, it->second->config.mergeItem1);
        }

        // update in client B
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            it->second->update(it->second->createSourceA, it->second->config.mergeItem2);
        }

        // send change to server from client A (no conflict), then from B (conflict)
        sync(SYNC_TWO_WAY, ".send");
        sync(SYNC_TWO_WAY, ".recv");

        // figure out how the conflict during ".recv" was handled
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            std::auto_ptr<SyncSource> copy;
            SOURCE_ASSERT_NO_FAILURE(copy.get(), copy.reset(it->second->createSourceA()));
            SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->beginSync());
            int numItems;
            SOURCE_ASSERT_NO_FAILURE(copy.get(), numItems = countItems(copy.get()));
            SOURCE_ASSERT_EQUAL(copy.get(), 0, copy->endSync());
            CPPUNIT_ASSERT(numItems >= 1);
            CPPUNIT_ASSERT(numItems <= 2);
            std::cout << " " << it->second->config.sourceName << ": " << (numItems == 1 ? "conflicting items were merged" : "both of the conflicting items were preserved") << " ";
            std::cout.flush();
            CPPUNIT_ASSERT_NO_THROW(copy.reset());
        }
    }

    // test what the server does when it has to execute a slow sync
    // with identical data on client and server:
    // expected behaviour is that nothing changes
    void testTwinning() {
        // clean server and client A
        deleteAll();

        // import test data
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->testImport();
        }

        // send to server
        sync(SYNC_TWO_WAY, ".send");

        // ensure that client has the same data, thus ignoring data conversion
        // issues (those are covered by testItems())
        refreshClient();

        // copy to client B to have another copy
        accessClientB->refreshClient();

        // slow sync should not change anything
        sync(SYNC_TWO_WAY, ".twinning");

        // check
        compareDatabases();
    }

    // tests one-way sync from server:
    // - get both clients and server in sync with no items anywhere
    // - add one item on first client, copy to server
    // - add a different item on second client, one-way-from-server
    // - two-way sync with first client
    // => one item on first client, two on second
    // - delete on first client, sync that to second client
    //   via two-way sync + one-way-from-server
    // => one item left on second client (the one inserted locally)
    void testOneWayFromServer() {
        // no items anywhere
        deleteAll();
        accessClientB->refreshClient();
        
        // check that everything is empty, also resets change tracking
        // in second sources of each client
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // add one item on first client, copy to server, and check change tracking via second source
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->insertManyItems(it->second->createSourceA, 1, 1);
        }
        sync(SYNC_TWO_WAY, ".send");
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // add a different item on second client, one-way-from-server
        // => one item added locally, none sent to server
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            it->second->insertManyItems(it->second->createSourceA, 2, 1);

            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
        accessClientB->sync(SYNC_ONE_WAY_FROM_SERVER, ".recv");
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 2, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // two-way sync with first client for verification
        // => no changes
        sync(SYNC_TWO_WAY, ".check");
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // delete items on clientA, sync to server
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);

            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
        sync(SYNC_TWO_WAY, ".delete");
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // sync the same change to second client
        // => one item left (the one inserted locally)
        accessClientB->sync(SYNC_ONE_WAY_FROM_SERVER, ".delete");
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
    }
    
    // tests one-way sync from client:
    // - get both clients and server in sync with no items anywhere
    // - add one item on first client, copy to server
    // - add a different item on second client, one-way-from-client
    // - two-way sync with first client
    // => two items on first client, one on second
    // - delete on second client, sync that to first client
    //   via one-way-from-client, two-way
    // => one item left on first client (the one inserted locally)
    void testOneWayFromClient() {
        // no items anywhere
        deleteAll();
        accessClientB->deleteAll();
        
        // check that everything is empty, also resets change tracking
        // in second sources of each client
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // add one item on first client, copy to server, and check change tracking via second source
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->insertManyItems(it->second->createSourceA, 1, 1);
        }
        sync(SYNC_TWO_WAY, ".send");
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // add a different item on second client, one-way-from-client
        // => no item added locally, one sent to server
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            it->second->insertManyItems(it->second->createSourceA, 2, 1);

            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
        accessClientB->sync(SYNC_ONE_WAY_FROM_CLIENT, ".send");
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // two-way sync with client A for verification
        // => receive one item
        sync(SYNC_TWO_WAY, ".check");
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 2, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // delete items on client B, sync to server
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            it->second->deleteAll(it->second->createSourceA);

            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
        accessClientB->sync(SYNC_ONE_WAY_FROM_CLIENT, ".delete");
        for (it = accessClientB->sources.begin(); it != accessClientB->sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 0, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }

        // sync the same change to client A
        // => one item left (the one inserted locally)
        sync(SYNC_TWO_WAY, ".delete");
        for (it = sources.begin(); it != sources.end(); ++it) {
            if (it->second->config.createSourceB) {
                std::auto_ptr<SyncSource> source;
                SOURCE_ASSERT_NO_FAILURE(source.get(), source.reset(it->second->createSourceB()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->beginSync());
                SOURCE_ASSERT_EQUAL(source.get(), 1, countItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countNewItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 1, countDeletedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, countUpdatedItems(source.get()));
                SOURCE_ASSERT_EQUAL(source.get(), 0, source->endSync());
                CPPUNIT_ASSERT_NO_THROW(source.reset());
            }
        }
    }

    // creates several items, transmits them back and forth and
    // then compares which of them have been preserved
    void testItems() {
        // clean server and first test database
        deleteAll();

        // import data
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->testImport();
        }

        // transfer from client A to server to client B
        sync(SYNC_TWO_WAY, ".send");
        accessClientB->refreshClient();
        
        compareDatabases();
    }

    // tests the following sequence of events:
    // - both clients in sync with server
    // - client 1 adds item
    // - client 1 updates the same item
    // - client 2 gets item: the client should be asked to add the item
    //
    // However it has been observed that sometimes the item was sent as "update"
    // for a non-existant local item. This is a server bug, the client does not
    // have to handle that. See
    // http://forge.objectweb.org/tracker/?func=detail&atid=100096&aid=305018&group_id=96
    void testAddUpdate() {
        // clean server and both test databases
        deleteAll();
        accessClientB->refreshClient();

        // add item
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->insert(it->second->createSourceA, it->second->config.insertItem);
        }
        sync(SYNC_TWO_WAY, ".add");

        // update it
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->insert(it->second->createSourceB, it->second->config.updateItem);
        }
        sync(SYNC_TWO_WAY, ".update");

        // now download the updated item into the second client
        accessClientB->sync(SYNC_TWO_WAY, ".recv");

        // compare the two databases
        compareDatabases();
    }

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
    void testManyItems() {
        // clean server and client A
        deleteAll();

        // import artificial data
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            it->second->insertManyItems(it->second->createSourceA);
        }
    
        // send data to server
        sync(SYNC_TWO_WAY, ".send");

        // ensure that client has the same data, ignoring data conversion
        // issues (those are covered by testItems())
        refreshClient();
        
        // also copy to second client
        accessClientB->refreshClient();

        // slow sync now should not change anything
        sync(SYNC_SLOW, ".twinning");

        // compare
        compareDatabases();
    }


    /**
     * implements testMaxMsg(), testLargeObject(), testLargeObjectEncoded()
     * using a sequence of items with varying sizes
     */
    void doVarSizes(bool withMaxMsgSize,
                    bool withLargeObject,
                    const char *encoding) {
        static const int maxMsgSize = 8 * 1024;
    
        // clean server and client A
        deleteAll();

        // insert items, doubling their size, then restart with small size
        source_it it;
        for (it = sources.begin(); it != sources.end(); ++it) {
            int item = 1;
            for (int i = 0; i < 2; i++ ) {
                int size = 1;
                while (size < 2 * maxMsgSize) {
                    it->second->insertManyItems(it->second->createSourceA, item, 1, (int)strlen(it->second->config.templateItem) + 10 + size);
                    size *= 2;
                    item++;
                }
            }
        }

        // transfer to server
        sync(SYNC_TWO_WAY, ".send",
             withMaxMsgSize ? maxMsgSize : 0,
             withMaxMsgSize ? maxMsgSize * 100 : 0,
             withLargeObject,
             encoding);

        // copy to second client
        accessClientB->sync(SYNC_REFRESH_FROM_SERVER, ".recv",
                            withLargeObject ? maxMsgSize : withMaxMsgSize ? maxMsgSize * 100 /* large enough so that server can sent the largest item */ : 0,
                            withMaxMsgSize ? maxMsgSize * 100 : 0,
                            withLargeObject,
                            encoding);

        // compare
        compareDatabases();
    }

    void sync(SyncMode syncMode,
              const std::string &logprefix = "",
              long maxMsgSize = 0,
              long maxObjSize = 0,
              bool loSupport = false,
              const char *encoding = "") {
        int res = 0;
        static int syncCounter = 0;
        static std::string lastTest;
        std::stringstream logstream;

        // reset counter when switching tests
        if (lastTest != getCurrentTest()) {
            syncCounter = 0;
            lastTest = getCurrentTest();
        }

        logstream << std::setw(4) << std::setfill('0') << syncCounter << "_" << getCurrentTest() << logprefix;
        std::string logname = logstream.str();
        simplifyFilename(logname);
        syncCounter++;

        std::string logfile = logname + ".client." + (accessClientB ? "A" : "B") + ".log";
        remove(logfile.c_str());
        LOG.setLogName(logfile.c_str());
        LOG.reset();

        try {
            res = client.sync(sourceArray,
                              syncMode,
                              maxMsgSize,
                              maxObjSize,
                              loSupport,
                              encoding);

            client.postSync(res, logname);
        } catch (...) {
            res = 1;
            client.postSync(res, logname);

            // this logs the original exception using CPPUnit mechanisms
            CPPUNIT_ASSERT_NO_THROW( throw );
        }
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
                    new LocalTests(tests->getName() + "::" + config.sourceName, client, source, config);
                sourcetests->addTests();
                tests->addTest(sourcetests);
            }
        }
        alltests->addTest(tests);
        tests = 0;

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
        tests = 0;
        
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

ClientTest::ClientTest(int serverSleepSec, const std::string &serverLog) :
    serverSleepSeconds(serverSleepSec),
    serverLogFileName(serverLog),
    factory(NULL)
{
}

ClientTest::~ClientTest()
{
    if(factory) {
#ifdef ENABLE_INTEGRATION_TESTS
        CppUnit::TestFactoryRegistry::getRegistry().unregisterFactory((CppUnit::TestFactory *)factory);
        delete (CppUnit::TestFactory *)factory;
        factory = 0;
#endif
    }
}

int ClientTest::dump(ClientTest &client, SyncSource &source, const char *file)
{
    std::ofstream out(file);

    std::auto_ptr<SyncItem> item;
    SOURCE_ASSERT_NO_FAILURE(&source, item.reset(source.getFirstItem()));
    while (item.get()) {
        out << (char *)item->getData() << std::endl;
        SOURCE_ASSERT_NO_FAILURE(&source, item.reset(source.getNextItem()));
    }
    out.close();
    return out.bad();
}

int ClientTest::import(ClientTest &client, SyncSource &source, const char *file)
{
    // import the file
    std::ifstream input;
    input.open(file);
    CPPUNIT_ASSERT(!input.bad());
    CPPUNIT_ASSERT(input.is_open());
    std::string data, line;
    while (input) {
        do {
            getline(input, line);
            CPPUNIT_ASSERT(!input.bad());
            // empty line marks end of record
            if (line != "\r" && line.size() > 0) {
                data += line;
                data += "\n";
            } else {
                importItem(&source, data);
                data = "";
            }
        } while(!input.eof());
    }
    importItem(&source, data);

    return 0;
}

bool ClientTest::compare(ClientTest &client, const char *fileA, const char *fileB)
{
    std::string cmdstr = std::string("perl synccompare.pl ") + fileA + " " + fileB;
    return system(cmdstr.c_str()) == 0;
}


#ifndef WIN32
#include <fcntl.h>
#endif

void ClientTest::postSync(int res, const std::string &logname)
{
#ifdef WIN32
	Sleep(serverSleepSeconds * 1000);
#else
    sleep(serverSleepSeconds);

    // make a copy of the server's log (if found), then truncate it
    if (serverLogFileName.size()) {
        int fd = open(serverLogFileName.c_str(), O_RDWR);
            
        if (fd >= 0) {
            std::string cmd = std::string("cp ") + serverLogFileName + " " + logname + ".server.log";
            system(cmd.c_str());
            ftruncate(fd, 0);
        } else {
            perror(serverLogFileName.c_str());
        }
    }
#endif
}

void ClientTest::getTestData(const char *type, Config &config)
{
    memset(&config, 0, sizeof(config));

    if (!strcmp(type, "vcard30")) {
        config.sourceName = "vcard30";
        config.type = "text/vcard";
        config.insertItem =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "X-EVOLUTION-FILE-AS:Doe\\, John\n"
            "X-MOZILLA-HTML:FALSE\n"
            "NOTE:\n"
            "END:VCARD\n";
        config.updateItem =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:Joan Doe\n"
            "N:Doe;Joan;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, Joan\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 2\n"
            "BDAY:2006-01-08\n"
            "X-MOZILLA-HTML:TRUE\n"
            "END:VCARD\n";
        /* adds a second phone number: */
        config.complexUpdateItem =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:Joan Doe\n"
            "N:Doe;Joan;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, Joan\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "TEL;TYPE=HOME;TYPE=VOICE:home 2\n"
            "BDAY:2006-01-08\n"
            "X-MOZILLA-HTML:TRUE\n"
            "END:VCARD\n";
        /* add a telephone number, email and X-AIM to initial item */
        config.mergeItem1 =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:tester\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, John\n"
            "X-MOZILLA-HTML:FALSE\n"
            "TEL;TYPE=WORK;TYPE=VOICE:business 1\n"
            "EMAIL:john.doe@work.com\n"
            "X-AIM:AIM JOHN\n"
            "END:VCARD\n";
        config.mergeItem2 =
            "BEGIN:VCARD\n"
            "VERSION:3.0\n"
            "TITLE:developer\n"
            "FN:John Doe\n"
            "N:Doe;John;;;\n"
            "X-EVOLUTION-FILE-AS:Doe\\, John\n"
            "X-MOZILLA-HTML:TRUE\n"
            "BDAY:2006-01-08\n"
            "END:VCARD\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "FN:N:X-EVOLUTION-FILE-AS";
        config.sizeProperty = "NOTE";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/vcard30.vcf";
    } else if(!strcmp(type, "ical20")) {
        config.sourceName = "ical20";
        config.type = "text/x-vcalendar";
        config.insertItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND;20060406T163000Z\n"
            "DTSTART;20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:my office\n"
            "DESCRIPTION:let's talk\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.updateItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:meeting on site\n"
            "DTEND;20060406T163000Z\n"
            "DTSTART;20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:big meeting room\n"
            "DESCRIPTION:nice to see you\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        /* change location in insertItem in testMerge() */
        config.mergeItem1 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND;20060406T163000Z\n"
            "DTSTART;20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:calling from home\n"
            "DESCRIPTION:let's talk\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.mergeItem2 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VEVENT\n"
            "SUMMARY:phone meeting\n"
            "DTEND;20060406T163000Z\n"
            "DTSTART;20060406T160000Z\n"
            "UID:1234567890!@#$%^&*()<>@dummy\n"
            "DTSTAMP:20060406T211449Z\n"
            "LAST-MODIFIED:20060409T213201\n"
            "CREATED:20060409T213201\n"
            "LOCATION:my office\n"
            "DESCRIPTION:what the heck\\, let's even shout a bit\n"
            "CLASS:PUBLIC\n"
            "TRANSP:OPAQUE\n"
            "SEQUENCE:1\n"
            "END:VEVENT\n"
            "END:VCALENDAR\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "SUMMARY:UID";
        config.sizeProperty = "DESCRIPTION";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/ical20.ics";
    } else if(!strcmp(type, "itodo20")) {
        config.sourceName = "itodo20";
        config.type = "text/x-vcalendar";
        config.insertItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:0\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        config.updateItem =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me ASAP\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:1\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        /* change summary in insertItem in testMerge() */
        config.mergeItem1 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me please\\, please\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:0\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        config.mergeItem2 =
            "BEGIN:VCALENDAR\n"
            "PRODID:-//Ximian//NONSGML Evolution Calendar//EN\n"
            "VERSION:2.0\n"
            "METHOD:PUBLISH\n"
            "BEGIN:VTODO\n"
            "UID:20060417T173712Z-4360-727-1-2730@gollum\n"
            "DTSTAMP:20060417T173712Z\n"
            "SUMMARY:do me\n"
            "DESCRIPTION:to be done\n"
            "PRIORITY:7\n"
            "STATUS:IN-PROCESS\n"
            "CREATED:20060417T173712\n"
            "LAST-MODIFIED:20060417T173712\n"
            "END:VTODO\n"
            "END:VCALENDAR\n";
        config.templateItem = config.insertItem;
        config.uniqueProperties = "SUMMARY:UID";
        config.sizeProperty = "DESCRIPTION";
        config.import = import;
        config.dump = dump;
        config.compare = compare;
        config.testcases = "testcases/itodo20.ics";
    }
}