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
 
#include "base/fscapi.h" 
#include "base/util/utils.h"
#include "syncml/core/Anchor.h"

Anchor::Anchor(const BCHAR* last, const BCHAR* next) {
    this->last = stringdup(last);
    this->next = stringdup(next);
}

Anchor::~Anchor() {
    if (last) {
		delete [] last; last = NULL;
	}
    if (next) {
		delete [] next; next = NULL;
	}
}


void Anchor::setLast(const BCHAR* last) {
	if (last) {
		delete [] this->last;
	}
	this->last = stringdup(last);
}

const BCHAR* Anchor::getLast() {    
    return last;
}

void Anchor::setNext(const BCHAR* next) {
	if (next) {
		delete [] this->next;
	}
	this->last = stringdup(last);
}

const BCHAR* Anchor::getNext() {
   return next;
}

Anchor* Anchor::clone() {
    Anchor* ret = NULL;
    if (this) {
        ret = new Anchor(last, next);
    }
    return ret;
}
