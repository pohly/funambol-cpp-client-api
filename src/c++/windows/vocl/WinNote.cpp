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

#include "base/util/utils.h"
#include "vocl/WinNote.h"
#include "vocl/VConverter.h"
#include "base/stringUtils.h"

using namespace std;


/**
 * Following are the possible values for WinNote property "X-FUNAMBOL-COLOR".
 * @see enum WinNoteColor
 */
static WCHAR* colorName[] = 
{
	{L"BLUE"   },
	{L"GREEN"  },
	{L"PINK"   },
	{L"YELLOW" },
	{L"WHITE"  },
    {NULL}
};


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

    //
    // Conversion: WinNote -> vObject.
    // -------------------------------
    //
    VObject* vo = new VObject();
    VProperty* vp  = NULL;
    wstring element;

    vp = new VProperty(TEXT("BEGIN"), TEXT("VNOTE"));
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("VERSION"), VNOTE_VERSION);
    vo->addProperty(vp);
    delete vp; vp = NULL;


    // Folder path.
    if (getProperty(L"Folder", element)) {
        vp = new VProperty(L"X-FUNAMBOL-FOLDER");
        vp->addValue(element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    if (getProperty(L"Subject", element)) {
        vp = new VProperty(TEXT("SUMMARY"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Body", element)) {
        vp = new VProperty(TEXT("BODY"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Categories", element)) {
        vp = new VProperty(TEXT("CATEGORIES"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (getProperty(L"Color", element)) {
        long color = _wtoi(element.c_str());
        vp = new VProperty(TEXT("X-FUNAMBOL-COLOR"));

        if (color < 0 || color >= NUM_NOTE_COLOR) {
            color = 1;  // Default = yellow.
        }
        vp->addValue(colorName[color]);

        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Height", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-HEIGHT"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Width", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-WIDTH"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Left", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-LEFT"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Top", element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-TOP"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    vp = new VProperty(TEXT("END"), TEXT("VNOTE"));
    vo->addProperty(vp);
    delete vp; vp = NULL;


    //
    // Format the vNote.
    // -----------------
    //
    WCHAR* tmp = vo->toString();
    if (tmp) {
        vNote = tmp;
        delete [] tmp;
    }
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
        //sprintf(lastErrorMsg, ERR_ITEM_VOBJ_PARSE);
        setError(1, ERR_ITEM_VOBJ_PARSE);
        LOG.error(getLastErrorMsg());
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
    if (element = getVObjectPropertyValue(vo, L"SUMMARY")) {
        setProperty(L"Subject", element);
    }
    if (element = getVObjectPropertyValue(vo, L"BODY")) {
        setProperty(L"Body", element);
    }
    if (element = getVObjectPropertyValue(vo, L"CATEGORIES")) {
        setProperty(L"Categories", element);
    }
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-FOLDER")) {
        setProperty(L"Folder", element);
    }


    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vNote.
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-COLOR")) {
        WCHAR tmp[10];
        int i=0;
        for (i=0; i<NUM_NOTE_COLOR; i++) {
            if (!wcscmp(colorName[i], element)) { break; }
        }
        if (i == NUM_NOTE_COLOR) { 
            i = 1;      // Default = yellow
        }
        wsprintf(tmp, TEXT("%i"), i);
        setProperty(L"Color", tmp);
    }
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-HEIGHT")) {
        setProperty(L"Height", element);
    }
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-WIDTH")) {
        setProperty(L"Width", element);
    }
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-LEFT")) {
        setProperty(L"Left", element);
    }
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-TOP")) {
        setProperty(L"Top", element);
    }

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
