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
}


const wchar_t* SyncSourceConfig::getName(wchar_t* buf) const {
    if (buf == NULL) {
        return name;
    }

    return wcscpy(buf, name);
}

void SyncSourceConfig::setName(const wchar_t* n) {
    safeDelete(&name);

    if (n) {
        name = new wchar_t[wcslen(n)+1];
        wcscpy(name, n);
    }
}

const wchar_t* SyncSourceConfig::getURI(wchar_t* buf) const {
    if (buf == NULL) {
        return uri;
    }

    return wcscpy(buf, uri);
}

void SyncSourceConfig::setURI(const wchar_t* u) {
    safeDelete(&uri);

    if (u) {
        uri = new wchar_t[wcslen(u)+1];
        wcscpy(uri, u);
    }
}


const wchar_t* SyncSourceConfig::getSyncModes(wchar_t* buf) const {
    if (buf == NULL) {
        return syncModes;
    }

    return wcscpy(buf, syncModes);
}

void SyncSourceConfig::setSyncModes(const wchar_t* s) {
    safeDelete(&syncModes);

    if (s) {
        syncModes = new wchar_t[wcslen(s)+1];
        wcscpy(syncModes, s);
    }
}

const wchar_t* SyncSourceConfig::getType(wchar_t* buf) const {
    if (buf == NULL) {
        return type;
    }

    return wcscpy(buf, type);
}

void SyncSourceConfig::setType(const wchar_t* t) {
    safeDelete(&type);

    if (t) {
        type = new wchar_t[wcslen(t)+1];
        wcscpy(type, t);
    }
}

const wchar_t* SyncSourceConfig::getSync(wchar_t* buf) const {
    if (buf == NULL) {
        return sync;
    }

    return wcscpy(buf, sync);
}

void SyncSourceConfig::setSync(const wchar_t *s) {
    safeDelete(&sync);

    if (s) {
        sync = new wchar_t[wcslen(s)+1];
        wcscpy(sync, s);
    }
}

void SyncSourceConfig::setLast(unsigned long t) {
    last = t;
}

unsigned long SyncSourceConfig::getLast() const {
    return (unsigned long)last;
}


// ------------------------------------------------------------- Private methods

void SyncSourceConfig::assign(const SyncSourceConfig& sc) {
    setName     (sc.getName     ());
    setURI      (sc.getURI      ());
    setSyncModes(sc.getSyncModes());
    setType     (sc.getType     ());
    setSync     (sc.getSync     ());
    setLast     (sc.getLast     ());
}
