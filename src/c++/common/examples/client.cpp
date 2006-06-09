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
#include "client/Sync4jClient.h"
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

#include "examples/MySyncListener.h"
#include "examples/MySyncSourceListener.h"
#include "examples/MySyncStatusListener.h"
#include "examples/MySyncItemListener.h"
#include "examples/MyTransportListener.h"

#include "event/SetListener.h"


// Define the test configuration
#include "examples/config.h"

void testFilter();
void testClause();
void testConfigFilter();
void testEncryption();

static void testXMLProcessor();

//#define APPLICATION_URI T("Funambol/examples/dummy")
#define APPLICATION_URI T("Funambol/SyncclientPIM")

#define LOG_TITLE		T("Funambol Win32 Example Log")
#define LOG_PATH		T(".")
#define LOG_LEVEL		LOG_LEVEL_DEBUG


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
    wchar_t *content = loadAndConvert(TEXT("message.xml"), TEXT("base64"));
    if(!content) {
        fprintf(stderr, "Error in uudecode.");
        exit(1);
    }
    convertAndSave(TEXT("message_out.xml"), content, TEXT("base64"));
#endif

#ifdef TEST_SYNCSOURCE
	Sync4jClient& s4j = Sync4jClient::getInstance();
    s4j.setDMConfig(APPLICATION_URI);
    
	
    TestSyncSource source = TestSyncSource(TEXT("briefcase"));           
    //TestSyncSource2 source2 = TestSyncSource2(TEXT("contact"));           
    //TestSyncSource source3 = TestSyncSource(TEXT("notExisting"));           

    SyncSource** ssArray = new SyncSource*[2];
    ssArray[0] = &source;
    //ssArray[1] = &source2;
    //ssArray[2] = &source3;
    ssArray[1] = NULL;
    if( s4j.sync(ssArray) ) {
        LOG.error("Error in sync.");
    }
    s4j.dispose();
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


#ifdef TEST_EVENT_HANDLING

    Sync4jClient& s4j = Sync4jClient::getInstance();
    s4j.setDMConfig(APPLICATION_URI);

    SetListener * set = new SetListener();

    MySyncListener * listener1 = new MySyncListener();
    set->setSyncListener(listener1);

    MySyncSourceListener * listener2 = new MySyncSourceListener();
    set->setSyncSourceListener(listener2);

    MySyncStatusListener * listener3 = new MySyncStatusListener();
    set->setSyncStatusListener(listener3);

    MySyncItemListener * listener4 = new MySyncItemListener();
    set->setSyncItemListener(listener4);

    MyTransportListener * listener5 = new MyTransportListener();
    set->setTransportListener(listener5); 
    TestSyncSource2 source2 = TestSyncSource2(TEXT("contact"));

    SyncSource** ssArray = new SyncSource*[2];
    ssArray[0] = &source2;

    ssArray[1] = NULL;
    s4j.sync(ssArray);

    set->unsetSyncListener();
    set->unsetSyncSourceListener();
    set->unsetSyncStatusListener();
    set->unsetSyncItemListener();
    set->unsetTransportListener();
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
    char *doc = XMLProcessor::getElementContent(xml1, "document", &pos);
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
    char *empty = XMLProcessor::getElementContent(doc, "emptytag");
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
