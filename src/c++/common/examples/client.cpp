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

#include "base/fscapi.h"
//#include "crtdbg.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"
#include "examples/TestSyncSource.h"
#include "examples/TestSyncSource2.h"
#include "filter/AllClause.h"
#include "filter/ClauseUtil.h"
#include "filter/LogicalClause.h"
#include "filter/FieldClause.h"
#include "filter/SourceFilter.h"
#include "filter/WhereClause.h"
#include "syncml/core/core.h"
#include "syncml/formatter/Formatter.h"
#include "spds/DefaultConfigFactory.h"

#include "examples/listeners/TestSyncListener.h"
#include "examples/listeners/TestSyncSourceListener.h"
#include "examples/listeners/TestSyncStatusListener.h"
#include "examples/listeners/TestSyncItemListener.h"
#include "examples/listeners/TestTransportListener.h"
#include "event/SetListener.h"

// Define the test configuration
#include "examples/config.h"


void testFilter();
void testClause();
void testConfigFilter();
void testEncryption();
void createConfig(DMTClientConfig& config);
static void testXMLProcessor();
void printReport(SyncReport* sr, const char* sourceName);


#define APPLICATION_URI "Funambol/SyncclientPIM"
#define LOG_TITLE		"Funambol Win32 Example Log"
#define LOG_PATH		"."
#define LOG_LEVEL		LOG_LEVEL_DEBUG
#define SOURCE_NAME     "briefcase"
#define WSOURCE_NAME    TEXT("briefcase")
#define DEVICE_ID       "Funambol Win32 Example"


// Define DEBUG_SETTINGS in your project to create a default configuration
// tree for the test client. WARNING: it will override any previous setting!
// 
#ifdef DEBUG_SETTINGS
int settings(const char *root);
#endif

#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd ) {
#else
int main(int argc, char** argv) {
#endif

    // Init LOG
    Log(0, LOG_PATH, LOG_NAME);
	LOG.reset(LOG_TITLE);
    LOG.setLevel(LOG_LEVEL);
    
#if 0
    _CrtSetDbgFlag (ON);

    // Get current flag
    int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

    // Turn on leak-checking bit
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

    // Turn on automatic checks
    tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;

    // Set flag to the new value
    _CrtSetDbgFlag( tmpFlag );
#endif

#ifdef DEBUG_SETTINGS
    if ( settings(APPLICATION_URI) ){
        sprintf(logmsg, "Error %d setting config paramaters.", lastErrorCode);
        LOG.error(logmsg);
        return lastErrorCode;
    }
#endif

#ifdef TEST_ENCODE
    WCHAR *content = loadAndConvert(TEXT("message.xml"), TEXT("base64"));
    if(!content) {
        fprintf(stderr, "Error in uudecode.");
        exit(1);
    }
    convertAndSave(TEXT("message_out.xml"), content, TEXT("base64"));
#endif


#ifdef TEST_EVENT_HANDLING

    //
    // Set listeners:
    //
    TestSyncListener*       listener1 = new TestSyncListener();
    TestSyncSourceListener* listener2 = new TestSyncSourceListener();
    TestSyncStatusListener* listener3 = new TestSyncStatusListener();
    TestSyncItemListener*   listener4 = new TestSyncItemListener();
    TestTransportListener*  listener5 = new TestTransportListener();

    setSyncListener      (listener1);
    setSyncSourceListener(listener2);
    setSyncStatusListener(listener3);
    setSyncItemListener  (listener4);
    setTransportListener (listener5);

    #ifndef TEST_SYNCSOURCE
    #define TEST_SYNCSOURCE  1
    #endif

#endif

// ------------- Main sample client ------------------------
#ifdef TEST_SYNCSOURCE
    
    //
    // Create the configuration.
    //
    DMTClientConfig config(APPLICATION_URI); 
 
    // Read config from registry.
    if (!config.read() || 
        strcmp(config.getDeviceConfig().getDevID(), DEVICE_ID)) {
        // Config not found -> generate a default config
        createConfig(config);
    }
	
    //
    // Create the SyncSource passing its name and its config.
    //
    TestSyncSource source(WSOURCE_NAME, config.getSyncSourceConfig(SOURCE_NAME));
    SyncSource* ssArray[2];
    ssArray[0] = &source;
    ssArray[1] = NULL;

    //
    // Create the SyncClient passing the config.
    //
    SyncClient sampleClient;

    // SYNC!
    if( sampleClient.sync(config, ssArray) ) {
        LOG.error("Error in sync.");
    }

    // Print sync results.
    printReport(sampleClient.getSyncReport(), SOURCE_NAME);

    // Save config to registry.
    config.save();

#endif
// ----------------------------------------------------------


#ifdef TEST_EVENT_HANDLING

    //
    // Unset Listeners
    //
    unsetSyncListener      ();
    unsetSyncSourceListener();
    unsetSyncStatusListener();
    unsetSyncItemListener  ();
    unsetTransportListener ();

#endif

    
#ifdef TEST_SYNC_ENCRYPTION
	Sync4jClient& s4j = Sync4jClient::getInstance();
    s4j.setDMConfig(APPLICATION_URI);
    
    TestSyncSource source = TestSyncSource(TEXT("briefcase"));                             

    SyncSource** ssArray = new SyncSource*[2];
    ssArray[0] = &source;
    ssArray[1] = NULL;
    s4j.sync(ssArray);
#endif

#ifdef TEST_ENCRYPTION
	testEncryption();
#endif

#ifdef TEST_FILTER
    testFilter();
#endif

#ifdef TEST_CLAUSE
    testClause();
#endif

#ifdef TEST_CONFIG_FILTER
    testConfigFilter();
#endif



#ifdef TEST_XMLPROCESSOR
    testXMLProcessor();
#endif
    return 0;
}

