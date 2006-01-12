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

#define TEST_SYNCSOURCE

void testFilter();
void testClause();
void testConfigFilter();
void testEncryption();


#define APPLICATION_URI TEXT("Funambol/examples/dummy")

// Define DEBUG_SETTINGS in your project to create a default configuration
// tree for the test client. WARNING: it will override any previous setting!
// 
#ifdef DEBUG_SETTINGS
void settings();
#endif

#ifdef _WIN32_WCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd ) {
#else
int main(int argc, char** argv) {
#endif
    
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
   settings();
#endif

#ifdef TEST_ENCODE
    wchar_t *content = loadAndConvert(TEXT("message.xml"), TEXT("base64"));
    if(!content) {
        fprintf(stderr, "Error in uudecode.");
        exit(1);
    }
    convertAndSave(TEXT("message_out.xml"), content, TEXT("base64"));
    
    return 0;
#endif

#ifdef TEST_SYNCSOURCE
	Sync4jClient& s4j = Sync4jClient::getInstance();
    s4j.setDMConfig(APPLICATION_URI);
    
	
    TestSyncSource source = TestSyncSource(TEXT("briefcase"));           
    TestSyncSource2 source2 = TestSyncSource2(TEXT("contact"));           
    //TestSyncSource source3 = TestSyncSource(TEXT("notExisting"));           

    SyncSource** ssArray = new SyncSource*[3];
    ssArray[0] = &source;
    ssArray[1] = &source2;
    //ssArray[2] = &source3;

    ssArray[2] = NULL;
    s4j.sync(ssArray);
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

    return 0;
}

