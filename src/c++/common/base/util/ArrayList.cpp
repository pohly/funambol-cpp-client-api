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


#include "base/util/ArrayList.h"

/**
 * This class implements a simple linked list that can be accessed by index too.
 * This class does not make use of C++ templates by choice, since it must be
 * as much easier and portable as possible.
 *
 * Since this class works with pointers, it can be instructed to delete the
 * list elements at object destruction. If the property autoDeleteElements is
 * TRUE all elements are delete using the C++ delete operator (therefore, make
 * sure elements are allocated with a compatible memory allocation function -
 * such as the C++ new operator)
 */

ArrayList::ArrayList() : head(0), iterator(0), lastElement(0) {
    count = 0;
}

ArrayList::ArrayList(const ArrayList &other) {
    count = 0;
    head = iterator = lastElement = 0;
    for (Element *p = other.head; p; p = p->n) {
        add( *(p->e) );
    }
}

ArrayList::~ArrayList() {
    clear();
}

/**
 * Is this list empty?
 */
bool ArrayList::isEmpty() {
    return (head == NULL);
}

/**
 * Adds a new element at the specified index. If index is greater than
 * the list size the element is appended.
 * The element is dinamically duplicated so that the caller can release
 * the element at any time. This method returns the position (0 based) at which
 * the element has been inserted. It can be different by index if index is out
 * of the array bounds, in that case element is appended as last element.
 * It returns -1 in case of errors.
 *
 * @param index the insertion position
 * @param element the element to insert - NULL is not allowed!
 */
int ArrayList::add(int index, ArrayElement& element) {
    if (index < 0) {
        return -1;
    }

    int s = size();
    if (index > s) {
        index = s;
    }

    Element* newElement = new Element();

    newElement->e = element.clone();
    newElement->n = NULL;

    Element* e;
    if (index == s) {
        // Inserting the new element at the end
        e = lastElement;

    }
    else {
    // Inserting the new element at the index-th position
        e = head;
    for (int i=0; i<index-1; ++i) {
        e = e->n;
    }
    }

    if (e == NULL || index == 0) {
        //
        // Insertion at the beginning of the array
        //
        newElement->n = head;
        head = newElement;

        if (e == NULL) {
            lastElement = newElement;
        }
        count++;
        return index;
    }

    if (e->n) {
        //
        // Insertion in the middle of the array
        //
        newElement->n = e->n;
        e->n = newElement;
    } else {
        //
        // Insertion at the end of the array
        //
        e->n = newElement;
        lastElement = newElement;
    }

    count++;

    return index;
}

/**
 * Same as add(index, element), but appending at the end of the array.
 *
 * @param element the element to insert
 */
int ArrayList::add(ArrayElement& element) {
    return add(size(), element);
}

/**
* Add all the ArrayElement of the given ArrayList to the current
* array list
*/
int ArrayList::add(ArrayList* list) {
    int ret = 0;
    for (int i = 0; i < list->size(); i++) {
        ret = ret + add(*(ArrayElement*)list->get(i));
    }
    return ret;
}


int ArrayList::removeElementAt(int index) {

    int s = size() - 1;

    if (index > s) {
        index = s;
    }

    if (index < 0) {
        return -1;
    }

    //
    // deleting the existing element at the index-th position
    //
    Element* e = head;
    Element* before = NULL;

    for (int i=0; i<index; ++i) {
        before = e;
        e = e->n;
    }

    if (before) {
        before->n = e->n;
    } else {
        assert(index == 0);
        head = e->n;
    }
    if (e == lastElement) {
        assert(index == count - 1);
        lastElement = before;
    }

    delete e;
    e = NULL;

    count--;
    return index;
}

/**
 * Frees the list. All elements are freed as well.
 */
void ArrayList::clear()
{
    Element* p = head;
    while (p) {
        delete p->e;
        head = p->n;
        delete p;
        p = head;
    }
    count = 0;
    head = NULL;
    lastElement = NULL;
}

/**
 * Returns the index-th element of the array or NULL if index is out of
 * the array bounds.
 *
 * @param index the element position
 */
ArrayElement* ArrayList::get(int index) {

    if ( index<0 ) {
        return NULL;
    }

    Element* e = head;
    for(int i=0; e && i<index; ++i) {
        e = e->n;
    }
    if(!e)
        return NULL;
    else
        return e->e;
}

/**
 * Returns the array size.
 */
int ArrayList::size() {
/*    Element *e = head;
    int i = 0;
    while (e) {
        ++i;
        e = e->n;
    }

    return i;*/
    return count;
}

ArrayElement* ArrayList::front() {
    iterator = head;
    return (iterator) ? iterator->e : 0 ;
}
ArrayElement* ArrayList::next() {
    if(!iterator) {
        return 0;
    }
    iterator = iterator->n;
    return (iterator) ? iterator->e : 0 ;
}

ArrayElement* ArrayList::prev() {
    Element *e;

    if( !iterator)
        return 0;
    if(iterator == head)
        return 0;
    for(e = head; e->n == iterator; e = e->n) {
        if(!e)
            return 0;
    }
    return e->e;
}

ArrayElement* ArrayList::back() {
    for(iterator = head; iterator->n; iterator = iterator->n);
    return iterator->e;
}

/**
 * Same as get(index)
 */
ArrayElement* ArrayList::operator[] (int index) {
    return get(index);
}

ArrayList& ArrayList::operator= (const ArrayList &v) {
    clear();
    for (Element *p = v.head; p; p = p->n) {
        add( *(p->e) );
    }
    return *this;
}

ArrayList* ArrayList::clone() {

    ArrayList* ret = new ArrayList();
    int dim = size();
    for (int i = 0; i < dim; i++) {

        ret->add(*((ArrayElement*)get(i)));
    }
    return ret;

}

#ifdef ENABLE_UNIT_TESTS

#include "base/util/StringBuffer.h"

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

FUNAMBOL_TEST_SUITE_REGISTRATION(ArrayListTest);

#endif