static void testXMLProcessor(void)
{
    const char xml1[] = 
        "<document>\n\
            <LocURI>./devinf11</LocURI>\n\
            <plaintag>\n\
                <attrtag attr=\"val\">content</attrtag>\n\
            </plaintag>\n\
            <emptytag/>\n\
         </document>" ;
         
    unsigned int pos = 0, start = 0, end = 0;
    const char *p = 0;

    // Get 'document' tag
    char *doc = XMLProcessor::copyElementContent(xml1, "document", &pos);
    LOG.debug("Document: '%s'", doc);
    LOG.debug("xml[pos]= '%s'", xml1 + pos);

    char buf[256];

    // Get 'plaintag' content, using start/end pos
    if(!XMLProcessor::getElementContent(doc, "plaintag", &pos, &start, &end)){
        LOG.error("TEST FAILED.");
        return;
    }
    memset(buf, 0, 255);
    memcpy(buf, doc+start, end-start);
    LOG.debug("Plaintag: '%s'", buf);

    // Get 'LocURI' content, using start/end pos
    if(!XMLProcessor::getElementContent(doc, "LocURI", &pos, &start, &end)){
        LOG.error("TEST FAILED.");
        return;
    }
    memset(buf, 0, 255);
    memcpy(buf, doc+start, end-start);
    LOG.debug("LocURI: '%s'", buf);

    // Get 'attrtag' content, using start/end pos
    if(!XMLProcessor::getElementContent(doc, "attrtag", &pos, &start, &end)){
        LOG.error("TEST FAILED.");
        return;
    }
    memset(buf, 0, 255);
    memcpy(buf, doc+start, end-start);
    LOG.debug("Attrtag: '%s'", buf);

    // Get 'attrtag' attr list, using start/end pos
    if(!XMLProcessor::getElementAttributes(doc, "attrtag", &start, &end)){
        LOG.error("TEST FAILED.");
        return;
    }
    memset(buf, 0, 255);
    memcpy(buf, doc+start, end-start);
    LOG.debug("Attrlist: '%s'", buf);

    // Get 'emptytag' content, that should be empty
    const char*empty = XMLProcessor::copyElementContent(doc, "emptytag");
    if(!empty){
        LOG.error("TEST FAILED.");
        return;
    }
    LOG.debug("Emptytag: '%s'", empty);
    
    if(doc)
        delete [] doc;
    if (empty)
        delete [] empty;
}


