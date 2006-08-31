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
#include "base/debug.h"
#include "base/errors.h"
#include "base/Log.h"
#include "spds/DefaultConfigFactory.h"


DefaultConfigFactory::DefaultConfigFactory() {
}

DefaultConfigFactory::~DefaultConfigFactory() {
}


AccessConfig* DefaultConfigFactory::getAccessConfig() {

    AccessConfig* ac = new AccessConfig();

    ac->setUsername             (T("guest"));
    ac->setPassword             (T("guest"));
    ac->setFirstTimeSyncMode    (SYNC_NONE);
    ac->setUseProxy             (FALSE);
    ac->setProxyHost            (T(""));
    ac->setProxyPort            (8080);
    ac->setProxyUsername        (T(""));
    ac->setProxyPassword        (T(""));
    ac->setSyncURL              (T("http://localhost:8080/funambol/ds"));
    ac->setBeginSync            (0);
    ac->setEndSync              (0);
    ac->setServerAuthRequired   (FALSE);
    ac->setClientAuthType       (T("syncml:auth-basic"));
    ac->setServerAuthType       (T("syncml:auth-basic"));
    ac->setServerPWD            (T("funambol"));
    ac->setServerID             (T("funambol"));
    ac->setServerNonce          (T(""));
    ac->setClientNonce          (T(""));
    ac->setMaxMsgSize           (10000);
    ac->setMaxModPerMsg         (100);
    ac->setReadBufferSize       (0);
    ac->setUserAgent            (T(""));
    ac->setCheckConn            (FALSE);
    ac->setResponseTimeout      (0);
    ac->setEncryption           (FALSE);

    return ac;
}



DeviceConfig* DefaultConfigFactory::getDeviceConfig() {

    DeviceConfig* dc = new DeviceConfig();

    dc->setVerDTD               (T("1.1"));
    dc->setMan                  (T(""));
    dc->setMod                  (T(""));
    dc->setOem                  (T(""));
    dc->setFwv                  (T(""));
    dc->setSwv                  (T(""));
    dc->setHwv                  (T(""));
    dc->setDevID                (T("funambol-client"));
    dc->setDevType              (T("workstation"));
    dc->setDsV                  (T(""));
    dc->setUtc                  (FALSE);
    dc->setLoSupport            (FALSE);
    dc->setNocSupport           (FALSE);
    dc->setLogLevel             (LOG_LEVEL_INFO);
    dc->setMaxObjSize           (0);
    dc->setDevInfHash           (T(""));

    return dc;
}



SyncSourceConfig* DefaultConfigFactory::getSyncSourceConfig(const BCHAR* name) {

    SyncSourceConfig* sc = new SyncSourceConfig();

    sc->setName                 (name);
    sc->setSyncModes            (T("slow,two-way"));
    sc->setSync                 (T("two-way"));
    sc->setEncoding             (T("b64"));
    sc->setLast                 (0);

    if (!bstrcmp(name, T("contact"))){
        sc->setURI              (T("scard"));
        sc->setType             (T("text/x-s4j-sifc"));
    }
    else if (!bstrcmp(name, T("calendar"))){
        sc->setURI              (T("scal"));
        sc->setType             (T("text/x-s4j-sife"));
    }
    else if (!bstrcmp(name, T("task"))){
        sc->setURI              (T("stask"));
        sc->setType             (T("text/x-s4j-sift"));
    }
    else if (!bstrcmp(name, T("note"))){
        sc->setURI              (T("snote"));
        sc->setType             (T("text/x-s4j-sifn"));
    }

    // *** TBD ***
    //sc->setCtCap

    return sc;
}

