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


#include "base/util/KeyValuePair.h"
#include "spdm/ManagementObject.h"


ManagementObject::ManagementObject( const wchar_t* context,
                                    const wchar_t* name   )
: LeafManagementNode (context, name) {
}

ManagementObject::~ManagementObject() {
}

void ManagementObject::getPropertyValue(const wchar_t* p, wchar_t* v, int size) {
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

void ManagementObject::setPropertyValue(const wchar_t* p, wchar_t* v) {
    KeyValuePair property((wchar_t*)p, v);

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