//
// Function to create a default config.
//
void createConfig(DMTClientConfig& config) {

    AccessConfig* ac = DefaultConfigFactory::getAccessConfig();
    config.setAccessConfig(*ac);
    delete ac;

    DeviceConfig* dc = DefaultConfigFactory::getDeviceConfig();
    dc->setDevID(DEVICE_ID);     // So next time won't be generated, we always save config at the end.
    dc->setMan  ("Funambol");
    config.setDeviceConfig(*dc);
    delete dc;

    SyncSourceConfig* sc = DefaultConfigFactory::getSyncSourceConfig(SOURCE_NAME);
    sc->setEncoding("plain/text");
    sc->setType    ("text");
    sc->setURI     ("briefcase");
    config.setSyncSourceConfig(*sc);
    delete sc;
}




#include "base/util/StringBuffer.h"
//
// Prints a formatted report for the synchronization process.
//
void printReport(SyncReport* sr, const char*sourceName) {

    StringBuffer res;
    char tmp[512];

    res =      "===========================================================\n";
    res.append("================   SYNCHRONIZATION REPORT   ===============\n");
    res.append("===========================================================\n");

    sprintf(tmp, "Last error code = %d\n", sr->getLastErrorCode());
    res.append(tmp);
    sprintf(tmp, "Last error msg  = %s\n\n", sr->getLastErrorMsg());
    res.append(tmp);

    res.append("----------|--------CLIENT---------|--------SERVER---------|\n");
    res.append("  Source  |  NEW  |  MOD  |  DEL  |  NEW  |  MOD  |  DEL  |\n");
    res.append("----------|-----------------------------------------------|\n");

    int sourceNumber = 1;
    for (int i=0; i<sourceNumber; i++) {
        SyncSourceReport* ssr = sr->getSyncSourceReport(sourceName);

        sprintf(tmp, "%10s|", ssr->getSourceName());
        res.append(tmp);
        sprintf(tmp, "%3d/%3d|", ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_ADD), ssr->getItemReportCount(CLIENT, COMMAND_ADD));
        res.append(tmp);
        sprintf(tmp, "%3d/%3d|", ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_REPLACE), ssr->getItemReportCount(CLIENT, COMMAND_REPLACE));
        res.append(tmp);
        sprintf(tmp, "%3d/%3d|", ssr->getItemReportSuccessfulCount(CLIENT, COMMAND_DELETE), ssr->getItemReportCount(CLIENT, COMMAND_DELETE));
        res.append(tmp);

        sprintf(tmp, "%3d/%3d|", ssr->getItemReportSuccessfulCount(SERVER, COMMAND_ADD), ssr->getItemReportCount(SERVER, COMMAND_ADD));
        res.append(tmp);
        sprintf(tmp, "%3d/%3d|", ssr->getItemReportSuccessfulCount(SERVER, COMMAND_REPLACE), ssr->getItemReportCount(SERVER, COMMAND_REPLACE));
        res.append(tmp);
        sprintf(tmp, "%3d/%3d|\n", ssr->getItemReportSuccessfulCount(SERVER, COMMAND_DELETE), ssr->getItemReportCount(SERVER, COMMAND_DELETE));
        res.append(tmp);
        res.append("----------|-----------------------------------------------|\n\n");

        sprintf(tmp, "%s:\n----------", ssr->getSourceName());
        res.append(tmp);
        sprintf(tmp, "\nSource State    = %d\n", ssr->getState());
        res.append(tmp);
        sprintf(tmp, "Last error code = %d\n", ssr->getLastErrorCode());
        res.append(tmp);
        sprintf(tmp, "Last error msg  = %s\n\n", ssr->getLastErrorMsg());
        res.append(tmp);
    }


    printf("\n%s", res.c_str());
}
