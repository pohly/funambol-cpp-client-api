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

#include "vocl/WinContact.h"
#include "vocl/VConverter.h"

using namespace std;



// Constructor
WinContact::WinContact() {
    vCard = L"";
}

// Constructor: fills propertyMap parsing the passed vCard string
WinContact::WinContact(const wstring dataString) {
    vCard = L"";
    parse(dataString);
}

// Destructor
WinContact::~WinContact() {
}




//
// Format and return a vCard string from the propertyMap.
//
wstring& WinContact::toString() {

    vCard = L"";

    //
    // Conversion: WinContact -> vObject.
    // ----------------------------------
    //
    VObject* vo = new VObject();
    wstring element;
    VProperty* vp = NULL;
    bool found = false;


    vp = new VProperty(L"BEGIN", L"VCARD");
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(L"VERSION", VCARD_VERSION);
    vo->addProperty(vp);
    delete vp; vp = NULL;


    // Folder path.
    if (getProperty(L"Folder", element)) {
        vp = new VProperty(L"X-FUNAMBOL-FOLDER");
        vp->addValue(element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    // ------- Name -------
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    found = false;
    vp = new VProperty(L"N");
    if (getProperty(L"LastName",   element))  found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"FirstName",  element))  found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"MiddleName", element))  found = true;  
    vp->addValue(element.c_str());
    if (getProperty(L"Title",      element))  found = true;  
    vp->addValue(element.c_str());
    if (getProperty(L"Suffix",     element))  found = true;  
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    if (getProperty(L"FileAs", element)) {
        vp = new VProperty(L"FN", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Birthday", element)) {
        vp = new VProperty(L"BDAY", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"BusinessFaxNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"WORK", NULL);
        vp->addParameter(L"FAX", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"CompanyName", element)) {
        vp = new VProperty(L"ORG", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    // Mapping is:
    // Email1Address <-> EMAIL;INTERNET:
    // Email2Address <-> EMAIL;INTERNET;HOME:
    // Email3Address <-> EMAIL;INTERNET;WORK:
    if (getProperty(L"Email1Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Email2Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vp->addParameter(L"HOME",     NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Email3Address", element)) {
        vp = new VProperty(L"EMAIL", element.c_str());
        vp->addParameter(L"INTERNET", NULL);
        vp->addParameter(L"WORK",     NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    if (getProperty(L"MobileTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"CELL", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"PagerNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"PAGER", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"BusinessTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"WORK", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Business2TelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"WORK", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"JobTitle", element)) {
        vp = new VProperty(L"TITLE", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"HomeTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"HOME", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Home2TelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"HOME", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"CarTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"CAR", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"HomeFaxNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"HOME", NULL);
        vp->addParameter(L"FAX", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"OtherTelephoneNumber", element)) {
        vp = new VProperty(L"TEL", element.c_str());
        vp->addParameter(L"VOICE", NULL);
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Categories", element)) {
        vp = new VProperty(L"CATEGORIES", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"WebPage", element)) {
        vp = new VProperty(L"URL", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Body", element)) {
        vp = new VProperty(L"NOTE", element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    // ----- Address HOME -----
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    // "AddressPostOfficeBox" is not supported by WM.
    // "AddressExtended" is not supported by Outlook/WM.
    found = false;
    vp = new VProperty(L"ADR");
    vp->addParameter(L"HOME", NULL);
    if (getProperty(L"HomeAddressPostOfficeBox", element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressExtended",      element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressStreet",        element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressCity",          element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressState",         element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressPostalCode",    element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"HomeAddressCountry",       element))   found = true;
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    // ----- Address -----
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    // "AddressPostOfficeBox" is not supported by WM.
    // "AddressExtended" is not supported by Outlook/WM.
    found = false;
    vp = new VProperty(L"ADR");
    if (getProperty(L"OtherAddressPostOfficeBox", element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressExtended",      element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressStreet",        element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressCity",          element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressState",         element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressPostalCode",    element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"OtherAddressCountry",       element))   found = true;
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    // ----- Address WORK -----
    // Add only if at least 1 property is supported, but include 
    // all elements in the right order.
    // "AddressPostOfficeBox" is not supported by WM.
    // "AddressExtended" is not supported by Outlook/WM.
    found = false;
    vp = new VProperty(L"ADR");
    vp->addParameter(L"WORK", NULL);
    if (getProperty(L"BusinessAddressPostOfficeBox", element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressExtended",      element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressStreet",        element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressCity",          element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressState",         element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressPostalCode",    element))   found = true;
    vp->addValue(element.c_str());
    if (getProperty(L"BusinessAddressCountry",       element))   found = true;
    vp->addValue(element.c_str());
    if (found) {
        vo->addProperty(vp);
    }
    delete vp; vp = NULL;


    vp = new VProperty(L"END", L"VCARD");
    vo->addProperty(vp);
    delete vp; vp = NULL;


    //
    // Format the vCard.
    // -----------------
    //
    WCHAR* tmp = vo->toString();
    if (tmp) {
        vCard = tmp;
        delete [] tmp;
    }
    return vCard;
}



//
// Parse a vCard string and fills the propertyMap.
//
int WinContact::parse(const wstring dataString) {

	int businessTel  = 0;
	int homeTel		 = 0;
	WCHAR* name      = NULL;
    WCHAR* element   = NULL;


    //
    // Parse the vCard and fill the VObject.
    // -------------------------------------
    //
    VObject* vo = VConverter::parse(dataString.c_str());
    if (!vo) {
        sprintf(lastErrorMsg, ERR_ITEM_VOBJ_PARSE);
        LOG.error(lastErrorMsg);
        return 1;
    }
    // Check if VObject type and version are the correct ones.
    if (!checkVCardTypeAndVersion(vo)) {
        if (vo) delete vo;
        return 1;
    }


    //
    // Conversion: vObject -> WinContact.
    // ----------------------------------
    // Note: properties found are added to the propertyMap, so that the 
    //       map will contain only parsed properties after this process.
    //
    propertyMap.clear();
    for(int i=0; i < vo->propertiesCount(); i++) {

        VProperty* vp = vo->getProperty(i);
		name    = vp->getName();
        element = vp->getValue(0);      // the first value of the property.


        if(!wcscmp(name, L"X-FUNAMBOL-FOLDER")) {
            setProperty(L"Folder", element);
        }

		else if(!wcscmp(name, L"N")) {
			if(element = vp->getPropComponent(1))  setProperty(L"LastName",   element);
			if(element = vp->getPropComponent(2))  setProperty(L"FirstName",  element);
			if(element = vp->getPropComponent(3))  setProperty(L"MiddleName", element);
			if(element = vp->getPropComponent(4))  setProperty(L"Title",      element);
			if(element = vp->getPropComponent(5))  setProperty(L"Suffix",     element);
		}
        else if(!wcscmp(name, L"FN")) {
			setProperty(L"FileAs", element);
        }
        else if(!wcscmp(name, L"BDAY")) {
			setProperty(L"Birthday", element);
        }
		else if(!wcscmp(name, L"TEL") && vp->containsParameter(L"WORK")) {
            if (vp->containsParameter(L"FAX")) {
				setProperty(L"BusinessFaxNumber", element);
            }
            else if (vp->containsParameter(L"CELL")) {
                // ****???***
            }
            else {
				if(businessTel == 0) {
					setProperty(L"BusinessTelephoneNumber", element);
					businessTel++;
				}
				else{
					setProperty(L"Business2TelephoneNumber", element);
				}
            }
		}
		else if(!wcscmp(name, L"ORG")) {
			setProperty(L"CompanyName", element);
		}

		else if(!wcscmp(name, L"EMAIL")) {
            // Mapping is:
            // Email1Address <-> EMAIL;INTERNET:
            // Email2Address <-> EMAIL;INTERNET;HOME:
            // Email3Address <-> EMAIL;INTERNET;WORK:
			if (vp->containsParameter(L"HOME")) {
				setProperty(L"Email2Address", element);
			}
			else if (vp->containsParameter(L"WORK")) {
				setProperty(L"Email3Address", element);
			}
            else {
				setProperty(L"Email1Address", element);
            }
		}

        else if(!wcscmp(name, L"TEL") && vp->containsParameter(L"CELL")) {
			setProperty(L"MobileTelephoneNumber", element);
        }
        else if(!wcscmp(name, L"TEL") && vp->containsParameter(L"PAGER")) {
			setProperty(L"PagerNumber", element);
        }
        else if(!wcscmp(name, L"TITLE")) {
			setProperty(L"JobTitle", element);
        }
		else if(!wcscmp(name, L"TEL") && vp->containsParameter(L"HOME")) {
            if(vp->containsParameter(L"FAX")) {
				setProperty(L"HomeFaxNumber", element);
            }
            else if(vp->containsParameter(L"CELL")) {
            }
			else if(homeTel == 0) {
				setProperty(L"HomeTelephoneNumber", element);
				homeTel++;
			}
            else {
				setProperty(L"Home2TelephoneNumber", element);
            }
		}

        else if(!wcscmp(name, L"TEL") && vp->containsParameter(L"CAR")) {
			setProperty(L"CarTelephoneNumber", element);
        }
        else if(!wcscmp(name, L"TEL") && vp->containsParameter(L"VOICE")) {
			setProperty(L"OtherTelephoneNumber", element);
        }
        else if(!wcscmp(name, L"CATEGORIES")) {
			setProperty(L"Categories", element);
        }
        else if(!wcscmp(name, L"URL")) {
			setProperty(L"WebPage", element);
        }
        else if(!wcscmp(name, L"NOTE")) {
			setProperty(L"Body", element);
        }

        else if(!wcscmp(name, L"ADR")) {
			if(vp->containsParameter(L"HOME")) {
				if(element = vp->getPropComponent(1))  setProperty(L"HomeAddressPostOfficeBox", element);
                if(element = vp->getPropComponent(2))  setProperty(L"HomeAddressExtended",      element);
				if(element = vp->getPropComponent(3))  setProperty(L"HomeAddressStreet",        element);
				if(element = vp->getPropComponent(4))  setProperty(L"HomeAddressCity",          element);
				if(element = vp->getPropComponent(5))  setProperty(L"HomeAddressState",         element);
				if(element = vp->getPropComponent(6))  setProperty(L"HomeAddressPostalCode",    element);
				if(element = vp->getPropComponent(7))  setProperty(L"HomeAddressCountry",       element);
			}
			else if(vp->containsParameter(L"WORK")) {
				if(element = vp->getPropComponent(1))  setProperty(L"BusinessAddressPostOfficeBox", element);
                if(element = vp->getPropComponent(2))  setProperty(L"BusinessAddressExtended",      element);
				if(element = vp->getPropComponent(3))  setProperty(L"BusinessAddressStreet",        element);
				if(element = vp->getPropComponent(4))  setProperty(L"BusinessAddressCity",          element);
				if(element = vp->getPropComponent(5))  setProperty(L"BusinessAddressState",         element);
				if(element = vp->getPropComponent(6))  setProperty(L"BusinessAddressPostalCode",    element);
				if(element = vp->getPropComponent(7))  setProperty(L"BusinessAddressCountry",       element);
			}
			else {
                if(element = vp->getPropComponent(1))  setProperty(L"OtherAddressPostOfficeBox", element);
                if(element = vp->getPropComponent(2))  setProperty(L"OtherAddressExtended",      element);
				if(element = vp->getPropComponent(3))  setProperty(L"OtherAddressStreet",        element);
				if(element = vp->getPropComponent(4))  setProperty(L"OtherAddressCity",          element);
				if(element = vp->getPropComponent(5))  setProperty(L"OtherAddressState",         element);
				if(element = vp->getPropComponent(6))  setProperty(L"OtherAddressPostalCode",    element);
				if(element = vp->getPropComponent(7))  setProperty(L"OtherAddressCountry",       element);
			}
        }
    }

    return 0;
}



// Utility to check the productID and version of VObject passed.
bool WinContact::checkVCardTypeAndVersion(VObject* vo) {

    WCHAR* prodID  = vo->getProdID();
    WCHAR* version = vo->getVersion();
    if (!prodID || !version) {
        return false;
    }

    if (wcscmp(prodID, L"VCARD")) {
        LOG.error(ERR_ITEM_VOBJ_WRONG_TYPE, prodID, L"VCARD");
        return false;
    }
    if (wcscmp(version, VCARD_VERSION)) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_WRONG_VERSION, version, VCARD_VERSION);
    }

    return true;
}