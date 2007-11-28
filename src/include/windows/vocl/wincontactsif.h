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

#ifndef INCL_WINCONTACT_SIF
#define INCL_WINCONTACT_SIF

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/VObject.h"
#include "vocl/WinItem.h"
#include "vocl/WinContact.h"

using namespace std;


/**
 * Rapresents a contact object for Windows Clients.
 * The object can be filled passing a vCard, or filling
 * directly the map. Calling 'toString()' a vCard is formatted and returned.
 */
class WinContactSIF : public WinContact {

private:

    /// Internal string formatted (SIF).
    wstring sif;
    const wchar_t** sifFields;
   
public:

    /// Default Constructor
    WinContactSIF();
    /// Constructor: fills propertyMap parsing the passed vCard
    WinContactSIF(const wstring dataString, const wchar_t **fields);

    /// Destructor
    ~WinContactSIF();   
    
     /**
     * Parse a vCard string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCard string to be parsed
     * @return            0 if no errors
     */
    int parse(const wstring dataString);
   
    /**
     * Format and return a vCard string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vCard string formatted, reference to internal wstring
     */
    wstring toString();     

    /**
    * Transform the value of the specified property according to the SIF specifications.
    * The values are formatted following the vcard and icalendar specs that in some cases
    * they are different from the SIF expectations. If there are no differences, propValue 
    * is returned.
    *
    * @param  propName   [IN] the property name
    * @param  propValue  [IN] the property value
    * @param  type       [IN] the type of the data (contact, calendar...)
    * @return            the converted value if necessary
    */
    wstring adaptToSpecsSIF(const wstring& propName, const wstring& propValue, const wstring& type);


    /**
    * Adds a tag <PropertyName>PropertyValue</PropertyName> into 'sif' string.
    */
    void addPropertyToSIF(const wstring propertyName, wstring propertyValue);

    /**
    * Adds a tag <Photo TYPE="JPEG">PropertyValue</Photo> into 'sif' string.
    */
    void addPhotoToSIF(wstring propertyValue);

    /*
    * Trim the string
    */
    wstring trim(const wstring& str);

    /*
    * Format a date like yyyyMMdd in yyyy-MM-dd
    */
    wstring formatDateWithMinus(const wstring& stringDate);

    /**
    * Transform the value of the specified property according to the SIF specifications.
    * The values are formatted following the vcard and icalendar specs that in some cases
    * they are different from the SIF expectations. If there are no differences, propValue 
    * is returned.
    *
    * @param  propName   [IN] the property name
    * @param  propValue  [IN] the property value    
    * @return            the converted value if necessary
    */
    wstring adaptToSIFSpecs(const wstring& propName, const wstring& propValue);
    
};
/** @} */
/** @endcond */
#endif
