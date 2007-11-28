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


#include "base/util/KeyValuePair.h"
#include "spdm/ManagementObject.h"


ManagementObject::ManagementObject( const WCHAR* context,
                                    const WCHAR* name   )
: LeafManagementNode (context, name) {
}

ManagementObject::~ManagementObject() {
}

void ManagementObject::getPropertyValue(const WCHAR* p, WCHAR*v, int size) {
    KeyValuePair* property = NULL;

    int l = properties.size();
    for (int i=0; i<l; ++i) {
        property = (KeyValuePair*)properties.get(i);
        if (wcscmp(property->getKey(), p) == 0) {
            wcsncpy(v, property->getValue(), size);
            return;
        }
    }

}

void ManagementObject::setPropertyValue(const WCHAR* p, const WCHAR*v) {
    KeyValuePair property((WCHAR*)p, v);

    properties.add(property);
}

ArrayElement* ManagementObject::clone() {
    ManagementObject* ret = new ManagementObject(context, name);

    KeyValuePair* property = NULL;

    int l = properties.size();
    for (int i=0; i<l; ++i) {
        property = (KeyValuePair*)properties.get(i);
        ret->setPropertyValue(property->getKey(), property->getValue());
    }

    return ret;
}


ArrayList& ManagementObject::getProperties() {
    return properties;
}
