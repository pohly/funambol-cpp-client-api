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
#include "syncml/core/VerProto.h"
 

VerProto::VerProto() {
    version = NULL;
}
VerProto::~VerProto() {
    if (version) {
        delete [] version; version = NULL;
    }
}

/**
 * Creates a new VerProto object from its version.
 *
 * @param version the protocol version - NOT NULL
 *
 */
VerProto::VerProto(wchar_t* version) {
    this->version = NULL;
    setVersion(version);
}

/**
 * Returns the protocol version.
 * 
 * @return the protocol version - NOT NULL
 *
 */
wchar_t* VerProto::getVersion(wchar_t* retVersion) {
    if (retVersion == NULL) {
        return version;
    }
    return wcscpy(retVersion, version);
}

/**
 * Sets the protol version.
 *
 * @param version the protocol version - NOT NULL
 *
 */
void VerProto::setVersion(wchar_t* version) {
    
    if (version == NULL) {
        // TBD
    }
    if (this->version) {
        delete [] this->version; this->version = NULL;
    }
    this->version = stringdup(version);        
}

VerProto* VerProto::clone() {
    VerProto* ret = new VerProto(version);
    return ret;
}
