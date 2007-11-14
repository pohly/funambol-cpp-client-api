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

#ifndef INCL_WINNOTE
#define INCL_WINNOTE

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */

#include "vocl/WinItem.h"
#include "vocl/constants.h"
#include "vocl/VObject.h"


/**
 * Rapresents a note object for Windows Clients.
 * The object can be filled passing a vNote, or filling
 * directly the map. Calling 'toString()' a vNote is formatted and returned.
 */
class WinNote : public WinItem {

private:

    /// Internal string formatted (VNOTE).
    wstring vNote;

    /**
     * Checks the productID and version of VObject passed for vNote.
     * - 'productID' MUST be "VNOTE"
     * - 'version' is only checked to be the one supported (log info if wrong)
     *
     * @param  vo         the VObject to check
     * @return            true if productID is correct
     */
    bool checkVNoteTypeAndVersion(VObject* vo);


public:

    /// Default Constructor
    WinNote();

    /// Constructor: fills propertyMap parsing the passed vNote string
    WinNote(const wstring dataString);

    /// Destructor
    ~WinNote();


    /**
     * Parse a vNote string and fills the propertyMap.
     * The map is cleared and will contain only found properties
     * at the end of the parsing.
     * @param dataString  input vNote string to be parsed
     * @return            0 if no errors
     */
    virtual int parse(const wstring dataString);

    /**
     * Format and return a vNote string from the propertyMap.
     * Not supported properties are ignored and so not formatted 
     * as they don't have a correspondence in propertyMap.
     * @return  the vNote string formatted, reference to internal wstring
     */
    virtual wstring& toString();
};


/** @} */
/** @endcond */
#endif
