/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#ifndef INCL_SYMBIAN_SYNC_PROFILE_MANAGER
#define INCL_SYMBIAN_SYNC_PROFILE_MANAGER

#include <stdio.h>
#include <ctype.h>
#include <f32file.h>
#include <utf.h>

#include "base/util/utils.h"
#include "base/util/stringUtils.h"
#include "base/util/StringBuffer.h"
#include "base/fscapi.h"
#include "base/SymbianLog.h"
#include "spdm/spdmutils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"
#include "base/globalsdef.h"

#include <e32base.h>
#include <e32cmn.h>
#include <syncmlclient.h>
#include <syncmlclientds.h>
#include <syncmldef.h>
#include <syncmltransportproperties.h>
#include <CommDbConnPref.h>  // for IAP retrieval
#include <apselect.h>
#include <CDBPREFTABLE.H>

#include <cemailaccounts.h>
#include <imapset.h>
#include <IAPprefs.h>
#include <commdb.h>
#include <smtpset.h>
#include <mtclreg.h>
#include <IMPCMTM.H>
#include <smtcmtm.h>
#include <impcmtm.h>

#include "spdm/SyncProfileManager.h"

BEGIN_NAMESPACE

/**
 * Sync application enum.
 * The applications for which we enable/disable sync.They can 
 * be ESyncCalendar for the calendar application and ESyncContacts
 * for the contacts application.During the creation of the 
 * default Funambol profile they are both enabled.
 */
enum TSyncApp 
{
    ESyncCalendar,
    ESyncContacts
};

/**
 * Errors.
 * Errors specific to this wrapper
 * in addition to those listed into syncmlerr.h
 */
const TInt KErrProfileMgrErrorBase = -9250;
const TInt KErrNoContactsProvider = KErrProfileMgrErrorBase -1;
const TInt KErrNoCalendarProvider = KErrProfileMgrErrorBase -2;
const TInt KErrNoProfile = KErrProfileMgrErrorBase -3;
const TInt KErrWrongSyncApp = KErrProfileMgrErrorBase -4;

/*
 * SyncML profile manager.
 * Provide a small number of functions to create/delete a 
 * SyncML profile, change the username/password used to connect
 * to the sync servr, enable/disable the sync of calendar and 
 * contacts.Link against syncmlclientapi.lib, syncmldatafilter.lib, nsmltransporthandler.lib
 * commdb.lib apengine.lib
 */
class CSyncProfileManager: public CBase
{

public:

    static CSyncProfileManager* createNewInstance();
   
    /**
     *  Destructor.
     */
    ~CSyncProfileManager();

    bool createProfile(const StringBuffer& username, const StringBuffer& password,
                       const StringBuffer& serverURI, const StringBuffer& iapName);
    bool deleteProfile();
    bool setUsername(const StringBuffer& username);
    bool setPassword(const StringBuffer& password);
    bool setCredentials(const StringBuffer& username,
                        const StringBuffer& password);
    bool setIapIdForConnProperties(const StringBuffer& iapName);
    bool enableTask(TSyncApp aSyncApp);
    bool disableTask(TSyncApp aSyncApp);
    
private:

    /**
     * C++ default constructor can NOT contain any code, that might leave.
     */
    CSyncProfileManager();

    /**
     * Create default SyncML profile.
     * Create a default Funambol profile; both calendar and contacts 
     * are enabled for sync; server source for calendar is implicitly set
     * to "cal" and for contacts to "card".If a profile with the same
     * name already exists, it is overwritten.Can leave.
     * @param aUserName The username used to connect to the sync server.
     * @param aPassword The password used to connect to the sync server.
     * @param aServerURI The sync server URI.
     * @param aIapName The IAP to be used; "Ask" sets "Always ask";
     *                 "<IAPName>" sets the specified IAP, if it doesn't
     *                 exists sets to "Always ask"; "Default" set the
     *                 default IAP as in packet data conn prefs. NOTE:
     *                    if the user change the default IAP, SyncML profile
     *                 is not updated!!!
     */
    void createProfileL(const TDesC8& aUsername, const TDesC8& aPassword,
                        const TDesC8& aServerURI, const TDesC& aIapName);
    
    /**
     * Delete specified SyncML profile.
     * Delete specified SyncML profile.Can leave.
     */
    void deleteProfileL();
    
    /**
     * Set username.
     * Set the username to connect to sync server for the specified 
     * profile.Can leave.
     * @param aUsername The username used to connect.
     */
    void setUsernameL(const TDesC8& aUsername);

    /**
     * Set password.
     * Set the password to connect to sync server for the specified 
     * profile.Can leave.
     * @param aPassword The password used to connect.
     */
    void setPasswordL(const TDesC8& aPassword);
    
    /*
     * Enable sync application.
     * Enable the specified sync application for the specified 
     * profile.Can leave.
     * @param aSyncApp The application to be enabled for sync.
     */
    void enableTaskL(TSyncApp aSyncApp, bool enable);

    /**
     * Set internal IAP id.
     * Find the IAP id of the specified IAP and set the internal iIapId.
     * @param aIapName The name of the IAP.
     */
    void setIapIdForConnPropertiesL(const TDesC& aIapName); 

    void setCredentialsL(const TDesC8* aUsername, const TDesC8* aPassword);

    TSmlProfileId getProfileID(TInt& index);

    /*
     * descriptor storing the IAP id, used by CreateProfileL.
     */
    TBuf8<32>   iIapId;
};

END_NAMESPACE

#endif

