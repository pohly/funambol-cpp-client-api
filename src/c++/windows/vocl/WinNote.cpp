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
#include "vocl/WinNote.h"
#include "vocl/VConverter.h"
#include "base/stringUtils.h"

using namespace std;



// Constructor
WinNote::WinNote() {
    vNote = L"";
}

// Constructor: fills propertyMap parsing the passed data
WinNote::WinNote(const wstring dataString) {
    vNote = L"";
    parse(dataString);
}

// Destructor
WinNote::~WinNote() {
}




// Format and return a vNote string from the propertyMap.
wstring& WinNote::toString() {

    vNote = L"";

    //..... todo


    //
    // Format the vNote.
    // -----------------
    //
    //WCHAR* tmp = vo->toString();
    //if (tmp) {
    //    vNote = tmp;
    //    delete [] tmp;
    //}
    return vNote;
}




//
// Parse a vNote string and fills the propertyMap.
//
int WinNote::parse(const wstring dataString) {

    WCHAR* element = NULL;

    //
    // Parse the vNote and fill the VObject.
    // -----------------------------------------
    //
    VObject* vo = VConverter::parse(dataString.c_str());
    if (!vo) {
        sprintf(lastErrorMsg, ERR_ITEM_VOBJ_PARSE);
        LOG.error(lastErrorMsg);
        return 1;
    }
    // Check if VObject type and version are the correct ones.
    if (!checkVNoteTypeAndVersion(vo)) {
        if (vo) delete vo;
        return 1;
    }


    //
    // Conversion: vObject -> WinNote.
    // -------------------------------
    // Note: properties found are added to the propertyMap, so that the 
    //       map will contain only parsed properties after this process.
    //

    //.... todo


    return 0;
}


// Utility to check the productID and version of VObject passed.
bool WinNote::checkVNoteTypeAndVersion(VObject* vo) {

    WCHAR* prodID  = vo->getProdID();
    WCHAR* version = vo->getVersion();
    
    if (!prodID) {
        LOG.error(ERR_ITEM_VOBJ_TYPE_NOTFOUND, L"VNOTE");
        return false;
    }
    if (wcscmp(prodID, L"VNOTE")) {
        LOG.error(ERR_ITEM_VOBJ_WRONG_TYPE, prodID, L"VNOTE");
        return false;
    }

    if (!version) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_VERSION_NOTFOUND, VNOTE_VERSION);
    }
    else if (wcscmp(version, VNOTE_VERSION)) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_WRONG_VERSION, version, VNOTE_VERSION);
    }
    return true;
}

