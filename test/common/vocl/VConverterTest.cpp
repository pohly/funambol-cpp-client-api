/*
 * Funambol is a mobile platform developed by Funambol, Inc.
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 *
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 *
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include <stdlib.h>

#include "testUtils.h"

#include "base/globalsdef.h"
#include "base/fscapi.h"
#include "base/winadapter.h"
#include "base/util/utils.h"
#include "base/util/WString.h"
#include "vocl/VConverter.h"
#include "vocl/VObjectFactory.h"

USE_NAMESPACE

#define BS   TEXT("\\")
#define EBS  TEXT("\\\\")
#define CRLF TEXT("\r\n")

class VConverterTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(VConverterTest);

    CPPUNIT_TEST(testVcard21Unfolding);
    CPPUNIT_TEST(testVcard21Folding);
    CPPUNIT_TEST(testVcard21UnfoldingQP);
    CPPUNIT_TEST(testVcard21FoldingQP);
    CPPUNIT_TEST(testVcard21Unescaping);
    CPPUNIT_TEST(testVcard21Escaping);


    CPPUNIT_TEST(testVcal20Unfolding);
    CPPUNIT_TEST(testVcal20Folding);
    CPPUNIT_TEST(testVcal20UnfoldingQP);
    CPPUNIT_TEST(testVcal20FoldingQP);
    CPPUNIT_TEST(testVcal20Unescaping);
    CPPUNIT_TEST(testVcal20Escaping);

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        this->vobj = NULL;
    }

    void tearDown() {
        if (this->vobj != NULL) {
            delete this->vobj;
            this->vobj = NULL;
        }
    }

private:

    VObject * vobj;

    void checkField(const WString & input, const WString & field, const WString & expected) {

        this->vobj = VConverter::parse(input.c_str());

        VProperty * vprop = vobj->getProperty(field);
        CPPUNIT_ASSERT(vprop != NULL);

        WString actual = vprop->getValue();
        CPPUNIT_ASSERT(actual == expected);
    }

    void createAndCheckField(const WString & pid, const WString & version, const WString & field, const WString & input, WString expected) {

        this->vobj = VObjectFactory::createInstance(pid, version);
        VProperty vprop = VProperty(field, input);
        this->vobj->addProperty(&vprop);

        WCHAR * string = this->vobj->toString();
        WString actual = string;
        delete [] string;

        expected = WString(field).append(expected).append(TEXT("\r\n"));

        CPPUNIT_ASSERT(actual == expected);
    }

    void testVcard21Unfolding() {
        WString input = TEXT("BEGIN:VCARD") CRLF
                        TEXT("VERSION:2.1") CRLF
                        TEXT("N:") CRLF
                        TEXT("NOTE:a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a") CRLF
                        TEXT(" a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a") CRLF
                        TEXT("END:VCARD") CRLF;

        WString expected = TEXT("a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a");
        checkField(input, TEXT("NOTE"), expected);
    }

    void testVcard21Folding() {
        WString input    = TEXT( "a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a");
        WString expected = TEXT(":a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a\r\n  a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a a");

        createAndCheckField(TEXT("VCARD"), TEXT("2.1"), TEXT("NOTE"), input, expected);
    }

    void testVcard21UnfoldingQP() {
        WString input = 
            TEXT("BEGIN:VCARD") CRLF
            TEXT("VERSION:2.1") CRLF
            TEXT("N:") CRLF
            TEXT("NOTE;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:aaaaaaaaaaaaaaaaaaaaaaaaaa=3D=") CRLF
            TEXT("=0D=0A=0D=0Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") CRLF
            TEXT("END:VCARD") CRLF;
        WString expected = TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaa=\r\n\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        checkField(input, TEXT("NOTE"), expected);
    }

    void testVcard21FoldingQP() {

        WString input =
            TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaa=\r\n\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        WString expected =
            TEXT(   ";ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:aaaaaaaaaaaaaaaaaaaaaaaaaa=3D=") CRLF
            TEXT("=0D=0A=0D=0Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=") CRLF
            TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

        createAndCheckField(TEXT("VCARD"), TEXT("2.1"), TEXT("NOTE"), input, expected);
    }

    void testVcard21Unescaping() {
        WString input =
            TEXT("BEGIN:VCARD") CRLF
            TEXT("VERSION:2.1") CRLF
            TEXT("N:") CRLF
            TEXT("NOTE:") EBS BS TEXT(";") BS TEXT(",") BS TEXT("n") BS TEXT("r") CRLF
            TEXT("END:VCARD") CRLF;
        WString expected = TEXT("\\;\\,\\n\\r");
        checkField(input, TEXT("NOTE"), expected);
    }

    void testVcard21Escaping() {
        WString input    =           BS  TEXT(";")    BS  TEXT(",") BS  TEXT("r") BS  TEXT("n");
        WString expected = TEXT(":") EBS BS TEXT(";") EBS TEXT(",") EBS TEXT("r") EBS TEXT("n");

        createAndCheckField(TEXT("VCARD"), TEXT("2.1"), TEXT("NOTE"), input, expected);
    }


    void testVcal20Unfolding() {
        WString input =
            TEXT("BEGIN:VCALENDAR") CRLF
            TEXT("VERSION:2.0") CRLF
            TEXT("BEGIN:VEVENT") CRLF
                //123456789012345678901234567890123456789012345678901234567890123456789012345
            TEXT("DESCRIPTION:aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") CRLF
            TEXT(" aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") CRLF
            TEXT("END:VEVENT") CRLF
            TEXT("END:VCALENDAR") CRLF;

        WString expected = TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        checkField(input, TEXT("DESCRIPTION"), expected);
    }

    void testVcal20Folding() {
        /*
DESCRIPTION:aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        */
        WString input    = TEXT( "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
                    //123456789012345678901234567890123456789012345678901234567890123456789012345    123456789012345678901234567890123456789012345678901234567890123456789012345
                    //DESCRIPTION:
        WString expected = TEXT(":aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\n aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

        createAndCheckField(TEXT("VCALENDAR"), TEXT("2.0"), TEXT("DESCRIPTION"), input, expected);
    }

    void testVcal20UnfoldingQP() {
        WString input = 
            TEXT("BEGIN:VCALENDAR") CRLF
            TEXT("VERSION:2.0") CRLF
            TEXT("BEGIN:VEVENT") CRLF
            //    123456789012345678901234567890123456789012345678901234567890123456789012345
            TEXT("DESCRIPTION;ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:aaaaaaaaaaaaaaaaaa=3D=") CRLF
            //    123456789012345678901234567890123456789012345678901234567890123456789012345
            TEXT("=0D=0A=0D=0Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=3D=") CRLF
            TEXT("=0D=0A=0D=0Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") CRLF
            TEXT("END:VEVENT") CRLF
            TEXT("END:VCALENDAR") CRLF;
        WString expected = TEXT("aaaaaaaaaaaaaaaaaa=\r\n\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=\r\n\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        checkField(input, TEXT("DESCRIPTION"), expected);
    }

    void testVcal20FoldingQP() {

        WString input =
            TEXT("aaaaaaaaaaaaaaaaaaa=\r\n\r\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
        WString expected =
            //    123456789012345678901234567890123456789012345678901234567890123456789012345
            //   DESCRIPTION
            TEXT(          ";ENCODING=QUOTED-PRINTABLE;CHARSET=UTF-8:aaaaaaaaaaaaaaaaaaa=3D=") CRLF
            TEXT("=0D=0A=0D=0Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=3D=") CRLF
            TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

        createAndCheckField(TEXT("VCALENDAR"), TEXT("2.0"), TEXT("DESCRIPTION"), input, expected);
    }

    void testVcal20Unescaping() {
        WString input =
            TEXT("BEGIN:VCALENDAR") CRLF
            TEXT("VERSION:2.0") CRLF
            TEXT("BEGIN:VEVENT") CRLF
            TEXT("DESCRIPTION:") EBS BS TEXT(";") BS TEXT(",") BS TEXT("n") BS TEXT("\N") BS TEXT("r") CRLF
            TEXT("END:VEVENT") CRLF
            TEXT("END:VCALENDAR") CRLF;
        WString expected = BS TEXT(";") TEXT(",") TEXT("\n") TEXT("\n") BS TEXT("r");
        checkField(input, TEXT("DESCRIPTION"), expected);
    }

    void testVcal20Escaping() {
        WString input    =           BS  TEXT(";")    BS  TEXT(",") TEXT("\n");
        WString expected = TEXT(":") EBS BS TEXT(";") EBS BS TEXT(",") BS TEXT("n");

        createAndCheckField(TEXT("VCALENDAR"), TEXT("2.0"), TEXT("DESCRIPTION"), input, expected);
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( VConverterTest );

