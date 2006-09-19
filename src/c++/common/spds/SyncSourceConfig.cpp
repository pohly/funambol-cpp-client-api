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
    name            = NULL;
    uri             = NULL;
    syncModes       = NULL;
    type            = NULL;
    sync            = NULL;
    encodings       = NULL;
    version         = NULL;
    supportedTypes  = NULL;
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
    if (version) {
        delete [] version;
    }
    if (supportedTypes) {
        delete [] supportedTypes;
    }
}


const char* SyncSourceConfig::getName() const {
    return name;
}

void SyncSourceConfig::setName(const char* n) {
    safeDelete(&name);
    name = stringdup(n);
}

const char* SyncSourceConfig::getURI() const {
    return uri ? uri :
        name ? name :
        "";
}

void SyncSourceConfig::setURI(const char* u) {
    safeDelete(&uri);
    uri = stringdup(u);
}


const char* SyncSourceConfig::getSyncModes() const {
    return syncModes ? syncModes :
        "";
        
}

void SyncSourceConfig::setSyncModes(const char* s) {
    safeDelete(&syncModes);

    if (s) {
        syncModes = new char[strlen(s)+1];
        strcpy(syncModes, s);
    }
}

const char* SyncSourceConfig::getType() const {
    return type ? type :
        "";
}

void SyncSourceConfig::setType(const char* t) {
    safeDelete(&type);

    if (t) {
        type = new char[strlen(t)+1];
        strcpy(type, t);
    }
}

const char* SyncSourceConfig::getSync() const {
    return sync ? sync :
        "";
}

void SyncSourceConfig::setSync(const char *s) {
    safeDelete(&sync);

    if (s) {
        sync = new char[strlen(s)+1];
        strcpy(sync, s);
    }
}

void SyncSourceConfig::setLast(unsigned long t) {
    last = t;
}

unsigned long SyncSourceConfig::getLast() const {
    return (unsigned long)last;
}

const char* SyncSourceConfig::getEncoding() const {
    return encodings ? encodings :
        "";
}
        
void SyncSourceConfig::setEncoding(const char* s) {
    safeDelete(&encodings);

    if (s) {
        encodings = stringdup(s);
    }
}

const char* SyncSourceConfig::getVersion() const {
    return version;
}
void SyncSourceConfig::setVersion(const char* v) {
    safeDelete(&version);
    version = stringdup(v);
}

const char* SyncSourceConfig::getSupportedTypes() const {
    return supportedTypes;
}
void SyncSourceConfig::setSupportedTypes(const char* s) {
    safeDelete(&supportedTypes);
    supportedTypes = stringdup(s);
}

CTCap SyncSourceConfig::getCtCap() const {
    return ctCap;
}
void SyncSourceConfig::setCtCap(CTCap v){
    ctCap.setCTTypeSupported(v.getCTTypeSupported());
}

// ------------------------------------------------------------- Private methods

void SyncSourceConfig::assign(const SyncSourceConfig& sc) {
    setName          (sc.getName          ());
    setURI           (sc.getURI           ());
    setSyncModes     (sc.getSyncModes     ());
    setType          (sc.getType          ());
    setSync          (sc.getSync          ());
    setLast          (sc.getLast          ());
    setEncoding      (sc.getEncoding      ());
    setVersion       (sc.getVersion       ());
    setSupportedTypes(sc.getSupportedTypes());
    setCtCap         (sc.getCtCap         ());
}
