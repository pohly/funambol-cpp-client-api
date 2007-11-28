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

#ifndef INCL_WINEVENT
#define INCL_WINEVENT

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/WinItem.h"
#include "vocl/WinRecurrence.h"
#include "vocl/WinRecipient.h"
#include "vocl/constants.h"
#include "vocl/VObject.h"


/**
 * Rapresents an event object for Windows Clients.
 * The object can be filled passing a vCalendar, or filling
 * directly the map. Calling 'toString()' a vCalendar is formatted and returned.
 */
class WinEvent : public WinItem {

private:

    /// Internal string formatted (VCALENDAR).
    wstring vCalendar;

    /// The recurrence pattern object, containing recurring properties.
    WinRecurrence recPattern;

    /// List of occurrences dates to exclude (recurring exceptions to delete).
    list<wstring> excludeDate;
    /// List of occurrences dates to include (recurring exceptions to add).
    list<wstring> includeDate;

    /// List of recipients (attendees) for this event.
    list<WinRecipient> recipients;

    //bool isRecurring;
    //bool isAllday;


    /**
     * Checks the productID and version of VObject passed for vCalendar.
     * - 'productID' MUST be "VCALENDAR"
     * - 'version' is only checked to be the one supported (log info if wrong)
     *
     * @param  vo         the VObject to check
     * @return            true if productID is correct
     */
    bool checkVCalendarTypeAndVersion(VObject* vo);

    /**
     * Utility to safe-retrieve the property value inside VObject 'vo'.
     * @param vo           : VObject to read from
     * @param propertyName : the property name requested
     * @return               the property value (NULL if not found)
     */
    WCHAR* getVObjectPropertyValue(VObject* vo, const WCHAR* propertyName);


public:

    /// Default Constructor
    WinEvent();

    /// Constructor: fills propertyMap parsing the passed vCalendar string
    WinEvent(const wstring dataString);

    /// Destructor
    ~WinEvent();


    /**
     * Parse a vCalendar string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vCalendar string to be parsed
     * @return            0 if no errors
     */
    int parse(const wstring dataString);

    /**
     * Format and return a vCalendar string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vCalendar string formatted, reference to internal wstring
     */
    wstring& toString();



    /// Returns a pointer to the (internally owned) WinRecurrence.
    WinRecurrence* getRecPattern();

    /// Returns a pointer to the list (internally owned) of exclude dates.
    list<wstring>* getExcludeDates();

    /// Returns a pointer to the list (internally owned) of include dates.
    list<wstring>* getIncludeDates();

    /// Returns a pointer to the list (internally owned) of recipients.
    list<WinRecipient>* getRecipients();
};

/** @} */
/** @endcond */
#endif
