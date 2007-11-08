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

#include "base/util/StringBuffer.h"

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

/**
 * Test case for the class StringBuffer.
 */
class StringBufferTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(StringBufferTest);
    CPPUNIT_TEST(testSprintf);
    CPPUNIT_TEST_SUITE_END();

private:
    void testSprintf() {
        StringBuffer buf;

        buf.sprintf("foo %s %d", "bar", 42);
        CPPUNIT_ASSERT(buf == "foo bar 42");

        buf = doSprintf("foo %s %d", "bar", 42);
        CPPUNIT_ASSERT(buf == "foo bar 42");

        for (unsigned long size = 1; size < (1<<10); size *= 2) {
            buf.sprintf("%*s", (int)size, "");
            CPPUNIT_ASSERT_EQUAL(size, buf.length());
        }
    }

    StringBuffer doSprintf(const char* format, ...) {
        va_list ap;
        StringBuffer buf;

        va_start(ap, format);
        buf.vsprintf(format, ap);
        va_end(ap);

        return buf;
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( StringBufferTest );

