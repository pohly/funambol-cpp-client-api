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

#include "vocl/WinItem.h"

using namespace std;

// Init static member.
wstring WinItem::badString = L"<NULL>";


// Constructor
WinItem::WinItem() {}

// Destructor
WinItem::~WinItem() {}




// Sets a value into propertyMap.
void WinItem::setProperty(const wstring propertyName, const wstring propertyValue) {
    propertyMap[propertyName] = propertyValue;
}


// Gets a value from propertyMap.
bool WinItem::getProperty(const wstring propertyName, wstring& propertyValue){

    map<wstring,wstring>::iterator it = propertyMap.find(propertyName);
    if (it != propertyMap.end()) {
        // Found
        propertyValue = it->second;
        return true;
    }
    else {
        // Not found
        propertyValue = L"";
        return false;
    }
}

// Gets a value from propertyMap.
wstring& WinItem::getPropertyRef(const wstring propertyName, bool* found) {

    map<wstring,wstring>::iterator it = propertyMap.find(propertyName);
    if (it != propertyMap.end()) {
        // Found
        *found = true;
        return it->second;
    }
    else {
        // Not found
        *found = false;
        return badString;
    }
}


int WinItem::getPropertyMapSize() {
    return propertyMap.size();
}

void WinItem::resetPropertyMap() {
    propertyMap.clear();
}


void WinItem::resetAllValues() {

    map<wstring,wstring>::iterator it = propertyMap.begin();
    while (it != propertyMap.end()) {
        it->second = L"";
        it ++;
    }
}
