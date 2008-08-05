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

#include "base/globalsdef.h"
#include "base/util/PropertyFile.h"

USE_NAMESPACE

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class PropertyFileTest : public CppUnit::TestFixture {
    CPPUNIT_TEST_SUITE(PropertyFileTest);
    CPPUNIT_TEST(testSetProperty);
    CPPUNIT_TEST(testGetProperties);
    CPPUNIT_TEST(testRemoveProperty);
    CPPUNIT_TEST(testRemoveAll);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() {
        propFile = new PropertyFile("test.properties");
        CPPUNIT_ASSERT(propFile);
    }

    void tearDown() {
    }

protected:
    void testSetProperty() {
        // FIXME: the value returned by setPropertyValue is always 0?

        // Write a value into the property file
        propFile->setPropertyValue("property", "value");
        propFile->save();
        // Now read back
        StringBuffer value = propFile->readPropertyValue("property");
        CPPUNIT_ASSERT(value == "value");
    }

    void testGetProperties() {
        // Add three properties
        propFile->setPropertyValue("property1", "value1");
        propFile->setPropertyValue("property2", "value2");
        propFile->setPropertyValue("property3", "value3");
        // Now iterate on them
        Enumeration& enumeration = propFile->getProperties();
        bool prop1 = false, prop2 = false, prop3 = false;
        while (enumeration.hasMoreElement()) {
            KeyValuePair* item = (KeyValuePair*)enumeration.getNextElement();
            const char* key = item->getKey();
            const char* value = item->getValue();
            CPPUNIT_ASSERT(value != NULL && strlen(value) > 0);
            if (strcmp(key, "property1") == 0) {
                prop1 = true;
            } else if (strcmp(key, "property2") == 0) {
                prop2 = true;
            } else if (strcmp(key, "property3") == 0) {
                prop3 = true;
            }
        }
        CPPUNIT_ASSERT(prop1 && prop2 && prop3);
    }

    void testRemoveProperty() {
        // Write a value into the property file
        propFile->setPropertyValue("property4", "value4");
        propFile->save();
        // Remove it
        int success = propFile->removeProperty("property4");
        CPPUNIT_ASSERT(success == 0);
        // Now read back
        StringBuffer value = propFile->readPropertyValue("property4");
        CPPUNIT_ASSERT(value.empty());
    }

    void testRemoveAll() {
        // Iterate on all properties
        Enumeration& enumeration = propFile->getProperties();
        while (enumeration.hasMoreElement()) {
            KeyValuePair* item = (KeyValuePair*)enumeration.getNextElement();
            const char* key = item->getKey();
            int success = propFile->removeProperty(key);
            CPPUNIT_ASSERT(success == 0);
        }
        // Now iterate again, and we should have now more elements
        enumeration = propFile->getProperties();
        CPPUNIT_ASSERT(!enumeration.hasMoreElement());
    }


private:
    PropertyFile *propFile;
};

CPPUNIT_TEST_SUITE_REGISTRATION( PropertyFileTest );

