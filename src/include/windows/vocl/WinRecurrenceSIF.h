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

#ifndef INCL_WINRECURRENCE_SIF
#define INCL_WINRECURRENCE_SIF

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "base/timeUtils.h"
#include "vocl/WinRecurrence.h"


/**
 * Rapresents a recurrence pattern object for Windows Clients, for SIF format.
 * The object can be filled passing a SIF rec pattern string, or filling
 * directly the map. Calling 'toString()' a SIF rec pattern string is formatted and returned.
 */
class WinRecurrenceSIF : public WinRecurrence {

private:

    /// Internal string formatted (SIF rec pattern).
    wstring sif;

    /// NULL terminated array of SIF fields names.
    const wchar_t** sifFields;


public:

    /// Default Constructor
    WinRecurrenceSIF();

    /**
     * Constructor: fills propertyMap parsing the SIF rec pattern string
     * @param dataString   input SIF string to parse
     * @param fields      the NULL terminated array of SIF fields
     */
    WinRecurrenceSIF(const wstring dataString, const wchar_t **fields);

    /// Destructor
    ~WinRecurrenceSIF();


    /// Sets the internal pointer sifFields with the passed array.
    void setSifFields(const wchar_t** fields);


    /**
     * Parse a SIF string and fills the propertyMap.
     * @param dataString   input SIF string to parse
     * @return             0 if no errors
     */
    int parse(const wstring dataString);

    /// Format and return a SIF rec pattern string from the propertyMap.
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
