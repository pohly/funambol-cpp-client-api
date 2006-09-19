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
#include "base/util/ArrayList.h"
#include "base/util/KeyValuePair.h"
#include "spdm/spdmutils.h"
#include "spdm/DeviceManagementRecord.h"


DeviceManagementRecord::DeviceManagementRecord(int ndx = -1) {
    record = NULL;
    index  = ndx ;
    size   = 0   ;
}

DeviceManagementRecord::DeviceManagementRecord(int i, WCHAR* data, unsigned int size) {
    index = i;
    setData(data, size);
}

DeviceManagementRecord::~DeviceManagementRecord() {
    if (record != NULL) {
        delete [] record; record = NULL;
    }
}

void DeviceManagementRecord::setData(WCHAR* data, unsigned int s) {
    if (record) {
        delete [] record; record = NULL;
        size = 0;
    }

    if (data == NULL) {
        return;
    }

    size = s;

    record = new WCHAR[size];

    memcpy(record, data, size);
}

void DeviceManagementRecord::setData(AccessManagementNode& n) {
    //
    // First of all, calculate the record size
    //
    WCHAR v[256];
    WCHAR l[ 32];
    WCHAR b[ 32];
    WCHAR e[ 32];
    unsigned int len = 0;

    AccessConfig& ac = n.getAccessConfig();

    n.getFullName  (v, sizeof(v)); len += (wcslen(v)+1);
    ac.getUsername (v, sizeof(v)); len += (wcslen(v)+1);
    ac.getPassword (v, sizeof(v)); len += (wcslen(v)+1);
    ac.getDeviceId (v, sizeof(v)); len += (wcslen(v)+1);
    ac.getSyncURL  (v, sizeof(v)); len += (wcslen(v)+1);
    len += 2; // use proxy
    ac.getProxyHost(v, sizeof(v)); len += (wcslen(v)+1);

    wcsprintf(l, "%u", ac.getFirstTimeSyncMode()); len += (wcslen(l)+1);
    timestampToAnchor(ac.getBeginSync(), b); len += (wcslen(b)+1);
    timestampToAnchor(ac.getEndSync(), e); len += (wcslen(e)+1);

    //
    // Now we create the record data
    //

    WCHAR* data = new WCHAR[len];
    WCHAR* p = data;

    n.getFullName (p, len); p += (wcslen(p)+1);
    ac.getUsername(p,  -1); p += (wcslen(p)+1);
    ac.getPassword(p,  -1); p += (wcslen(p)+1);
    ac.getDeviceId(p,  -1); p += (wcslen(p)+1);
    ac.getSyncURL (p,  -1); p += (wcslen(p)+1);
    if (ac.getUseProxy()) {
        wcscpy(p, "T");
    } else {
        wcscpy(p, "F");
    }
    p += 2;
    ac.getProxyHost(p, -1);
    p += (wcslen(p)+1);
    wcscpy(p, l); p += (wcslen(l)+1);
    wcscpy(p, b); p += (wcslen(b)+1);
    wcscpy(p, e);

    setData(data, len*sizeof(WCHAR));

    delete [] data;
}

void DeviceManagementRecord::setData(SourceManagementNode& n) {
    //
    // First of all, calculate the record size
    //
    WCHAR v[256];
    WCHAR l[12];
    unsigned int len = 0;

    SyncSourceConfig& ssc = n.getSourceConfig();

    n.getFullName   (v, sizeof(v)); len += (wcslen(v)+1);
    ssc.getName     (v)           ; len += (wcslen(v)+1);
    ssc.getURI      (v)           ; len += (wcslen(v)+1);
    ssc.getType     (v)           ; len += (wcslen(v)+1);
    ssc.getSyncModes(v)           ; len += (wcslen(v)+1);
    ssc.getSync     (v)           ; len += (wcslen(v)+1);
    timestampToAnchor(ssc.getLast(), l); len += (wcslen(l)+1);

    //
    // Now we create the record data
    //
    WCHAR* data = new WCHAR[len];
    WCHAR* p = data;

    n.getFullName   (p, len) ; p += (wcslen(p)+1);
    ssc.getName     (p)      ; p += (wcslen(p)+1);
    ssc.getURI      (p)      ; p += (wcslen(p)+1);
    ssc.getType     (p)      ; p += (wcslen(p)+1);
    ssc.getSyncModes(p)      ; p += (wcslen(p)+1);
    ssc.getSync     (p)      ; p += (wcslen(p)+1);
    wcscpy          (p, l)   ;

    setData(data, len*sizeof(WCHAR));

    delete [] data;
}

