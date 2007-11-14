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

#include "base/util/utils.h"
#include "base/stringUtils.h"
#include "vocl/WinRecurrenceSIF.h"
#include "vocl/constants.h"
#include "vocl/sifUtils.h"
#include <oleauto.h>

using namespace std;


// Constructor
WinRecurrenceSIF::WinRecurrenceSIF() {
    sifFields = NULL;
    sif = L"";
}

// Constructor: fills propertyMap parsing the passed SIF
WinRecurrenceSIF::WinRecurrenceSIF(const wstring dataString, const wchar_t **fields) {
    sif = L"";
    sifFields = fields;
    parse(dataString);
}

// Destructor
WinRecurrenceSIF::~WinRecurrenceSIF() {
}


void WinRecurrenceSIF::setSifFields(const wchar_t** fields) {
    sifFields = fields;
}


// Format and return a SIF string from the propertyMap.
wstring& WinRecurrenceSIF::toString() {

    wstring propertyValue;
    sif = L"";

    //
    // Add all recurrence properties
    //
    map<wstring,wstring>::iterator it = propertyMap.begin();       
    while (it != propertyMap.end()) {        
        propertyValue = it->second;      
        propertyValue = adaptToSIFSpecs(it->first, propertyValue);
        addPropertyToSIF(sif, it->first, propertyValue);
        it ++;
    }

    return sif;
}




// Parse a SIF string and fills the propertyMap.
int WinRecurrenceSIF::parse(const wstring dataString) {

    if (!sifFields) {
        LOG.error(ERR_SIFFIELDS_NULL);
        return 1;
    }
    propertyMap.clear();
    wstring propertyValue;
       
    for (int i=0; sifFields[i]; i++) {
        // Set only properties found!
        if (!getElementContent(dataString, sifFields[i], propertyValue, 0)) {

            replaceAll(L"&lt;",  L"<", propertyValue);
            replaceAll(L"&gt;",  L">", propertyValue);
            replaceAll(L"&amp;", L"&", propertyValue);
            
            setProperty(sifFields[i], propertyValue);
        }
    } 

    return 0;
}


wstring WinRecurrenceSIF::adaptToSIFSpecs(const wstring& propName, const wstring& propValue) {
    
    wstring propertyValue = L"";

    // "PatternEndDate" is always in UTC format "YYYYMMDDThhmmssZ"
    if (propValue.length() == 8 && propName == L"PatternStartDate") {
        propertyValue = formatDateWithMinus(propValue);    
    }

    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;
}
