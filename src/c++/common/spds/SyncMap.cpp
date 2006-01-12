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
#include "spds/SyncMap.h"

/*
SyncMap::SyncMap() {
    guid = luid = NULL;
}
*/

SyncMap::SyncMap(wchar_t *g, wchar_t* l) {
    guid = luid = NULL;

    setGUID(g);
    setLUID(l);
}

SyncMap::~SyncMap() {
    if (guid) {
        delete [] guid;
    }

    if (luid) {
        delete [] luid;
    }
}

/*
 * Returns the luid of this mapping. If luid is NULL, the internal
 * buffer address is returned, otherwise the value is copied into the
 * given buffer and its pointer is returned.
 *
 * @param luid - the buffer where the luid is copied to. It must be
 *               big enough
 */
wchar_t* SyncMap::getGUID(wchar_t* g) {
    if (g == NULL) {
        return guid;
    }

    if (guid == NULL) {
        return NULL;
    }

    return wcscpy(g, guid);
}


/*
 * Returns the luid of this mapping
 *
 * @param l - the buffer where the luid is copied to. It must be
 *               big enough
 */
wchar_t* SyncMap::getLUID(wchar_t* l) {
    if (l == NULL) {
        return luid;
    }

    if (luid == NULL) {
        return NULL;
    }

    return wcscpy(l, luid);
}

/**
 * Sets a new value for the GUID property. The value is copied in the
 * class internal memory.
 *
 * @param guid the new value
 */
void SyncMap::setGUID(wchar_t* g) {
    if (guid) {
        delete [] guid; guid = NULL;
    }

    if (g) {
        guid = stringdup(g);
    }
}

/**
 * Sets a new value for the LUID property. The value is copied in the
 * class internal memory.
 *
 * @param luid the new value
 */
void SyncMap::setLUID(wchar_t* l) {
    if (luid) {
        delete [] luid; luid = NULL;
    }

    if (l) {
        luid = stringdup(l);
    }
}

/**
 * Sets a new value for the LUID property (as unsigned int). It internally
 * calls setLUID(wchar_t*)
 *
 * @param luid the new value
 */
void SyncMap::setLUID(unsigned long l) {
    wchar_t ls[12];

    wcsprintf(ls, TEXT("%lu"), l);
    setLUID(ls);
}

ArrayElement* SyncMap::clone() {

    SyncMap* ret = new SyncMap();
    ret->setGUID(this->getGUID(NULL));
    ret->setLUID(this->getLUID(NULL));
    
    return ret;
}
