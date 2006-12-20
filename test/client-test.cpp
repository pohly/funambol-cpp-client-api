/*
 * Copyright (C) 2006 Funambol
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

/**
 * This code uses the ClientTest and FILESyncSource to test real
 * synchronization against a server. More than one FILESyncSource can
 * be active at once and each of them may (but does not have to be)
 * used for different kinds of data. The name of the source determines
 * which data is stored in it: it must be something supported by the
 * ClientTest class, because that is where the test data comes from.
 *
 * At least the following kinds of data are currently supported by the
 * ClientTest class (see ClientTest::getSourceConfig() for more
 * information):
 * - vcard30 = vCard 3.0 contacts
 * - ical20 = iCalendar 2.0 calendar events
 * - itodo20 = iCalendar 2.0 tasks
 *
 * Configuration is done by environment variables which indicate which
 * part below the root node "client-test" of the the configuration tree to use;
 * beyond that everything needed for synchronization is read from the
 * configuration tree.
 *
 * - CLIENT_TEST_SERVER = maps to name of root node in configuration tree
 * - CLIENT_TEST_SOURCES = comma separated list of active sources,
 *                         names as listed above
 * - CLIENT_TEST_DELAY = number of seconds to sleep between syncs, required
 *                       by some servers
 * - CLIENT_TEST_LOG = logfile name of a server, can be empty:
 *                     if given, then the content of that file will be
 *                     copied and stored together with the client log
 *                     (only works on Unix)
 *
 * For example, on Linux running
 *    CLIENT_TEST_SERVER=funambol CLIENT_TEST_SOURCES=vcard30,ical20 ./client-test
 * expects the following configuration layout:
 * ~/.sync4j/client-test/
 *                       funambol_1/spds/
 *                                       syncml/config.text
 *                                       sources/
 *                                               vcard30/config.txt
 *                                               ical20/config.txt
 *                       funambol_1/spds/
 *                                       <same as for funambol_1>
 *
 * If any of the configuration nodes does not exist yet, then it will
 * be created, but further information may have to be added, in
 * particular:
 * - server URL
 * - server user name, password
 * - sources uri
 *
 * The two configurations are used to simulate synchronization between
 * two different clients.
 *
 * The file sources will store their items in sub directories of
 * a "client-data" directory created in the current working directory.
 *
 * Here is an example of using the CLIENT_TEST_LOG:
 *    CLIENT_TEST_SERVER=funambol \
 *    CLIENT_TEST_LOG=/opt/Funambol-3.0/ds-server/logs/funambol_ds.log \
 *    CLIENT_TEST_SOURCES=vcard30 \
 *    ./client-test
 * will create files with the suffix .client.1.log for synchronizations with
 * the first client and .client.2.log for the second client. The base name
 * of these files is unique, so the corresponding part of the server log
 * is stored with the same base name and .server.log as suffix. 
 */

#include "spdm/DeviceManagementNode.h"
#include "spds/RawFILESyncSource.h"
#include "spds/spdsutils.h"
#include "client/DMTClientConfig.h"
#include "client/SyncClient.h"
#include "test/ClientTest.h"

#include <string>
#include <vector>
#include <iomanip>
#include <memory>

#include <unistd.h>
#include <sys/stat.h>

class TestFileSource : public ClientTest {
public:
    TestFileSource(const std::string &id) :
        ClientTest(getenv("CLIENT_TEST_DELAY") ? atoi(getenv("CLIENT_TEST_DELAY")) : 0,
                   getenv("CLIENT_TEST_LOG") ? getenv("CLIENT_TEST_LOG") : ""),
        clientID(id) {
        const char *sourcelist = getenv("CLIENT_TEST_SOURCES");
        const char *server = getenv("CLIENT_TEST_SERVER");

        /* set up source list */
        if (!sourcelist) {
            sourcelist = "";
        }
        const char *eostr = strchr(sourcelist, ',');
        const char *start = sourcelist;

        while (eostr) {
            sources.push_back(std::string(start, 0, eostr - start));
            start = eostr + 1;
            eostr = strchr(start, ',');
        }
        if (start[0]) {
            sources.push_back(start);
        }

        /* check server */
        if (!server) {
            server = "funambol";
        }

        // get configuration and set obligatory fields
        LOG.setLevel(LOG_LEVEL_DEBUG);
        std::string root = std::string("client-test/") + server + "_" + id;
        config.reset(new DMTClientConfig(root.c_str()));
        config->read();
        DeviceConfig &dc(config->getDeviceConfig());
        if (!strlen(dc.getDevID())) {
            // no configuration yet
            config->setClientDefaults();
            dc.setDevID(id == "A" ? "sc-api-nat" : "sc-pim-ppc");
        }
        for (int source = 0; source < sources.size(); source++) {
            SyncSourceConfig* sc = config->getSyncSourceConfig(sources[source].c_str());
            if (!sc) {
                // no configuration yet
                config->setSourceDefaults(sources[source].c_str());
                sc = config->getSyncSourceConfig(sources[source].c_str());
                CPPUNIT_ASSERT(sc);
            }

            ClientTest::Config testconfig;
            getSourceConfig(source, testconfig);
            CPPUNIT_ASSERT(testconfig.type);
            sc->setType(testconfig.type);
        }
        config->save();
        config->open();

        if (id == "A") {
            /* we are the primary client, create a second one */
            clientB.reset(new TestFileSource("B"));
        }
    }
    
