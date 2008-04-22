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

#ifndef INCL_FCONNECTION
#define INCL_FCONNECTION

#include <es_sock.h>            // RConnection
#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "base/globalsdef.h"

BEGIN_NAMESPACE

/**
 * This class is a singleton, used to manage the network connection.
 * Get the only instance of FConnection calling FConnection::getInstance().
 * Owns the RSocketServ and RConnection of the current connection,
 * that can be used exterally calling getConnection() and getSession().
 */
class FConnection 
{

private:
    
    /// pointer to FConnection sole instance
    static FConnection*     iInstance;
    
    /// The Socket Server necessary to estabilish internet connection
    RSocketServ             iSession;
    
    /// The Network connection interface
    RConnection             iConnection;
    
    /// Current local IP adress of the device
    StringBuffer            iLocalIpAddress;
    
    /// Current IAP ID of the connection
    TUint32                 iIAP;
    
    /**
     *  Default IAP name for the connection.
     * Should be set at the beginning, calling setIAPDefaultName()
     */
    StringBuffer            iIAPDefaultName;
    
    /// The last error code
    TInt                    iLastError;

    
    // 1st and 2nd phase constructors
    static FConnection* NewL();
    static FConnection* NewLC();
    
    /**
     * Initialize the RConnection object connecting to network
     * with default preferences and without prompting the user.
     */
    void ConstructL();
    

protected:
    
    /// Default constructor.
    FConnection();
    
public:
    
    /// Method to get the sole instance of FConnection
    static FConnection* getInstance();

    
    /// Destructor. Closes the connection and the session.
    virtual ~FConnection();
    
    /**
     * Starts a new GPRS connection using the default IAP, which name
     * is stored in the member 'iIAPDefaultName'.
     * The method setIAPDefaultName() should be called before
     * this method, to set the IAP to be used - otherwise, the user
     * will be prompted to select the IAP.
     * @return  0 if no error
     */
    const int startConnection();
    
    /**
     * Starts a new GPRS connection, given the IAP name.
     * @param aIAPName  "Ask"    : if we want to prompt the user to select the IAP
     *                  "Default": will use the default IAP of the system, no prompts
     *                  "<name>" : will use the <name> IAP, no prompts. 
     *                             If not found, will prompt the user to select the IAP
     * @return  0 if no error
     */
    const int startConnection(const StringBuffer& aIAPName);
    
    /**
     * If a connection is active, it will be stopped.
     * @return  0 if no error
     */
    const int stopConnection();
    
    /**
     * Checks if the connection is active or down.
     * Between active connection, we search for the one started with startConnection().
     * @return  true if connection is active
     */
    const bool isConnected();
    
    /**
     * Returns the local IP address of the device.
     * TODO
     */
    const StringBuffer& getLocalIpAddress();
    
    /**
     * Sets the default IAP name, that will be used in startConnection().
     * This method should be called at the beginning, so that
     * subsequent calls to startConnection() will use this default IAP.
     * @param  aIAPName  the name of the IAP
     */
    void setIAPDefaultName(const StringBuffer& aIAPName);
    

    
    /// Returns a pointer to the (internally owned) RConnection.
    RConnection* getConnection() { return &iConnection; }

    /// Returns a pointer to the (internally owned) SocketServ.
    RSocketServ* getSession()    { return &iSession;    }

    /// Returns the last error code.
    TInt         getLastError()  { return iLastError;   }
};

END_NAMESPACE

#endif
