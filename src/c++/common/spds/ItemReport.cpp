/*
 * Copyright (C) 2003-2006 Funambol
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


#include "spds/ItemReport.h"



//--------------------------------------------------- Constructor & Destructor
ItemReport::ItemReport() {
    status = 0;
    id  = NULL;
}
ItemReport::ItemReport(const WCHAR* luid, const int statusCode) {
    id  = NULL;
    setStatus(statusCode);
    setId(luid);
}

ItemReport::ItemReport(ItemReport& ir) {
    status = 0;
    id  = NULL;

    assign(ir);
}

ItemReport::~ItemReport() {
    if (id) {
        delete [] id; 
        id = NULL;
    }
}

//------------------------------------------------------------- Public Methods

const WCHAR* ItemReport::getId() const {
    return id;
}
void ItemReport::setId(const WCHAR* v) {
    if (id) {
        delete [] id; 
        id = NULL;
    }

	id = wstrdup(v);
}

const int ItemReport::getStatus() const {
    return status;
}
void ItemReport::setStatus(const int v) {
    status = v;
}


ArrayElement* ItemReport::clone() {
    ItemReport* it = new ItemReport(getId(), getStatus());
    return it;
}

//------------------------------------------------------------- Private Methods
void ItemReport::assign(const ItemReport& ir) {
    setId    (ir.getId    ());
    setStatus(ir.getStatus());
}