    virtual int getNumSources() {
        return sources.size();
    }

    virtual void getSourceConfig(int source, Config &config) {
        memset(&config, 0, sizeof(config));

        getTestData(sources[source].c_str(), config);
        config.createSourceA =
            config.createSourceB = createSource;
    }

    virtual ClientTest* getClientB() {
        return clientB.get();
    }

    virtual bool isB64Enabled() {
        return false;
    }

    virtual int sync(
        const int *activeSources,
        SyncMode syncMode,
        long maxMsgSize,
        long maxObjSize,
        bool loSupport,
        const char *encoding = 0) {
        SyncSource **syncSources = new SyncSource *[sources.size() + 1];
        int source;
        memset(syncSources, 0, sizeof(syncSources[0]) * (sources.size() + 1));

        for (source = 0; activeSources[source] >= 0 && source < sources.size(); source++) {
            // rewrite configuration as needed for test
            SyncSourceConfig *sourceConfig = config->getSyncSourceConfig(sources[source].c_str());
            CPPUNIT_ASSERT(sourceConfig);
            sourceConfig->setSync(syncModeKeyword(syncMode));
            sourceConfig->setEncoding(encoding);
            config->getAccessConfig().setMaxMsgSize(maxMsgSize);
            config->getDeviceConfig().setMaxObjSize(maxObjSize);
            config->getDeviceConfig().setLoSupport(loSupport);

            // create sync source using the third change tracking for syncs
            syncSources[source] = createSource(source, "S");
        }

        SyncClient client;
        int res = client.sync(*config, syncSources);

        for (source = 0; syncSources[source]; source++) {
            delete syncSources[source];
        }

        return res;
    }

private:
    /** either "A" or "B" for first respectively second client */
    std::string clientID;

    /** only in "A": pointer to second client */
    std::auto_ptr<TestFileSource> clientB;

    /** vector of enabled sync sources, identified by a name which SyncClient::getConfig() supports */
    std::vector<std::string> sources;

    /** configuration tree itself */
    std::auto_ptr<DMTClientConfig> config;

    static SyncSource *createSource(ClientTest &client, int source, bool isSourceA) {
        // hand work over to real member function
        ((TestFileSource &)client).createSource(source, isSourceA ? "A" : "B");
    }

    SyncSource *createSource(int source, const char *trackingSuffix) {
        class RawFILESyncSourceWithReport : public RawFILESyncSource {
        public:
            RawFILESyncSourceWithReport(const char* nodeName, const char* name, SyncSourceConfig* sc) :
                RawFILESyncSource(name, sc),
                fileNode(nodeName) {
                setReport(&report);
                setFileNode(&fileNode);
                /*
                 * Keeping track if changes is done via time() with a resolution of seconds.
                 * Sleep a bit to ensure that enough time passes.
                 */
                sleep(1);
            }
        private:
            SyncSourceReport report;
            DeviceManagementNode fileNode;
        };

        CPPUNIT_ASSERT(source < sources.size());
        ManagementNode *sourceNode = config->getSyncSourceNode(sources[source].c_str());
        CPPUNIT_ASSERT(sourceNode);
        char *fullName = sourceNode->createFullName();
        std::string nodeName = std::string(fullName) + "/changes_" + trackingSuffix;
        std::string dirName = sources[source] + "_" + clientID;
        delete [] fullName;
        FILESyncSource *ss = new RawFILESyncSourceWithReport(
            nodeName.c_str(),
            sources[source].c_str(),
            config->getSyncSourceConfig(sources[source].c_str()));
        mkdir(dirName.c_str(), S_IRWXU);
        ss->setDir(dirName.c_str());

        return ss;
    }
};


/**
 * the only purpose of this class is to own the first TestFileSource
 * and to register its tests at program startup
 */
static class RegisterTest {
public:
    RegisterTest() :
        testFileSource("A") {
        testFileSource.registerTests();
    }

private:
    TestFileSource testFileSource;
} registerTest;
