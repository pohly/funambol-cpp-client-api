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

#include "vocl/WinContactSIF.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
#include "vocl/sifUtils.h"

using namespace std;


// Constructor
WinContactSIF::WinContactSIF() {
    sifFields = NULL;
    sif = L"";
}

// Constructor: fills propertyMap parsing the passed SIF string
WinContactSIF::WinContactSIF(const wstring dataString, const wchar_t **fields) {
    sif = L"";
    sifFields = fields;
    parse(dataString);
}

// Destructor
WinContactSIF::~WinContactSIF() {
}

wstring& WinContactSIF::toString() {
    
    wstring propertyValue, propertyKey;
    sif = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";   
    sif += L"<contact>\n";

    sif += L"<SIFVersion>";
    sif += SIF_VERSION;
    sif += L"</SIFVersion>\n";

    map<wstring,wstring>::iterator it = propertyMap.begin();       
    while (it != propertyMap.end()) {        
        propertyValue = it->second;      
        propertyValue = adaptToSIFSpecs(it->first, propertyValue);
        if ((it->first) == L"Photo") {
            addPhotoToSIF(propertyValue);       // To manage the TYPE attribute
        }
        else {
            addPropertyToSIF(sif, it->first, propertyValue);
        }
        it ++;
    }
    sif += L"</contact>";

    return sif;
}



void WinContactSIF::addPhotoToSIF(wstring propertyValue) {

    if (photoType.length() == 0) {
        // Type not specified
        addPropertyToSIF(sif, L"Photo", propertyValue);
        return;
    }

    if (propertyValue != L"") {
        sif += L"<Photo TYPE=\"";
        sif += photoType;
        sif += L"\">";
        sif += propertyValue;
        sif += L"</Photo>\n";
    }
    else {
        sif += L"<Photo/>\n";
    }
}



wstring WinContactSIF::adaptToSIFSpecs(const wstring& propName, const wstring& propValue) {
    
    wstring propertyValue = L"";

    if ((propName == L"Anniversary" || propName == L"Birthday") && propValue != L"") {
       propertyValue = formatDateWithMinus(propValue);    
    } 
    else if (propName == L"Photo" && propValue != L"") {
        // the picture is right for vcard: for windows we have to format better the sif
        // even if it should work anyway.
        //<Photo> /9j/4AAQSkZJRgABAQEAcwBzAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRof
        //        Hh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwh   
        //        MDyN8gIU9aKyV+o9D//Z
        //</Photo>
        propertyValue = trim(propValue);
    } 

    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;

}

int WinContactSIF::parse(const wstring data) {

    if (!sifFields) {
        LOG.error(ERR_SIFFIELDS_NULL);
        return 1;
    }
    propertyMap.clear();

    // Check if <contact> tag is present...
    wstring::size_type pos = 0;
    wstring itemTypeTag = L"<contact>";    
    pos = data.find(itemTypeTag, 0);
    if (pos == wstring::npos) {
        LOG.error("Tag '%ls' not found", itemTypeTag.c_str());
        return 1;
    }
    wstring propertyValue;
       
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