void DeviceManagementRecord::setData(ManagementObject& o) {
    //
    // First of all, calculate the record size
    //
    unsigned int len = 0;
    ArrayList& properties = o.getProperties();
    KeyValuePair* property = NULL;

    WCHAR v[256];
    o.getFullName(v, sizeof(v)); len = (wcslen(v)+1);

    int l = properties.size();
    for (int i=0; i<l; ++i) {
        property = (KeyValuePair*)properties[i];
        len += (wcslen(property->getKey())+1);
        len += (wcslen(property->getValue())+1);
    }


    //
    // Now we create the record data
    //
    WCHAR* data = new WCHAR[len];
    WCHAR* p = data;

    wcscpy(p, v); p += (wcslen(p)+1);
    for (int i=0; i<l; ++i) {
        property = (KeyValuePair*)properties[i];
        wcscpy(p, property->getKey()); p += (wcslen(p)+1);
        wcscpy(p, property->getValue()); p += (wcslen(p)+1);
    }

    setData(data, len*sizeof(WCHAR));

    delete [] data;
}

WCHAR* DeviceManagementRecord::getData() {
    return record;
}

unsigned int DeviceManagementRecord::getSize() {
    return size;
}

unsigned int DeviceManagementRecord::getFieldNumber() {
    if (record == NULL) {
        return 0;
    }

    WCHAR* p = record;
    unsigned int n = 0;

    while ((p-record)<size) {
        ++n;

        p += (wcslen(p) + 1);
    }

    return n;
}

WCHAR* DeviceManagementRecord::getField(unsigned int n) {
    if ((record == NULL) || (n == 0) || (n > getFieldNumber())) {
        //
        // field not found!
        //
        return NULL;
    }

    WCHAR* p = record;

    for (unsigned int i = 0; i<(n-1); ++i) {
        p += (wcslen(p) + 1);
    }

    return p;
}

BOOL DeviceManagementRecord::getAccessManagementNode(AccessManagementNode& n) {
    if ((record == NULL) || (getFieldNumber() != 10)) {
        return FALSE;
    }

    WCHAR* v = NULL;
    AccessConfig& ac = n.getAccessConfig();

    ac.setUsername(getField(2));
    ac.setPassword(getField(3));
    ac.setDeviceId(getField(4));
    ac.setSyncURL(getField(5));
    v = getField(6);
    ac.setUseProxy((*v == 'T') ? TRUE : FALSE);
    ac.setProxyHost(getField(7));
    v = getField(8);
    ac.setFirstTimeSyncMode((SyncMode)((*v) ? wcstol(v) : 0));
    v = getField(9);
    ac.setBeginSync(wcstol(v));
    v = getField(10);
    ac.setEndSync(wcstol(v));

    return TRUE;
}

BOOL DeviceManagementRecord::getSourceManagementNode(SourceManagementNode& n) {
    if ((record == NULL) || (getFieldNumber() != 7)) {
        return FALSE;
    }

    WCHAR* v = NULL;
    SyncSourceConfig& c = n.getSourceConfig();

    c.setName     (getField(2))           ;
    c.setURI      (getField(3))           ;
    c.setType     (getField(4))           ;
    c.setSyncModes(getField(5))           ;
    c.setSync     (getField(6))           ;
    v = getField(7);
    c.setLast     (((*v) ? wcstol(v) : 0));

    return TRUE;
}

void DeviceManagementRecord::setIndex(unsigned int ndx) {
    index = ndx;
}

unsigned int DeviceManagementRecord::getIndex() {
    return index;
}

BOOL DeviceManagementRecord::getManagementObject(ManagementObject& o) {
    if (record == NULL) {
        return FALSE;
    }

    int l = getFieldNumber();

    if ((l & 0x0001) == FALSE) {
        return FALSE;
    }

    for (int i=2; i<=l; i+=2) {
        o.setPropertyValue(getField(i), getField(i+1));
    }

    return TRUE;
}