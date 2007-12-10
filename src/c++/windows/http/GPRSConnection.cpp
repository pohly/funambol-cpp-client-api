/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */



#include "winsock2.h"


#include <objbase.h>
#include <initguid.h>
#include <connmgr.h>
#include <wininet.h>

#include "http/GPRSConnection.h"
#include "base/Log.h"


/*
* Method to test if connection is alive. If no one is found, it tries to establish
* default connection.
*/

BOOL EstablishConnection() {

    HANDLE  phWebConnection = NULL;

    // First we check if we might have a connection
    DWORD  pdwStatus = 0;
    LOG.debug("Establish connection: test internet connection status...");
    ConnMgrConnectionStatus(phWebConnection, &pdwStatus);

    if (pdwStatus == CONNMGR_STATUS_CONNECTED) {
        LOG.info("Already connected");
        //We are already connected!
        return TRUE;
    } 
#if _WIN32_WCE > 0x500     
    else if (pdwStatus == CONNMGR_STATUS_PHONEOFF) {
        LOG.info("phone off");
        //We are already connected!
        return FALSE;
    }
#endif
    else {
        LOG.debug("Not connected (status = 0x%02x): try to connect...", pdwStatus);
        //We are not connected, so lets try:
        //The CONNECTIONINFO is the structure that
        //tells Connection Manager how we want
        //to connect

        CONNMGR_CONNECTIONINFO sConInfo;
        memset(&sConInfo,0, sizeof(CONNMGR_CONNECTIONINFO));
        sConInfo.cbSize = sizeof(CONNMGR_CONNECTIONINFO);
        // We want to use the "guidDestNet" parameter
        sConInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
        // This is the highest data priority.
        sConInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
        // sConInfo.dwFlags = 0;
        sConInfo.dwFlags=CONNMGR_FLAG_PROXY_HTTP;
        // Lets be nice and share the connection with
        // other applications
        sConInfo.bExclusive = FALSE;
        sConInfo.bDisabled = FALSE;
        sConInfo.guidDestNet = IID_DestNetInternet;
        LOG.debug("Try to establish connection...");
        if (ConnMgrEstablishConnection(&sConInfo,&phWebConnection) == S_OK) {
            LOG.debug("Start internet connection process...");
            //We start successfully process!
            for (unsigned int k = 0; k < 6; k++) {
                ConnMgrConnectionStatus(phWebConnection,&pdwStatus);

                if (pdwStatus == CONNMGR_STATUS_CONNECTED) {
                    LOG.info("Internet connection succesfully completed.");
                    return TRUE;

                }
                else {
                    if (pdwStatus == CONNMGR_STATUS_CONNECTIONCANCELED || pdwStatus == CONNMGR_STATUS_WAITINGCONNECTIONABORT) {
                        LOG.debug("Internet connection not succeded.");
                        return FALSE;

                    }
                    Sleep(2000);

                    ConnMgrConnectionStatus(phWebConnection,&pdwStatus);
                    if (pdwStatus == CONNMGR_STATUS_WAITINGCONNECTION) {
                        // it is possible to do something...
                    }

                    if (pdwStatus == CONNMGR_STATUS_CONNECTIONCANCELED || pdwStatus == CONNMGR_STATUS_WAITINGCONNECTIONABORT) {
                        LOG.debug("Internet connection not succeded");
                        return FALSE;
                    }
                }
            }
            LOG.debug("Internet connection not succeded after connection process");
            return FALSE;
        }
        else {
            //Connection failed!
            LOG.info("Internet connection failed.");
            return FALSE;

        }

    }

}
