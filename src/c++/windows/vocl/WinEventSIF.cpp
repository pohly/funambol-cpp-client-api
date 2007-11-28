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

#include "vocl/WinEventSIF.h"
#include "vocl/VConverter.h"
#include "vocl/constants.h"
#include "base/stringUtils.h"
#include "vocl/sifUtils.h"

using namespace std;


// Constructor
WinEventSIF::WinEventSIF() {
    sifFields = NULL;
    sif = L"";
}

// Constructor: fills propertyMap parsing the passed SIF string
WinEventSIF::WinEventSIF(const wstring dataString, const wchar_t** fields, const wchar_t** recFields) {
    
    sif = L"";
    sifFields = fields;
    recPatternSIF.setSifFields(recFields);

    parse(dataString);
}

// Destructor
WinEventSIF::~WinEventSIF() {
}

wstring& WinEventSIF::toString() {
    
    wstring propertyValue;
    sif = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";   
    sif += L"<appointment>\n";

    sif += L"<SIFVersion>";
    sif += SIF_VERSION;
    sif += L"</SIFVersion>\n";

    //
    // Add all appointment properties
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

        //
        // If recurring, append events exceptions
        //
        BOOL isRec = _wtoi(element.c_str());
        if(isRec) {
            list<wstring>::iterator it;

            if (!excludeDate.size() && !includeDate.size()) {
                sif += L"<Exceptions/>\n";
            }
            else {
                sif += L"<Exceptions>\n";

                // Exceptions: ExcludeDate
                if (excludeDate.size() > 0) {
                    for (it  = excludeDate.begin(); it != excludeDate.end(); it++) {
                        wstring exDate = adaptToSIFSpecs(L"ExcludeDate", *it);
                        addPropertyToSIF(sif, L"ExcludeDate", exDate);
                    }
                }
                // Exceptions: IncludeDate (should be empty for Outlook and WM)
                if (includeDate.size() > 0) {
                    for (it  = includeDate.begin(); it != includeDate.end(); it++) {
                        wstring rDate = adaptToSIFSpecs(L"IncludeDate", *it);
                        addPropertyToSIF(sif, L"IncludeDate", rDate);
                    }
                }

                sif += L"</Exceptions>\n";
            }
        }
    }

    //
    // TODO: format recipients
    //

    sif += L"</appointment>";

    return sif;
}



int WinEventSIF::parse(const wstring data) {
    
    if (!sifFields) {
        LOG.error(ERR_SIFFIELDS_NULL);
        return 1;
    }
    propertyMap.clear();
    
    // Check if <appointment> tag is present...
    wstring::size_type pos = 0;
    wstring itemTypeTag = L"<appointment>";    
    pos = data.find(itemTypeTag, 0);
    if (pos == wstring::npos) {
        LOG.error("Tag '%ls' not found", itemTypeTag.c_str());
        return 1;
    }
    wstring propertyValue;

    //
    // Set appointment properties
    //
    for (int i=0; sifFields[i]; i++) {
        // Set only properties found!
        if (!getElementContent(data, sifFields[i], propertyValue, 0)) {

            replaceAll(L"&lt;",  L"<", propertyValue);
            replaceAll(L"&gt;",  L">", propertyValue);
            replaceAll(L"&amp;", L"&", propertyValue);
            
            propertyValue = adaptFromSIFSpecs(sifFields[i], propertyValue);
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

            // Fill exceptions lists.
            parseExceptions(data);
        }
    }

    //
    // TODO: parse recipients and fill recipients list
    //

    return 0;
}


WinRecurrenceSIF* WinEventSIF::getRecPattern() {
    return &recPatternSIF;
}



wstring WinEventSIF::adaptToSIFSpecs(const wstring& propName, const wstring& propValue) {
    
    wstring propertyValue = L"";

    if (propValue.length() == 8) {
        if ( propName == L"Start"       || 
             propName == L"ExcludeDate" ||
             propName == L"IncludeDate" ) {
            propertyValue = formatDateWithMinus(propValue);    
        }
        else if (propName == L"End") {
            // the End value must be decremented of a day in allDayEvent appointment
            DATE d;
            stringTimeToDouble(propValue, &d);
            d -= 1;
            doubleToStringTime(propertyValue, d, true);
            propertyValue = formatDateWithMinus(propertyValue);
        }
    }

    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;
}



wstring WinEventSIF::adaptFromSIFSpecs(const wstring& propName, const wstring& propValue) {

    wstring propertyValue = L"";

    // the End value must be incremented of a day in allDayEvent appointment
    if ( propName == L"End" && 
        (propValue.size() == 8 || propValue.size() == 10)) { // both format yyyyMMdd and yyyy-MM-dd
        DATE d;
        stringTimeToDouble(propValue, &d);
        d += 1;
        doubleToStringTime(propertyValue, d, true);                                
    }


    if (propertyValue != L"") {
        return propertyValue;
    } 
    return propValue;
}



void WinEventSIF::parseExceptions(const wstring& sifString) {

    wstring exString;

    if (getElementContent(sifString, L"Exceptions", exString)) {
        // Not found: nothing to do.
        return;
    }

    // <Exceptions> not empty -> parse all exceptions
    if (exString.size() > 0) {
        wstring exDate, rDate;
        wstring::size_type pos, start, end;

        // Parse all <ExcludeDate>
        pos = 0;
        while ( !getElementContent(exString, L"ExcludeDate", exDate, pos, start, end) ) {
            pos = end;
            if (exDate.size() > 0) {
                excludeDate.push_back(exDate);
            }
        }

        // Parse all <IncludeDate>
        pos = start = end = 0;
        while ( !getElementContent(exString, L"IncludeDate", rDate, pos, start, end) ) {
            pos = end;
            if (rDate.size() > 0) {
                includeDate.push_back(rDate);
            }
        }
    }
}
