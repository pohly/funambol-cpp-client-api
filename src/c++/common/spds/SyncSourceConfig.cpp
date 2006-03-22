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



#include "spds/SyncSourceConfig.h"
#include "base/util/utils.h"


SyncSourceConfig::SyncSourceConfig() {
    name      = NULL;
    uri       = NULL;
    syncModes = NULL;
    type      = NULL;
    sync      = NULL;
    encodings = NULL;
}

SyncSourceConfig::~SyncSourceConfig() {
    if (name) {
        delete [] name;
    }
    if (uri) {
        delete [] uri;
    }
    if (syncModes) {
        delete [] syncModes;
    }
    if (type) {
        delete [] type;
    }
    if (sync) {
        delete [] sync;
    }

    if (encodings) {
        delete [] encodings;
    }
}


const BCHAR* SyncSourceConfig::getName(BCHAR* buf) const {
    if (buf == NULL) {
        return name;
    }

    return bstrcpy(buf, name);
}

void SyncSourceConfig::setName(const BCHAR* n) {
    safeDelete(&name);
    name = stringdup(n);
}

const BCHAR* SyncSourceConfig::getURI(BCHAR* buf) const {
    if (buf == NULL) {
        return uri;
    }

    return bstrcpy(buf, uri);
}

void SyncSourceConfig::setURI(const BCHAR* u) {
    safeDelete(&uri);
    uri = stringdup(u);
}


const BCHAR* SyncSourceConfig::getSyncModes(BCHAR* buf) const {
    if (buf == NULL) {
        return syncModes;
    }

    return bstrcpy(buf, syncModes);
}

void SyncSourceConfig::setSyncModes(const BCHAR* s) {
    safeDelete(&syncModes);

    if (s) {
        syncModes = new BCHAR[bstrlen(s)+1];
        bstrcpy(syncModes, s);
    }
}

const BCHAR* SyncSourceConfig::getType(BCHAR* buf) const {
    if (buf == NULL) {
        return type;
    }

    return bstrcpy(buf, type);
}

void SyncSourceConfig::setType(const BCHAR* t) {
    safeDelete(&type);

    if (t) {
        type = new BCHAR[bstrlen(t)+1];
        bstrcpy(type, t);
    }
}

const BCHAR* SyncSourceConfig::getSync(BCHAR* buf) const {
    if (buf == NULL) {
        return sync;
    }

    return bstrcpy(buf, sync);
}

void SyncSourceConfig::setSync(const BCHAR *s) {
    safeDelete(&sync);

    if (s) {
        sync = new BCHAR[bstrlen(s)+1];
        bstrcpy(sync, s);
    }
}

void SyncSourceConfig::setLast(unsigned long t) {
    last = t;
}

unsigned long SyncSourceConfig::getLast() const {
    return (unsigned long)last;
}

const BCHAR* SyncSourceConfig::getEncoding(BCHAR* buf) const {
     if (buf == NULL) {
        return encodings;
    }

    return bstrcpy(buf, encodings); 
}
        
void SyncSourceConfig::setEncoding(const BCHAR* s) {
    safeDelete(&encodings);

    if (s) {
        encodings = stringdup(s);
    }
}

// ------------------------------------------------------------- Private methods

void SyncSourceConfig::assign(const SyncSourceConfig& sc) {
    setName     (sc.getName     ());
    setURI      (sc.getURI      ());
    setSyncModes(sc.getSyncModes());
    setType     (sc.getType     ());
    setSync     (sc.getSync     ());
    setLast     (sc.getLast     ());
    setEncoding (sc.getEncoding ());
}
