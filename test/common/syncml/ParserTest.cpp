/*
* Copyright (C) 2007 Funambol, Inc.
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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/util/StringBuffer.h"

#include "syncml/formatter/Formatter.h"
#include "syncml/parser/Parser.h"
#include "base/util/utils.h"
#include "base/stringUtils.h"

class ParserTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ParserTest);
    // Basic tests
    CPPUNIT_TEST(roundTripTest1);
    CPPUNIT_TEST(roundTripTest2);
    CPPUNIT_TEST(wrongMessageTest);
    // Session test
    CPPUNIT_TEST(roundTripsml1);
    CPPUNIT_TEST(roundTripsml2);
    CPPUNIT_TEST(roundTripsml3);
    CPPUNIT_TEST(roundTripsml4);
    CPPUNIT_TEST(roundTripsml5);
    CPPUNIT_TEST(roundTripsml6);
    CPPUNIT_TEST(roundTripsml7);
    CPPUNIT_TEST(roundTripsml8);
    //SyncHdr Test
    CPPUNIT_TEST(testVerDTD);
    CPPUNIT_TEST(testVerProto);
    CPPUNIT_TEST(testSessionID);
    CPPUNIT_TEST(testMsgID);
    CPPUNIT_TEST(testTarget);
    CPPUNIT_TEST(testSource);
    //End SyncHdr Test
    CPPUNIT_TEST_SUITE_END();

public:
    char*       xml;
    size_t      xmlLen;
    
    SyncHdr*    SH;

    void setUp() {
        xml = 0;
        SH = 0;

        bool fileLoaded = readFile("ex1.xml", &xml, &xmlLen, false);
        CPPUNIT_ASSERT_MESSAGE("Failed to load XML", fileLoaded);

        SH = Parser::getSyncHdr(xml);
    }

    void tearDown() {    
        if (xml){
            delete [] xml;
        }
        if (SH){
            delete SH;
        }    
    }

private:

    /**
     * Load a SyncML message from file, parse and reformat it
     * and return the original message and the converted one.
     */
    void loadTestFile(const char* fileName, StringBuffer& ret) {
        char*       message;
        size_t      len;

        bool fileLoaded = readFile(fileName, &message, &len, false);
        CPPUNIT_ASSERT_MESSAGE("Failed to load XML", fileLoaded);
           
        ret = message ;

        delete [] message;
    }

    /**
     * Convert the message in, parsing it into a SyncML object
     * and formatting back to string out.
     */
    void convertMessage(const StringBuffer& in, StringBuffer& out) {

        SyncML* syncml   = Parser::getSyncML( in.c_str() );
        StringBuffer *s  = Formatter::getSyncML(syncml);

        CPPUNIT_ASSERT( s != 0);
        // Copy result
        out = *s ;
        delete s;
    }

    /*
     * Compare two messages ignoring newlines
     */
    bool compareMessages(const StringBuffer &msg1, const StringBuffer &msg2){
        StringBuffer cmp1(msg1), cmp2(msg2);
        // Remove newlines
        cmp1.replaceAll("\n", "");
        cmp2.replaceAll("\n", "");
        // Uniform quotes to the one produced by the formatter
        cmp1.replaceAll("\'", "\"");

        // Compare the modified strings
        return cmp1 == cmp2;
    }

    /**
     * Test the given tag content against refValue.
     */
    void testTag(const char *tag, const char *refValue){
        // Check input
        CPPUNIT_ASSERT(xml);
        CPPUNIT_ASSERT(tag);
        CPPUNIT_ASSERT(refValue);
        // Get tag
        char *value = XMLProcessor::copyElementContent(xml, tag);
        // Check output
        CPPUNIT_ASSERT(value);
        // Compare values
        CPPUNIT_ASSERT( strcmp(value, refValue)==0 );

        // Free memory
        delete value;
    }

    void roundTripTest(const char *filename){
        StringBuffer orig, conv;

        loadTestFile(filename, orig);
        convertMessage(orig, conv);

        CPPUNIT_ASSERT( compareMessages(orig, conv) );
    }

    /* ------------------------------------------------------ TestCases */


    void roundTripTest1(){
        roundTripTest("ex1.xml");
    }

    void roundTripTest2(){
        roundTripTest("ex2.xml");
    }

    void wrongMessageTest(){
        StringBuffer orig, conv;

        loadTestFile("wrongex.xml", orig);
        convertMessage(orig, conv);
        // IS THIS THE RIGHT TEST?
        CPPUNIT_ASSERT( !compareMessages(orig, conv) );
    }

    void roundTripsml1(){
        roundTripTest("syncML1.xml");
    }

    void roundTripsml2(){
        roundTripTest("syncML2.xml");
    }

    void roundTripsml3(){
        roundTripTest("syncML3.xml");
    }

    void roundTripsml4(){
        roundTripTest("syncML4.xml");
    }

    void roundTripsml5(){
        roundTripTest("syncML5.xml");
    }

    void roundTripsml6(){
        roundTripTest("syncML6.xml");
    }

    void roundTripsml7(){
        roundTripTest("syncML7.xml");
    }

    void roundTripsml8(){
        roundTripTest("syncML8.xml");
    }

    void testVerDTD(){
        testTag("VerDTD", SH->getVerDTD()->getValue());
    }

    void testVerProto(){
        testTag("VerProto", SH->getVerProto()->getVersion());
    }

    void testSessionID(){
        testTag("SessionID", SH->getSessionID()->getSessionID());
    }

    void testMsgID(){
        testTag("MsgID", SH->getMsgID());
    }

    void testTarget(){
        CPPUNIT_ASSERT(xml);
        char *target = XMLProcessor::copyElementContent(xml, "Target");
        CPPUNIT_ASSERT(target);
        char *locuri = XMLProcessor::copyElementContent(target, "LocURI");
        CPPUNIT_ASSERT(locuri);
        // Compare result
        CPPUNIT_ASSERT(strcmp(SH->getTarget()->getLocURI(),locuri)==0);
        // Free memory
        delete [] target;
        delete [] locuri;
    }

    void testSource(){
        CPPUNIT_ASSERT(xml);
        char *source = XMLProcessor::copyElementContent(xml, "Source");
        CPPUNIT_ASSERT(source);
        char *locuri = XMLProcessor::copyElementContent(source, "LocURI");
        CPPUNIT_ASSERT(locuri);
        // Compare result
        CPPUNIT_ASSERT(strcmp(SH->getSource()->getLocURI(),locuri)==0);
        // Free memory
        delete [] source;
        delete [] locuri;
    }


};



CPPUNIT_TEST_SUITE_REGISTRATION( ParserTest );

