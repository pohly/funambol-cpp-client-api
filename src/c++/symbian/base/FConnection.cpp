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

#include <CommDbConnPref.h>     // connection prefs (IAP)
#include <apselect.h>           // IAP selection
#include <in_sock.h>
#include <es_enum.h>            // TConnectionInfo

#include "base/FConnection.h"
#include "base/SymbianLog.h"
#include "base/util/stringUtils.h"
#include "base/util/symbianUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE


// Init static pointer.
FConnection* FConnection::iInstance = NULL;

_LIT(KIAPSettingName, "IAP\\Id");


/**
 * Method to create the sole instance of FConnection
 */
FConnection* FConnection::getInstance() {

    if (iInstance == NULL) {
        iInstance = FConnection::NewL();
    }
    return iInstance;
}

FConnection* FConnection::NewL() 
{
    FConnection* self = FConnection::NewLC();
    CleanupStack::Pop( self );
    return self;
}

FConnection* FConnection::NewLC() 
{
    FConnection* self = new ( ELeave ) FConnection();
    CleanupStack::PushL( self );
    self->ConstructL();
    
    if (self->getLastError() != KErrNone) {
        // Something wrong.
        delete self;
        return NULL;
    }
    
    return self;
}


void FConnection::ConstructL() 
{
    iLastError = KErrNone;
    StringBuffer errMsg;
    TCommDbConnPref prefs;
    
    // Connect SocketServer session
    TRAP(iLastError, iSession.Connect());
    if (iLastError != KErrNone) {
        errMsg.sprintf("FConnection error: unable to connect SocketServ (code %d)", iLastError);
        goto error;
    }
    
    
    //
    // Default preferences: no prompt, just to initialize the connection object
    //
    prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
    prefs.SetDirection(ECommDbConnectionDirectionUnknown);
    prefs.SetIapId(0);
    
    // Start connection
    TRAP(iLastError, 
        { iConnection.Open(iSession, KAfInet);
          iConnection.Start(prefs);
        }
    )
    if (iLastError != KErrNone) {
        errMsg.sprintf("FConnection error: unable to open/start connection (code %d)", iLastError);
        goto error;
    }
    return;

error:
    LOG.error(errMsg.c_str());
}



FConnection::FConnection() : iLocalIpAddress("127.0.0.1"),
                             iIAP(0) {
}


FConnection::~FConnection() {
    iConnection.Close();
    iSession.Close();
}


const int FConnection::startConnection() {
    return startConnection("Default");
}

const int FConnection::startConnection(const StringBuffer& aIAPName)
{
    LOG.debug("Starting gprs connection...");
    iLastError = KErrNone;
    StringBuffer errMsg;
    TCommDbConnPref prefs;
    
    // TODO: can we remove this?
    TRAP(iLastError, iSession.Connect());
    if (iLastError != KErrNone) {
        errMsg.sprintf("FConnection error: unable to connect SocketServ (code %d)", iLastError);
        goto error;
    }
    
    prefs.SetDirection(ECommDbConnectionDirectionUnknown);
     
    if (aIAPName == "Default") {
        //
        // Use the default IAP without prompting the user
        //
        prefs.SetDialogPreference (ECommDbDialogPrefDoNotPrompt);
        prefs.SetIapId(0);
        iIAP = 0;
    }
    else if (aIAPName == "Ask") {
        //
        // Prompt user for IAP selection
        //
    }
    else {
        //
        // Search for the desired IAP. If not found, will prompt the user.
        //
        TUint32 UidAP = 0;
        RBuf iapName;
        iapName.Assign(stringBufferToNewBuf(aIAPName));
        
        CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        CleanupStack::PushL(commDb);
        CApSelect* select = CApSelect::NewLC(*commDb,KEApIspTypeAll,EApBearerTypeAll,KEApSortUidAscending);
        TBuf<256> accessPoint;
        TBool ok = select->MoveToFirst();
        for (TInt32 i = 0; ok&&(i<select->Count()); i++) 
        {
            if (select->Name() == iapName) {
                UidAP = select->Uid();
                prefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
                prefs.SetIapId(UidAP);
                break;
            }
            else {
                ok = select->MoveNext();
            }
        }
        CleanupStack::PopAndDestroy(2);    //commdb and select
    }
    

/*
 In S60 3rd Edition, enabling/disabling the inactivity timer will require 
 the NetworkControl capability, which is only accessed via Symbian 
 partner. 
 */
//#if !defined(__SERIES60_3X__) || defined(__S60_3X_NET_CTRL__)
//    // Disable inactivity timer, otherwise inactive connection is closed 
//    // within few seconds if there are no activity (e.g sockets)
//    iConnection.SetOpt(KCOLProvider, KConnDisableTimers, ETrue);
//#endif
    

    //
    // Start connection
    //
    TRAP(iLastError, 
        { iConnection.Open(iSession, KAfInet);
          iConnection.Start(prefs);
        }
    )
    if (iLastError != KErrNone) {
        errMsg.sprintf("FConnection error: unable to open/start connetion (code %d)", iLastError);
        goto error;
    }
    
    // Save the IAP ID of this connection
    iConnection.GetIntSetting(KIAPSettingName, iIAP);
    LOG.debug("Current IAP ID = %d", iIAP);
    
    return 0;
    
error:
    LOG.error(errMsg.c_str());
    return iLastError;
}


const bool FConnection::isConnected() 
{
    iLastError = KErrNone;
    bool connected = false;
    TUint count;
    
    //Enumerate currently active connections
    TRAP(iLastError, iConnection.EnumerateConnections(count));
    if (iLastError != KErrNone) {
        LOG.error("Error checking active connections (code %d)", iLastError);
        return false;
    }

    //LOG.debug("Number of active connections = %d", count);
    if (count) {
        TPckgBuf<TConnectionInfoV2> connectionInfo;
        for (TUint i = 1; i <= count; ++i) {
            iConnection.GetConnectionInfo(i, connectionInfo);
            if (connectionInfo().iIapId == iIAP) {
                connected = true;
                break;
            }
        }
    }
    return connected;
}

const int FConnection::stopConnection() 
{
    iLastError = KErrNone;
    if (isConnected()) 
    {
        LOG.debug("Stopping the current connection");
        iLastError = iConnection.Stop(/*RConnection::EStopAuthoritative*/);
    }
    return iLastError;
}


const StringBuffer& FConnection::getLocalIpAddress() 
{
    //
    // TODO: http://wiki.forum.nokia.com/index.php/LocalDeviceIpAddress
    //
    return iLocalIpAddress;
}






