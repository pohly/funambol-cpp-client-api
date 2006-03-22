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

#include "examples/config.h"

#ifdef DEBUG_SETTINGS

#include "base/fscapi.h"
#include "spds/constants.h"
#include "spdm/DMTree.h"
#include "spdm/DMTreeFactory.h"
#include "spdm/ManagementNode.h"

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

int settings(const char *rootContext)
{
    char nodeName[DIM_MANAGEMENT_PATH];
    DMTree* dmt = DMTreeFactory::getDMTree(rootContext);
    lastErrorCode = 0;

    // Access config parameters
    sprintf(nodeName, "%s%s", rootContext, CONTEXT_SPDS_SYNCML);

    ManagementNode *node = dmt->getManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue(T("deviceId"), T(""));
    node->setPropertyValue(T("username"), T("guest"));
    node->setPropertyValue(T("password"), T("guest"));
    node->setPropertyValue(T("syncUrl"), T("http://192.168.0.xx:8080/funambol/ds"));
    node->setPropertyValue(T("serverName"), T("http://192.168.0.xx:8080"));
    node->setPropertyValue(T("beginTimestamp"), T("0"));
    node->setPropertyValue(T("endTimestamp"), T("0"));
    node->setPropertyValue(T("sourceCount"), T("0"));
    node->setPropertyValue(T("responseTimeout"), T("0"));
    node->setPropertyValue(T("checkConn"), T("1"));
    node->setPropertyValue(T("firstTimeSyncMode"), T("0"));
    node->setPropertyValue(T("proxyHost"), T(""));
    node->setPropertyValue(T("proxyPort"), T(""));
    node->setPropertyValue(T("useProxy"), T(""));
    node->setPropertyValue(T("clientNonce"), T("cGlwcG8="));
    node->setPropertyValue(T("serverNonce"), T("VGB8YHQ6U25lWmAuR3Z3bw=="));
    node->setPropertyValue(T("serverID"), T("funambol"));
    node->setPropertyValue(T("serverPWD"), T("funambol"));
    node->setPropertyValue(T("isServerAuthRequired"), T("F"));
    node->setPropertyValue(T("clientAuthType"), T("syncml:auth-basic"));
    node->setPropertyValue(T("serverAuthType"), T("syncml:auth-basic"));
    node->setPropertyValue(T("maxMsgSize"), T("122880"));
    node->setPropertyValue(T("maxModPerMsg"), T("20"));
    
    delete node;

    // Contact sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/contact");

    node = dmt->getManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue(T("sync"), T("two-way"));
    node->setPropertyValue(T("last"), T("0"));
    node->setPropertyValue(T("name"), T("contact"));
    node->setPropertyValue(T("type"), T("text/x-s4j-sifc"));
    node->setPropertyValue(T("syncModes"), T("slow,two-way,refresh"));
    node->setPropertyValue(T("uri"), T("scard"));
    node->setPropertyValue(T("useSIF"), T("1"));
    node->setPropertyValue(T("encoding"), T("b64"));

    delete node;

    // Contact sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/appointments");

    node = dmt->getManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue(T("sync"), T("none"));
    node->setPropertyValue(T("last"), T("0"));
    node->setPropertyValue(T("name"), T("calendar"));
    node->setPropertyValue(T("type"), T("text/x-s4j-sife"));
    node->setPropertyValue(T("syncModes"), T("slow,two-way,refresh"));
    node->setPropertyValue(T("uri"),  T("scal"));
    node->setPropertyValue(T("useSIF"), T("1"));
    node->setPropertyValue(T("encoding"), T("b64"));

    delete node;

    // Contact sync source parameters
    sprintf(nodeName, "%s%s", rootContext, "/spds/sources/mails");

    node = dmt->getManagementNode(nodeName);
    if ( ! node ) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        goto finally;
    }

    node->setPropertyValue(T("sync"), T("none"));
    node->setPropertyValue(T("last"), T("0"));
    node->setPropertyValue(T("name"), T("mail"));
    node->setPropertyValue(T("type"), T("text/x-s4j-sife"));
    node->setPropertyValue(T("syncModes"), T("slow,two-way,refresh"));
    node->setPropertyValue(T("uri"), T("mail"));
    node->setPropertyValue(T("useSIF"), T("1"));
    node->setPropertyValue(T("encoding"), T("b64"));
    node->setPropertyValue(T("Inbox" ), T("1" ) );
    node->setPropertyValue(T("Outbox" ), T("1" ) );
    node->setPropertyValue(T("Trash" ), T("0" ) );
    node->setPropertyValue(T("Sent" ), T("1" ) );
    node->setPropertyValue(T("Draft" ), T("0" ) );
    
finally:
    if (node)
        delete node;
    if (dmt)
        delete dmt;
        
    return lastErrorCode;
}

#endif // DEBUG

