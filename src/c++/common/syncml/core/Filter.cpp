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

#include "syncml/core/Filter.h"

Filter::Filter() : meta(NULL), field(NULL), record(NULL), filterType(NULL) {
}

Filter::Filter(Meta*    m,
               Item*    f,
               Item*    r,
               char* t): meta(NULL), field(NULL), record(NULL), filterType(NULL) {
    setMeta(m);
    setField(f);
    setRecord(r);
    setFilterType(t);
}

Filter::~Filter() {
    if (meta)       { delete meta      ; meta = NULL;         }
    if (field)      { delete field     ; field = NULL;        }
    if (record  )   { delete record    ; record   = NULL;     }
    if (filterType) { delete filterType; filterType   = NULL; }
}

Meta* Filter::getMeta() {
    return meta;
}

void Filter::setMeta(Meta* m) {
    if (this->meta) {
		delete this->meta; this->meta = NULL;
    } 

    this->meta = m->clone();   
}

Item* Filter::getField() {
    return field;
}

void Filter::setField(Item* f) {
    if (field) {
		delete field; field = NULL;
    } 
    if (f) {
	    field = (Item*)f->clone();
    }
}

Item* Filter::getRecord() {
    return record;
}

void Filter::setRecord(Item* r) {
    if (record) {
		delete record; record = NULL;
    } 
    if (r) {
	    record = (Item*)r->clone();   
    }
}

char* Filter::getFilterType(char* t) {
    if (t == NULL) {
        return filterType;
    }
    return strcpy(t, filterType);
}

void Filter::setFilterType(char* t) {
    if (filterType) {
        delete [] filterType; filterType = NULL;
    }

    if (t) {
        filterType = stringdup(t);  
    }
}

Filter* Filter::clone() {
    return new Filter(meta, field, record, filterType);
}
