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

#ifndef INCL_WINRECURRENCE
#define INCL_WINRECURRENCE

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "base/timeUtils.h"
#include "vocl/WinItem.h"


/**
 * Rapresents a recurrence pattern object for Windows Clients.
 * The object can be filled passing a vCalendar RRULE string, or filling
 * directly the map. Calling 'toString()' a vCalendar RRULE is formatted and returned.
 */
class WinRecurrence : public WinItem {

private:

    /// Internal string formatted (RRULE).
    wstring rrule;


protected:

    /// The start date (double format) of the correspondent event/task
    /// It's necessary to parse correctly the RRULE string
    DATE startDate;


public:

    /// Default Constructor
    WinRecurrence();

    /**
     * Constructor: fills propertyMap parsing the vCalendar RRULE string
     * @param dataString  input RRULE string to parse
     * @param date        start date (double format) of the correspondent event/task
     */
    WinRecurrence(const wstring dataString, const DATE date = 0);

    /// Destructor
    virtual ~WinRecurrence();


    /**
     * Parse a vCalendar RRULE string and fills the propertyMap.
     * @param dataString   input RRULE string to parse
     * @return             0 if no errors
     */
    virtual int parse(const wstring dataString);

    /// Format and return a vCalendar RRULE string from the propertyMap.
    virtual wstring& toString();


    /// Returns the startDate (double format)
    const DATE getStartDate()          { return startDate; }

    /// Sets the startDate (double format)
    void setStartDate(const DATE date) { startDate = date; }


    /// Alternate method to get a property, returns an int value.
    const int getIntProperty(const wstring propertyName);

    /// Alternate method to set a property, passing an int value.
    void setIntProperty(const wstring propertyName, const int propertyValue);
};

/** @} */
/** @endcond */
#endif
