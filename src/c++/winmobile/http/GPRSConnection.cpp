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



#include "winsock.h"


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
    LOG.info(T("Establish connection: test internet connection status..."));
    ConnMgrConnectionStatus(phWebConnection, &pdwStatus);
    
    if (pdwStatus == CONNMGR_STATUS_CONNECTED) {
        LOG.info(T("Arleady connected"));
        //We are already connected!        
        return TRUE;
    }
    else {
        LOG.info(T("Not connected: try to connect..."));
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
        LOG.debug(T("Try to establish connection..."));
        if (ConnMgrEstablishConnection(&sConInfo,&phWebConnection) == S_OK) {
            LOG.debug(T("Start internet connection process..."));
            //We start successfully process!
            for (unsigned int k = 0; k < 6; k++) {
                ConnMgrConnectionStatus(phWebConnection,&pdwStatus);

                if (pdwStatus == CONNMGR_STATUS_CONNECTED) {
                    LOG.info(T("Internet connection succesfully completed."));
                    return TRUE;

                }
                else {
                    if (pdwStatus == CONNMGR_STATUS_CONNECTIONCANCELED || pdwStatus == CONNMGR_STATUS_WAITINGCONNECTIONABORT) {
                        LOG.debug(T("Internet connection not succeded."));
                        return FALSE;
            
                    }
                    Sleep(2000);
                    
                    ConnMgrConnectionStatus(phWebConnection,&pdwStatus);
                    if (pdwStatus == CONNMGR_STATUS_WAITINGCONNECTION) {
                        // it is possible to do something...
                    }
                    
                    if (pdwStatus == CONNMGR_STATUS_CONNECTIONCANCELED || pdwStatus == CONNMGR_STATUS_WAITINGCONNECTIONABORT) {
                        LOG.debug(T("Internet connection not succeded"));   
                        return FALSE;
                    }
                }
            }
            LOG.debug(T("Internet connection not succeded after connection process"));
            return FALSE;
        }
        else {
            //Connection failed!
            LOG.info(T("Internet connection failed."));
            return FALSE;
           
        }

    }
    
}
