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

#include "vocl/WinTaskSIF.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
#include "vocl/sifUtils.h"

using namespace std;


// Constructor
WinTaskSIF::WinTaskSIF() {
    sifFields = NULL;
    sif = L"";
}

// Constructor: fills propertyMap parsing the passed SIF string
WinTaskSIF::WinTaskSIF(const wstring dataString, const wchar_t** fields, const wchar_t** recFields) {
    
    sif = L"";
    sifFields = fields;
    recPatternSIF.setSifFields(recFields);

    parse(dataString);
}

// Destructor
WinTaskSIF::~WinTaskSIF() {
}

wstring& WinTaskSIF::toString() {
    
    wstring propertyValue;
    sif = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";   
    sif += L"<task>\n";

    sif += L"<SIFVersion>";
    sif += SIF_VERSION;
    sif += L"</SIFVersion>\n";

    //
    // Add all task properties
    //
    map<wstring,wstring>::iterator it = propertyMap.begin();       
    while (it != propertyMap.end()) {        
        propertyValue = it->second;      
        propertyValue = adaptToSIFSpecs(it->first, propertyValue);
        addPropertyToSIF(sif, it->first, propertyValue);
        it ++;
    }

    //
    // Append recurrence properties
    //
    wstring element;
    if (getProperty(L"IsRecurring", element)) {
        wstring recurrenceSIF = recPatternSIF.toString();
        if(recurrenceSIF != L"") {
            sif += recurrenceSIF;
        }
    }

    sif += L"</task>";

    return sif;
}



int WinTaskSIF::parse(const wstring data) {
    
    if (!sifFields) {
        LOG.error(ERR_SIFFIELDS_NULL);
        return 1;
    }
    propertyMap.clear();
    
    // Check if <task> tag is present...
    wstring::size_type pos = 0;
    wstring itemTypeTag = L"<task>";    
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

    //
    // Set recurrence properties
    //
    wstring element;
    if (getProperty(L"IsRecurring", element)) {
        BOOL isRec = _wtoi(element.c_str());
        if(isRec) {
            // Fill recPatternSIF propertyMap.
            recPatternSIF.parse(data);
        }
    }

    return 0;
}


WinRecurrenceSIF* WinTaskSIF::getRecPattern() {
    return &recPatternSIF;
}



wstring WinTaskSIF::adaptToSIFSpecs(const wstring& propName, const wstring& propValue) {
    
    wstring propertyValue = L"";

    if (propValue.length() == 8) {
        if ( propName == L"DateCompleted" || 
             propName == L"DueDate"       ||
             propName == L"StartDate" ) {
            propertyValue = formatDateWithMinus(propValue);    
        }
    }

    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;
}

