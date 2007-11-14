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

#include "base/Log.h"
#include "base/stringUtils.h"
#include "vocl/sifUtils.h"

using namespace std;


void addPropertyToSIF(wstring& sif, const wstring propertyName, wstring propertyValue) {

    if (propertyValue != L"") {

        replaceAll(L"&", L"&amp;", propertyValue);
        replaceAll(L"<", L"&lt;",  propertyValue);
        replaceAll(L">", L"&gt;",  propertyValue);

        sif += L"<";
        sif += propertyName;
        sif += L">";
        sif += propertyValue;
        sif += L"</";
        sif += propertyName;
        sif += L">\n";
    }
    else {
        sif += L"<";
        sif += propertyName;
        sif += L"/>\n";
    }
}


wstring formatDateWithMinus(const wstring& stringDate) {
    
    wstring ret;
    if (stringDate.length() == 8) {
        ret = stringDate.substr(0, 4);
        ret += L"-";
        ret += stringDate.substr(4, 2);
        ret += L"-";
        ret += stringDate.substr(6, 2);
    }
    return ret;
}


wstring trim(const wstring& str) {
    wstring ret = str;
    int idx = 0;
    while((idx=ret.find_first_of(' ')) == 0 ) {
        ret.replace( idx, 1, L"" );            
    }
    while((idx=ret.find_last_of(' ')) == ret.size()-1 ) {
        ret.replace( idx, 1, L"" );            
    }
    return ret;
}

