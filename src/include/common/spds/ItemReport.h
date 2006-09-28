/*
 * Copyright (C) 2005-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCL_ITEMREPORT
#define INCL_ITEMREPORT

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"


/*
 *
 */
class ItemReport : public ArrayElement {

private:
    int  status;
    WCHAR* id;

    /*
     * Assign this object with the given ItemReport
     * @param ir: the ItemReport object
     */
    void assign(const ItemReport& ir);

public:
    ItemReport();
    ItemReport(ItemReport& ir);
    ItemReport(const WCHAR* luid, const int statusCode);
    virtual ~ItemReport();

    const WCHAR* getId() const;
    void setId(const WCHAR* v);

    const int getStatus() const;
    void setStatus(const int v);

    ArrayElement* clone();


    /*
     * Assign operator
     */
    ItemReport& operator = (const ItemReport& ir) {
        assign(ir);
        return *this;
    }
};

#endif

