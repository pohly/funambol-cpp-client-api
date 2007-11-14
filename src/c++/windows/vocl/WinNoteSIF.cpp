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

#include "vocl/WinNoteSIF.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
#include "vocl/sifUtils.h"

using namespace std;


// Constructor
WinNoteSIF::WinNoteSIF() {
    sifFields = NULL;
    sif = L"";
}

// Constructor: fills propertyMap parsing the passed SIF string
WinNoteSIF::WinNoteSIF(const wstring dataString, const wchar_t** fields) {
    
    sif = L"";
    sifFields = fields;
    parse(dataString);
}

// Destructor
WinNoteSIF::~WinNoteSIF() {
}

wstring& WinNoteSIF::toString() {
    
    wstring propertyValue;
    sif = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";   
    sif += L"<note>\n";

    sif += L"<SIFVersion>";
    sif += SIF_VERSION;
    sif += L"</SIFVersion>\n";

    //
    // Add all task properties
    //
    map<wstring,wstring>::iterator it = propertyMap.begin();       
    while (it != propertyMap.end()) {        
        propertyValue = it->second;
        addPropertyToSIF(sif, it->first, propertyValue);
        it ++;
    }

    sif += L"</note>";

    return sif;
}



int WinNoteSIF::parse(const wstring data) {
    
    if (!sifFields) {
        LOG.error(ERR_SIFFIELDS_NULL);
        return 1;
    }
    propertyMap.clear();
    
    // Check if <note> tag is present...
    wstring::size_type pos = 0;
    wstring itemTypeTag = L"<note>";    
    pos = data.find(itemTypeTag, 0);
    if (pos == wstring::npos) {
        LOG.error("Tag '%ls' not found", itemTypeTag.c_str());
        return 1;
    }
    wstring propertyValue;

    //
    // Set task properties
    //
    for (int i=0; sifFields[i]; i++) {
        // Set only properties found!
        if (!getElementContent(data, sifFields[i], propertyValue, 0)) {

            replaceAll(L"&lt;",  L"<", propertyValue);
            replaceAll(L"&gt;",  L">", propertyValue);
            replaceAll(L"&amp;", L"&", propertyValue);
            
            setProperty(sifFields[i], propertyValue);
        }
    }

    return 0;
}
