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
#include "base/util/ArrayList.h"

class ArrayListTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(ArrayListTest);
    CPPUNIT_TEST(removeFirst);
    CPPUNIT_TEST(removeLast);
    CPPUNIT_TEST(removeMiddle);
    CPPUNIT_TEST(removeAll);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        StringBuffer a("a"), b("b"), c("c");

        abc.add(a);
        abc.add(b);
        abc.add(c);

        bc.add(b);
        bc.add(c);

        ac.add(a);
        ac.add(c);
        
        ab.add(a);
        ab.add(b);
    }
    void tearDown() {
        abc.clear();
        bc.clear();
        ac.clear();
        ab.clear();
    }

private:
    void removeFirst() {
        ArrayList l = abc;

        l.removeElementAt(0);
        CPPUNIT_ASSERT_EQUAL(2, l.size());
        CPPUNIT_ASSERT(equal(l, bc));
    }

    void removeMiddle() {
        ArrayList l = abc;

        l.removeElementAt(1);
        CPPUNIT_ASSERT_EQUAL(2, l.size());
        CPPUNIT_ASSERT(equal(l, ac));
    }

    void removeLast() {
        ArrayList l = abc;

        l.removeElementAt(2);
        CPPUNIT_ASSERT_EQUAL(2, l.size());
        CPPUNIT_ASSERT(equal(l, ab));
    }

    void removeAll() {
        ArrayList l = abc;
        CPPUNIT_ASSERT_EQUAL(0, l.removeElementAt(0));
        CPPUNIT_ASSERT_EQUAL(1, l.removeElementAt(1));
        CPPUNIT_ASSERT_EQUAL(0, l.removeElementAt(0));
        CPPUNIT_ASSERT(equal(l, empty));
    }

    bool equal(ArrayList &first, ArrayList &second) {
        ArrayElement *first_e = first.front();
        int index = 0;
        ArrayElement *second_e = second.get(index);
        while (true) {
            if (!first_e && !second_e) {
                // end of both lists
                return true;
            }

            if (first_e && !second_e ||
                !first_e && second_e) {
                // different length
                return false;
            }

            if (*(StringBuffer *)first_e != *(StringBuffer *)second_e) {
                // different content
                return false;
            }

            first_e = first.next();
            index++;
            second_e = second.get(index);
        }
    }

    ArrayList abc, bc, ac, ab, empty;
};

CPPUNIT_TEST_SUITE_REGISTRATION( ArrayListTest );
