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

#include <CommDbConnPref.h>     // connection prefs (iap)

#include "push/FSocket.h"
#include "base/SymbianLog.h"
#include "base/util/stringUtils.h"
#include "base/util/symbianUtils.h"
#include "base/globalsdef.h"

USE_NAMESPACE

StringBuffer FSocket::lIP;




FSocket* FSocket::createSocket(const StringBuffer& peer, int32_t port) 
{
    return FSocket::NewL(peer, port);
}


FSocket* FSocket::NewL(const StringBuffer& peer, int32_t port)
{
    FSocket* self = FSocket::NewLC(peer, port);
    CleanupStack::Pop( self );
    return self;
}

FSocket* FSocket::NewLC(const StringBuffer& peer, int32_t port)
{
    FSocket* self = new ( ELeave ) FSocket();
    CleanupStack::PushL( self );
    self->ConstructL(peer, port);
    
    if (self->getLastStatus() != KErrNone) {
        // Something wrong.
        delete self;
        return NULL;
    }
    
    return self;
}


void FSocket::ConstructL(const StringBuffer& peer, int32_t port) 
{
    LOG.debug("FSocket::ConstructL");
    
    StringBuffer  errorMsg;
    RHostResolver resolver; 
    RBuf          serverName;
    TNameEntry    hostAddress;
    TInetAddr     address;

    serverName.Assign(stringBufferToNewBuf(peer));
    
    // Create the socket session
    iSocketSession.Connect();

    // Open the Client Socket tcp/ip
    TInt res = iSocket.Open(iSocketSession, KAfInet, KSockStream, KProtocolInetTcp);
    if (res != KErrNone) {
        iStatus = -1;
        errorMsg = "Error opening socket";
        goto error;
    }
    
    // This works if serverName is the ip address, like "x.y.z.w"
    res = address.Input(serverName);
    
    if (res != KErrNone) {
        // Try to resolve the host address
        LOG.debug("resolve IP address...");
        res = resolver.Open(iSocketSession, KAfInet, KProtocolInetTcp);
        if (res != KErrNone) {
            iStatus = -2;
            errorMsg = "Host resolver open failed";
            goto error;
        }
        
        resolver.GetByName(serverName, hostAddress, iStatus);
        User::WaitForRequest(iStatus);
        resolver.Close();
        if (iStatus != KErrNone) {
            errorMsg = "DNS lookup failed";
            goto error;
        }

        // Set the socket server address/port
        address = hostAddress().iAddr;
    }

    address.SetPort(port);
    
    
    // --- Connect to host ---
    LOG.debug("connect...");
    iSocket.Connect(address, iStatus);
    User::WaitForRequest(iStatus);
    if (iStatus != KErrNone) {
        errorMsg = "Failed to connect to Server";
        goto error;
    }
    
    return;
    
    
error:
    LOG.error(errorMsg.c_str());    // msgBox?
    iSocketSession.Close();
    return;
}



FSocket::FSocket() 
{
    iStatus = 0;
}

FSocket::~FSocket() 
{
    close();
}



int32_t FSocket::writeBuffer(const int8_t* const buffer, int32_t len) 
{
    // This doesn't copy the buffer in memory.
    TPtr8 data((TUint8*)buffer, len);
    
    // Sends data to the remote host.
    iSocket.Write(data, iStatus);
    User::WaitForRequest(iStatus);
    
    if (iStatus == KErrNone) {
        return len;
    }
    else {
        LOG.error("FSocket: error writing on socket (status = %d)", iStatus.Int());
        return -1;
    }
}


int32_t FSocket::readBuffer(int8_t* buffer, int32_t maxLen) 
{
    RBuf8 data;
    data.CreateL(maxLen);
    
    // Receives data from a remote host and completes when data is available.
    do {
        TSockXfrLength len;
        iSocket.RecvOneOrMore(data, 0, iStatus, len);
        User::WaitForRequest(iStatus);
        LOG.debug("received %d bytes...", data.Length());
    } while (iStatus == KErrNone);
    //
    // TODO: status error codes?
    //
    
    if (iStatus == KErrNone) {
        const char* ret = buf8ToNewChar(data);
        buffer = (int8_t*)ret;
        return data.Length();
    }
    else {
        LOG.error("FSocket: error reading on socket (status = %d)", iStatus.Int());
        buffer = NULL;
        return -1;
    }
}


void FSocket::close() 
{
    LOG.debug("FSocket::close");
    //iSocket.CancelAll();
    iSocket.Close();
    iSocketSession.Close();
}




const StringBuffer& FSocket::address() const {
    return lAddress;
}

const StringBuffer& FSocket::peerAddress() const {
    return pAddress;
}

const StringBuffer& FSocket::localIP() {
    return lIP;
}



void FSocket::startConnection()
{
    RConnection connection;
    RSocketServ socketServ;
    User::LeaveIfError(socketServ.Connect());
    
    // Use the default IAP without prompting the user
    TUint32 UidAP=0;
    TCommDbConnPref prefs;
    prefs.SetDialogPreference (ECommDbDialogPrefDoNotPrompt);
    prefs.SetDirection (ECommDbConnectionDirectionUnknown);
    prefs.SetIapId (UidAP);

    User::LeaveIfError (connection.Open (socketServ, KAfInet));
    User::LeaveIfError (connection.Start (prefs));

    // use this to search for a particular IAP to use
    // withouth prompting the user
#if defined(PREDEFINED_IAP)
    CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
    CleanupStack::PushL(commDb);
    CApSelect* select = CApSelect::NewLC(*commDb,KEApIspTypeAll,EApBearerTypeAll,KEApSortUidAscending);
    TBuf<256> accessPoint;
    TInt UidAP= 0;
    TBool ok = select->MoveToFirst();
    for (TInt32 i = 0; ok&&(i<select->Count()); i++)
        {
        if ( select->Name ()==_L(PREDEFINED_IAP))
            {
            UidAP = select->Uid ();
            TCommDbConnPref prefs;
            prefs.SetDialogPreference (ECommDbDialogPrefDoNotPrompt);
            prefs.SetDirection (ECommDbConnectionDirectionUnknown);
            prefs.SetIapId (UidAP);

            User::LeaveIfError (iConnection.Open (iSocketSession, KAfInet));
            User::LeaveIfError (iConnection.Start (prefs));

            }
        else
            {
            ok = select->MoveNext ();
            }
        }
#else
    // use this if you want prompt user for IAP
    TCommDbConnPref pref;
    pref.SetDirection(ECommDbConnectionDirectionUnknown);
    
    connection.Open(socketServ, KAfInet);
    connection.Start(pref);
#endif
    
    socketServ.Close();
}






