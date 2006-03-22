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
 
#include "base/util/utils.h"
#include "syncml/core/Source.h"
 

Source::Source() {
    locName = NULL;
    locURI  = NULL;
}
Source::~Source() {
    if (locURI) {
        delete [] locURI; locURI = NULL;
    }

    if (locName) {
        delete [] locName; locName = NULL;
    }
}

/**
 * Creates a new Source object given its URI and display name.
 *
 * @param locURI the source URI - NOT NULL
 * @param locName the source display name - NULL ALLOWED
 *
 */
Source::Source(const BCHAR* locURI, const BCHAR* locName) {
    this->locName  = NULL;
    this->locURI   = NULL;
    set(locURI, locName);
}

/**
 * Creates a new Source object given its URI
 *
 * @param locURI the source URI - NOT NULL
 *
 */
Source::Source(const BCHAR* locURI) {
    this->locName  = NULL;
    this->locURI   = NULL;
    set(locURI, NULL);
}


void Source::set(const BCHAR* locURI, const BCHAR* locName) {
    setLocURI(locURI);
    if (this->locName) {
        delete [] this->locName; this->locName = NULL;
        
    }
    this->locName = stringdup(locName);

}


/**
 * Returns the source URI value
 *
 * @return the source URI value
 */
const BCHAR* Source::getLocURI() {
        return locURI;
    }

/**
 * Sets the source URI
 *
 * @param locURI the source URI - NOT NULL
 *
 */
void Source::setLocURI(const BCHAR* locURI) {
    if (locURI == NULL) {
        // TBD
    }
    if (this->locURI) {
        delete [] this->locURI; this->locURI = NULL;
    }
    this->locURI = stringdup(locURI);
}

/**
 * Returns the source display name
 *
 * @return the source display name
 *
 */
const BCHAR* Source::getLocName() {
        return locName;
    }

/**
 * Sets the local name property
 *
 * @param locName the local name property
 *
 */
void Source::setLocName(const BCHAR* locName) {
    if (this->locName ) {
        delete [] this->locName ; this->locName  = NULL;
    }
    this->locName = stringdup(locName);
}


Source* Source::clone() {
    
    Source* ret = new Source(locURI, locName);
    return ret;
}
