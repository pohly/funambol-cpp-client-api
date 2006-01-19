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

//--README--------------------------------------------------------------
// This file contains the code to set the registry entries used by the
// WindowsMobile test client. It is intended mainly for emulator use.
//
// Remember to adjust the settings to your needs (the server address,
// at least!)
//
// This code is for TEST/DEBUG only, do not use it if you dont't know
// what you are doing.
//---------------------------------------------------------WARNING------

#ifdef DEBUG_SETTINGS

#include "base/fscapi.h"

#ifdef _WIN32_WCE
static wchar_t *convertSlashes(const wchar_t* str) {
    int i, len;

    len = wcslen(str);
    wchar_t *ret = new wchar_t[len+1];

    for (i=0; i<len; ++i) {
        if (str[i] == L'/') {
            ret[i] = L'\\';
        }
        else {
            ret[i] = str[i];
        }
    }
    ret[len] = 0;
    return ret;
}

/*
 * Sets a property value.
 *
 * @param property - the property name
 * @param value - the property value (zero terminated string)
 */
static void setProperty(wchar_t* context, wchar_t* property, wchar_t* value) {

    HKEY key;
    DWORD res;

    wchar_t *fullContext = convertSlashes(context);

    RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );

    if (key == 0) {
        goto finally;
    }

    RegSetValueEx(
            key,
            property,
            NULL,
            REG_SZ,  // we currently support only strings
            (UCHAR*)value,
            (wcslen(value)+1)*sizeof(wchar_t*)
            );

finally:

    if (key != 0) {
        RegCloseKey(key);
    }
    delete [] fullContext;
}

#endif  // _WIN32_CE

void settings() {
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("deviceId"),
        TEXT("sc-pim-ppc"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("username"),
        TEXT("guest"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("password"),
        TEXT("guest"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("syncUrl"),
        TEXT("http://192.168.0.00:8080/funambol/ds"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("serverName"),
        TEXT("http://192.168.0.00:8080"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("beginTimestamp"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("endTimestamp"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("sourceCount"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("responseTimeout"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("checkConn"),
        TEXT("1"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("firstTimeSyncMode"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("proxyHost"),
        TEXT(""));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("proxyPort"),
        TEXT(""));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("useProxy"),
        TEXT(""));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("clientNonce"),
        TEXT("cGlwcG8="));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("serverNonce"),
        TEXT("VGB8YHQ6U25lWmAuR3Z3bw=="));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("serverID"),
        TEXT("sync4j"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("serverPWD"),
        TEXT("sync4j"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("isServerAuthRequired"),
        TEXT("F"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("clientAuthType"),
        TEXT("syncml:auth-basic"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("serverAuthType"),
        TEXT("syncml:auth-basic"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("maxMsgSize"),
        TEXT("12288"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/syncml"),
        TEXT("maxModPerMsg"),
        TEXT("3"));
    
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/contact"),
        TEXT("sync"),
        TEXT("two-way"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/contact"),
        TEXT("last"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/contact"),
        TEXT("name"),
        TEXT("contact"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/contact"),
        TEXT("type"),
        TEXT("text/x-s4j-sifc"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/contact"),
        TEXT("syncModes"),
        TEXT("slow,two-way,refresh"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/contact"),
        TEXT("uri"),
        TEXT("scard"));
    setProperty(    
        TEXT("Software/Funambol/examples/dummy/spds/sourcesprop/contact"),
        TEXT("name"),
        TEXT("contact"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sourcesprop/contact"),
        TEXT("show"),
        TEXT("1"));

    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/briefcase"),
        TEXT("sync"),
        TEXT("none"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/briefcase"),
        TEXT("last"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/briefcase"),
        TEXT("name"),
        TEXT("calendar"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/briefcase"),
        TEXT("type"),
        TEXT("text/x-s4j-sife"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/briefcase"),
        TEXT("syncModes"),
        TEXT("slow,two-way,refresh"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/briefcase"),
        TEXT("uri"),
        TEXT("scal"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sourcesprop/briefcase"),
        TEXT("name"),
        TEXT("calendar"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sourcesprop/briefcase"),
        TEXT("show"),
        TEXT("1"));

    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/mail"),
        TEXT("sync"),
        TEXT("none"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/mail"),
        TEXT("last"),
        TEXT("0"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/mail"),
        TEXT("name"),
        TEXT("mail"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/mail"),
        TEXT("type"),
        TEXT("text/x-s4j-sife"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/mail"),
        TEXT("syncModes"),
        TEXT("slow,two-way,refresh"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sources/mail"),
        TEXT("uri"),
        TEXT("mail"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sourcesprop/mail"),
        TEXT("name"),
        TEXT("briefcase"));
    setProperty(
        TEXT("Software/Funambol/examples/dummy/spds/sourcesprop/mail"),
        TEXT("show"),
        TEXT("1"));
    
    setProperty(
        TEXT("Software/Funambol/examples"),
        TEXT("showRemoteSettings"),
        TEXT("1"));
    setProperty(
        TEXT("Software/Funambol/examples"),
        TEXT("logLevel"),
        TEXT("1"));
}
#endif // DEBUG

