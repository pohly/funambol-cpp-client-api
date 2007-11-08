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

#include "base/util/BasicTime.h"

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

class BasicTimeTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(BasicTimeTest);
    CPPUNIT_TEST(testEqual);
    CPPUNIT_TEST(testConversion);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        // millenium.set(2000, 01, 01, 6,
        //              00, 00, 00,
        //              00, 00);
        millenium.setYear(2000);
        buffer = NULL;
    }
    void tearDown() {
        if (buffer) {
            delete [] buffer;
        }
    }

protected:
    void testEqual() {
        BasicTime empty;
        CPPUNIT_ASSERT(empty != millenium);

        BasicTime copy(millenium);
        CPPUNIT_ASSERT(millenium == copy);
        copy = millenium;
        CPPUNIT_ASSERT(millenium == copy);
    }

    void testConversion() {
        buffer = millenium.formatRfc822();

        BasicTime copy;
        CPPUNIT_ASSERT_EQUAL(0, copy.parseRfc822(buffer));
        CPPUNIT_ASSERT(millenium == copy);
        delete [] buffer; buffer = NULL;

        CPPUNIT_ASSERT_EQUAL(-1, copy.parseRfc822("this is garbage"));

        static const char convertStr[] = "Mon, 6 Nov 2006 20:30:15 +0100";
        BasicTime convert;
        CPPUNIT_ASSERT_EQUAL(0, convert.parseRfc822(convertStr));
        buffer = convert.formatRfc822();
        CPPUNIT_ASSERT(!strcmp(buffer, convertStr));
        delete [] buffer; buffer = NULL;
    }

private:
    BasicTime millenium;
    char *buffer;
};

CPPUNIT_TEST_SUITE_REGISTRATION( BasicTimeTest );
