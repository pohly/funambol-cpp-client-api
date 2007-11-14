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

#ifndef INCL_WINTASK_SIF
#define INCL_WINTASK_SIF

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/VObject.h"
#include "vocl/WinItem.h"
#include "vocl/WinTask.h"
#include "vocl/WinRecurrenceSIF.h"

using namespace std;


/**
 * Rapresents an event object for Windows Clients, for SIF format.
 * The object can be filled passing a SIF string, or filling
 * directly the map. Calling 'toString()' a SIF is formatted and returned.
 */
class WinTaskSIF : public WinTask {

private:

    /// Internal string formatted (SIF).
    wstring sif;

    /// NULL terminated array of SIF fields names.
    const wchar_t** sifFields;

    /// The recurrence pattern object for SIF data, containing recurring properties.
    WinRecurrenceSIF recPatternSIF;


public:

    /// Default Constructor
    WinTaskSIF();

    /**
     * Constructor: fills propertyMap parsing the passed SIF
     * @param dataString  the input SIF string
     * @param fields      the NULL terminated array of SIF fields
     * @param fields      the NULL terminated array of SIF fields for recurrence pattern
     */
    WinTaskSIF(const wstring dataString, const wchar_t** fields, const wchar_t** recFields);

    /// Destructor
    ~WinTaskSIF();  


    /// Returns a pointer to the (internally owned) WinRecurrenceSIF.
    /// @note: overrides the method of WinTask, to return a SIF formatted string.
    WinRecurrenceSIF* getRecPattern();

    
     /**
     * Parse a SIF string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input SIF string to be parsed
     * @return            0 if no errors
     */
    int parse(const wstring dataString);
   
    /**
     * Format and return a SIF string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the SIF string formatted, reference to internal wstring
     */
    wstring& toString();


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
