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

using namespace std;


class ParserTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(ParserTest);
    CPPUNIT_TEST(roundTripTest1);
    CPPUNIT_TEST(roundTripTest2);
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
    //Sync Body Test
    CPPUNIT_TEST(testCmdID);
    CPPUNIT_TEST(testCmd);
    CPPUNIT_TEST(testMsgRef);
    CPPUNIT_TEST(testCmdRef);
    CPPUNIT_TEST(testData);
    //CPPUNIT_TEST(testTargetRef);
    //End Sync Body Test
    CPPUNIT_TEST_SUITE_END();

public:
    char*       xml;
    size_t      xmlLen;
    
    SyncHdr*    SH;

    void setUp() {
        xml = 0;
        SH = 0;

        if(!readFile("./ex1.xml", &xml, &xmlLen, true)){
            cout << "\nFailed to load XML";
        }
        else {
            SH = Parser::getSyncHdr(xml);
        }
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


    void setUpXMLFile(const char* fileName, wstring* amsg, wstring* pmsg){
        char*       message;
        size_t      len;
        size_t u;

        wstring a;
        wstring p;

        if(!readFile(fileName, &message, &len, false)){
            cout << "\nFailed to load XML";
            return;
        }

        wchar_t* messageW = toWideChar(message);

        p.assign ( messageW ) ;

        while ((u = p.find(TEXT(">\r\n<"))) != -1 ){
            p.erase(u+1,2);
        }
      
        while ((u = p.find(TEXT(">\n<"))) != -1 ){
            p.erase(u+1,1);
        }

        while ((u = p.find(TEXT("</SyncML>\r\n"))) != -1 ){
            p.erase(u+9,2);
        }
        while ((u = p.find(TEXT("</SyncML>\n"))) != -1 ){
            p.erase(u+9,1);
        }


        while ((u = p.find(TEXT("'"))) != -1 ){
            p.erase(u,1);
            p.insert(u,TEXT("\""));
        }
             
        char* pmsgC = toMultibyte( p.c_str() );



        SyncML* syncml      = Parser::getSyncML( pmsgC );

        StringBuffer *s     = Formatter::getSyncML(syncml);

        wchar_t* amsgC = toWideChar(s->c_str());

        a.assign( amsgC );

        while ((u = a.find(TEXT(">\r\n<"))) != -1 ){
            a.erase(u+1,2);
        }
        while ((u = a.find(TEXT(">\n<"))) != -1 ){
            a.erase(u+1,1);
        }

        amsg = &a;
        pmsg = &p;

        if (s){
            delete [] s;
        }
        if (pmsgC) {
            delete [] pmsgC;
        }
        if (amsgC) {
            delete [] amsgC;
        }
        if (message) {
            delete [] message;
        }
        if (messageW) {
            delete [] messageW;
        }
    }

    wstring testTag(WCHAR* wtag){

        if(!xml) {
            return wstring(TEXT(""));
        }

        wstring msg = toWideChar(xml);
        wstring tag = wtag;
        wstring result;

        int res = getElementContent(msg, tag, result);

        return result;
    }

    void roundTripTest1(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./ex1.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripTest2(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./ex2.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }
    void roundTripTestWrong(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./wrongex.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg != pmsg );
    }

    void roundTripsml1(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML1.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripsml2(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML2.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripsml3(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML3.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripsml4(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML4.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripsml5(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML5.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripsml6(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML6.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripsml7(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML7.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void roundTripsml8(){

        wstring amsg;
        wstring pmsg;

        setUpXMLFile("./syncML8.xml", &amsg, &pmsg);

        CPPUNIT_ASSERT( amsg == pmsg );
    }

    void testVerDTD(){
        wstring result = testTag(TEXT("VerDTD"));
        CPPUNIT_ASSERT(wcscmp(toWideChar(SH->getVerDTD()->getValue()),result.c_str())==0);
    }

    void testVerProto(){
        wstring result = testTag(TEXT("VerProto"));
        CPPUNIT_ASSERT(wcscmp(toWideChar(SH->getVerProto()->getVersion()),result.c_str())==0);
    }

    void testSessionID(){
        wstring result = testTag(TEXT("SessionID"));
        CPPUNIT_ASSERT(wcscmp(toWideChar(SH->getSessionID()->getSessionID()),result.c_str())==0);
    }

    void testMsgID(){
        wstring result = testTag(TEXT("MsgID"));
        CPPUNIT_ASSERT(wcscmp(toWideChar(SH->getMsgID()),result.c_str())==0);
    }

    void testTarget(){
        wstring target = testTag(TEXT("Target"));
        wstring tag = TEXT("LocURI");
        wstring result;
        int i = getElementContent(target, tag, result);
        CPPUNIT_ASSERT(wcscmp(toWideChar(SH->getTarget()->getLocURI()),result.c_str())==0);
    }

    void testSource(){
        wstring target = testTag(TEXT("Source"));
        wstring tag = TEXT("LocURI");
        wstring result;
        int i = getElementContent(target, tag, result);
        CPPUNIT_ASSERT(wcscmp(toWideChar(SH->getSource()->getLocURI()),result.c_str())==0);
    }

    void testCmdID(){
        wstring target = testTag(TEXT("Status"));
        wstring tag = TEXT("CmdID");
        wstring result;
        int i = getElementContent(target, tag, result);
        CPPUNIT_ASSERT(wcscmp(toWideChar(Parser::getStatus(xml)->getCmdID()->getCmdID()),result.c_str())==0);
    }

    void testCmd(){
        wstring target = testTag(TEXT("Status"));

        wstring tag = TEXT("Cmd");
        int pos = (int)target.find(TEXT("<Cmd>"));
        wstring result;
        int i = getElementContent(target, tag, result, pos);
        CPPUNIT_ASSERT(wcscmp(toWideChar(Parser::getStatus(xml)->getCmd()),result.c_str())==0);
    }

    void testMsgRef(){
        wstring target = testTag(TEXT("Status"));
        wstring tag = TEXT("MsgRef");
        wstring result;
        int i = getElementContent(target, tag, result);
        CPPUNIT_ASSERT(wcscmp(toWideChar(Parser::getStatus(xml)->getMsgRef()),result.c_str())==0);
    }
    void testCmdRef(){
        wstring target = testTag(TEXT("Status"));
        wstring tag = TEXT("CmdRef");
        wstring result;
        int i = getElementContent(target, tag, result);
        CPPUNIT_ASSERT(wcscmp(toWideChar(Parser::getStatus(xml)->getCmdRef()),result.c_str())==0);
    }
    
    /*
    void testTargetRef(){
        wstring target = testTag(TEXT("Status"));
        wstring tag = TEXT("TargetRef");
        wstring result;
        int i = getElementContent(target, tag, result);

        CPPUNIT_ASSERT(wcscmp(toWideChar(Parser::getTargetRef(xml)->getValue()),result.c_str())==0);
    }
    void testSourceRef(){
        wstring target = testTag(TEXT("Status"));
        wstring tag = TEXT("SourceRef");
        wstring result;
        int i = getElementContent(target, tag, result);
        CPPUNIT_ASSERT(wcscmp(toWideChar(Parser::getStatus(xml)->getCmdRef()),result.c_str())==0);
    }*/

    void testData(){
        wstring target = testTag(TEXT("Status"));
        wstring tag = TEXT("Data");
        wstring result;
        int i = getElementContent(target, tag, result);
        CPPUNIT_ASSERT(wcscmp(toWideChar(Parser::getStatus(xml)->getData()->getData()),result.c_str())==0);
    }

};



CPPUNIT_TEST_SUITE_REGISTRATION( ParserTest );

