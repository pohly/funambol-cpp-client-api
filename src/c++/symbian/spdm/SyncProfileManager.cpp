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

USE_NAMESPACE

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

/**
 * CSyncProfileManager::NewL()
 * Two-phased constructor.
 */
CSyncProfileManager* CSyncProfileManager::createNewInstance()
{
    CSyncProfileManager* self = new(ELeave) CSyncProfileManager();
    return self;
}

/**
 * CSyncProfileManager::CSyncProfileManager()
 * C++ default constructor can NOT contain any code, that might leave.
 */
CSyncProfileManager::CSyncProfileManager() 
{
}

/**
 * CSyncProfileManager::~CSyncProfileManager()
 * Class destructor
 */
CSyncProfileManager::~CSyncProfileManager()
{
}

/**
 * CSyncProfileManager::CreateProfileL()
 * Create a default SyncML Funambol profile, calendar and contacts sync 
 * enabled.If a profile with the same name already exists, it is
 * overwritten.
 */
bool CSyncProfileManager::createProfile(const StringBuffer& username,
                                        const StringBuffer& password,
                                        const StringBuffer& serverURI,
                                        const StringBuffer& iapName)
{
    HBufC8* aUsername  = stringBufferToNewBuf8(username);
    HBufC8* aPassword  = stringBufferToNewBuf8(password);
    HBufC8* aServerURI = stringBufferToNewBuf8(serverURI);
    HBufC*  aIapName   = stringBufferToNewBuf(iapName);

    TRAPD(err, createProfileL(*aUsername, *aPassword, *aServerURI, *aIapName));

    delete aUsername;
    delete aPassword;
    delete aServerURI;
    delete aIapName;

    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::deleteProfile() {
    TRAPD(err, deleteProfileL());
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::enableTask(TSyncApp aSyncApp) {
    TRAPD(err, enableTaskL(aSyncApp, true));
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::disableTask(TSyncApp aSyncApp) {
    TRAPD(err, enableTaskL(aSyncApp, false));
    return err == KErrNone ? true : false;
}

/**
 * CSyncProfileManager::SetUsernameL()
 * Set the username to connect to sync server for the specified SyncML profile.
 */
bool CSyncProfileManager::setUsername(const StringBuffer& username) {
    if (username.null()) {
        return false;
    }
    HBufC8* aUsername = stringBufferToNewBuf8(username);
    TRAPD(err, setCredentialsL(aUsername, (TDesC8*)NULL));
    delete aUsername;
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::setPassword(const StringBuffer& password) {
    if (password.null()) {
        return false;
    }
    HBufC8* aPassword = stringBufferToNewBuf8(password);
    TRAPD(err, setCredentialsL((TDesC8*)NULL, aPassword));
    delete aPassword;
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::setCredentials(const StringBuffer& username,
                                         const StringBuffer& password) {
    if (password.null() || username.null()) {
        return false;
    }
    HBufC8* aPassword = stringBufferToNewBuf8(password);
    HBufC8* aUsername = stringBufferToNewBuf8(username);
    TRAPD(err, setCredentialsL(aUsername, aPassword));
    delete aPassword;
    delete aUsername;
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::setIapIdForConnProperties(const StringBuffer& iapName) {
    if (iapName.empty()) {
        return false;
    }
    HBufC* aIapName = stringBufferToNewBuf(iapName);
    TRAPD(err, setIapIdForConnPropertiesL(*aIapName));
    delete aIapName;
    return err == KErrNone ? true : false;
}

/**
 * CSyncProfileManager::DeleteProfileL()
 * Delet the specified SyncML profile.
 */
void CSyncProfileManager::deleteProfileL()
{
    TInt index;
    TSmlProfileId profileId = getProfileID(index);

    if (profileId < 0) {
        User::Leave(KErrNoProfile);
    }

    RSyncMLSession syncMLSession;
    TInt err=KErrNone;
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
    
    // delete the profile
    TRAP(err,syncMLSession.DeleteProfileL(index));
    User::LeaveIfError(err);
    // close SyncML session
    syncMLSession.Close();
}

/**
 * CSyncProfileManager::getProfileID()
 */
TSmlProfileId CSyncProfileManager::getProfileID(TInt& index)
{
    RSyncMLSession syncMLSession;
    TSmlProfileId  profileId = -1;
    TInt err=KErrNone;

    index = -1;
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    if (err != KErrNone) {
        return -2;
    }
    
    // find the specified profile
    RArray<TSmlProfileId> profileArr;
    TRAP(err,syncMLSession.ListProfilesL(profileArr,ESmlDataSync));
    if (err != KErrNone) {
        return -2;
    }

    const StringBuffer& profileSb = DeviceManagementNode::getProfileName();
    HBufC* profile = stringBufferToNewBuf(profileSb);
    CleanupStack::PushL(profile);

    TInt count = profileArr.Count();
    for(int i=0;i<count;i++)
    {
        RSyncMLDataSyncProfile syncProfile;
        
        TRAP(err,syncProfile.OpenL(syncMLSession,profileArr[i],ESmlOpenRead));
        if (err != KErrNone) {
            return -2;
        }
        if(syncProfile.DisplayName().Compare(*profile)== 0)
        {
            profileId = syncProfile.Identifier();
            syncProfile.Close();
            index = i;
            break;
        }
        syncProfile.Close();
    }

    // Cleanup
    CleanupStack::PopAndDestroy(); // profile

    // close SyncML session
    syncMLSession.Close();
    return profileId;
}

#define CONTACTS_DATA_STORE_NAME _L("C:Contacts.cdb")
#define CALENDAR_DATA_STORE_NAME _L("C:Calendar")

void CSyncProfileManager::createProfileL(const TDesC8& aUsername,
                                         const TDesC8& aPassword,
                                         const TDesC8& /* aServerURI */,
                                         const TDesC& aIapName)
{
    LOG.debug("Creating Sync Profile");

    RSyncMLSession syncMLSession;
    RSyncMLDataSyncProfile syncProfile;

    TInt err=KErrNone;
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
    
    // create a profile
    TRAP(err,syncProfile.CreateL(syncMLSession));
    User::LeaveIfError(err);
    TSmlCreatorId creatorId = DeviceManagementNode::getUID();
    syncProfile.SetCreatorId(creatorId);

    // We ignore the aServerURI parameter and rather use the global setting that
    // is held in the DeviceManagementNode
    const StringBuffer& serverUri = DeviceManagementNode::getServerURI();
    HBufC8* server = stringBufferToNewBuf8(serverUri);
    CleanupStack::PushL(server);

    const StringBuffer& profileSb = DeviceManagementNode::getProfileName();
    HBufC* profile = stringBufferToNewBuf(profileSb);
    CleanupStack::PushL(profile);

    TRAP(err, {
        syncProfile.SetDisplayNameL(*profile);
        syncProfile.SetUserNameL(aUsername);
        syncProfile.SetPasswordL(aPassword);

        // protocol version, values are:
        // ESmlVersion1_1_2 = 1.1.2
        // ESmlVersion1_2 = 1.2
        syncProfile.SetProtocolVersionL(ESmlVersion1_1_2);
        // This is for accepting all sync requests
        syncProfile.SetSanUserInteractionL(ESmlEnableSync);
        //syncProfile.DeleteAllowed();
        // save profile
        syncProfile.UpdateL();
    });
    CleanupStack::PopAndDestroy(); // profile
    User::LeaveIfError(err);

    // create and enable tasks (applications)
    // retrieve needed data provider ids
    TSmlDataProviderId contactsProvider = -1;
    TSmlDataProviderId calendarProvider = -1;
   
    RArray<TSmlDataProviderId> dataProvidersArr;
    TRAP(err,syncMLSession.ListDataProvidersL(dataProvidersArr));
    User::LeaveIfError(err);
    TInt count = dataProvidersArr.Count();
    
    for(int i=0;i<count;i++)
    {
        RSyncMLDataProvider dataProvider;
        TRAP(err,dataProvider.OpenL(syncMLSession,dataProvidersArr[i]));
        User::LeaveIfError(err);

        // Bug fix: in Symbian FP1 devices the display name is not reliable for
        // all sources. We better rely on the DefaultDataStoreName which seems
        // consistent across different versions.
        //if(dataProvider.DisplayName().Compare(_L("Calendar"))==0)

        if(dataProvider.DefaultDataStoreName().Compare(CALENDAR_DATA_STORE_NAME)==0)
        {
            calendarProvider = dataProvider.Identifier();
        }
        else if(dataProvider.DefaultDataStoreName().Compare(CONTACTS_DATA_STORE_NAME)==0)
        {
            contactsProvider = dataProvider.Identifier();
        }
        dataProvider.Close();
    }
    
    if(contactsProvider == -1)
        User::Leave(KErrNoContactsProvider);
    // contacts
    // default Funambol server source is "card"
    RSyncMLTask contactsTask;

    const StringBuffer& cardURI = DeviceManagementNode::getCardURI();
    HBufC* cardURIBuf = stringBufferToNewBuf(cardURI);
    CleanupStack::PushL(cardURIBuf);
    TRAP(err,contactsTask.CreateL(syncProfile,contactsProvider,*cardURIBuf,CONTACTS_DATA_STORE_NAME));
    CleanupStack::PopAndDestroy();
    User::LeaveIfError(err);
    contactsTask.SetCreatorId(creatorId);

    TRAP(err, {
        contactsTask.SetEnabledL(ETrue);
        contactsTask.SetDisplayNameL(_L("Contacts"));
        contactsTask.SetDefaultSyncTypeL(ESmlTwoWay);
        contactsTask.SetFilterMatchTypeL(ESyncMLMatchDisabled);

        contactsTask.UpdateL();
    });
    User::LeaveIfError(err);
    contactsTask.Close();
    
    if(calendarProvider == -1)
        User::Leave(KErrNoCalendarProvider);
    // calendar 
    const StringBuffer& calURI = DeviceManagementNode::getCalURI();
    HBufC* calURIBuf = stringBufferToNewBuf(calURI);
    CleanupStack::PushL(calURIBuf);
    RSyncMLTask calendarTask;
    TRAP(err,calendarTask.CreateL(syncProfile,calendarProvider,*calURIBuf,CALENDAR_DATA_STORE_NAME));
    CleanupStack::PopAndDestroy(calURIBuf);
    User::LeaveIfError(err);
    calendarTask.SetCreatorId(creatorId);
    TRAP(err, {
        calendarTask.SetEnabledL(ETrue);
        calendarTask.SetDisplayNameL(_L("Calendar"));
        calendarTask.SetDefaultSyncTypeL(ESmlTwoWay);
        calendarTask.SetFilterMatchTypeL(ESyncMLMatchDisabled);
        calendarTask.UpdateL();
    });
    User::LeaveIfError(err);
    
    calendarTask.Close();

    // SAVE PROFILE AGAIN to save tasks!!!!
    TRAP(err,syncProfile.UpdateL());
    User::LeaveIfError(err);

    // set server URI and IapId
    // if NSmlIapId = -1, user is asked for IAP
    // otherwise it is set to the specified IapId
    // useful property names: NSmlIapId,NSmlIapId2,NSmlIapId3,NSmlIapId4,
    // NSmlUseRoaming,NSmlHTTPAuth,NSmlHTTPUsername,NSmlHTTPPassword
    RArray<TSmlConnectionId> connArr;
    TRAP(err,syncProfile.ListConnectionsByConnectionIdL(connArr));
    User::LeaveIfError(err);
    TInt count2 = connArr.Count();
    for(int i=0;i<count2;i++)
    {
        RSyncMLConnection syncConnection;
        TRAP(err,syncConnection.OpenByConnectionIdL(syncProfile,connArr[i]));
        User::LeaveIfError(err);
        setIapIdForConnPropertiesL(aIapName);

        TRAP(err, {
            syncConnection.SetPriority(0);
            syncConnection.SetRetryCount(2);
            syncConnection.SetServerURIL(*server);
            syncConnection.SetPropertyL(_L8("NSmlIapId"),iIapId);
            //syncConnection.SetPropertyL(_L8("NSmlTransportId"),_L8("0x101F99F0"));
            syncConnection.UpdateL();
        });
        User::LeaveIfError(err);
        syncConnection.Close();
    }
   
    // Cleanup server as it is no longer needed
    CleanupStack::PopAndDestroy(); // server

    // Update the profile
    TRAP(err,syncProfile.UpdateL());
    User::LeaveIfError(err);
    
    // close profile
    syncProfile.Close();
    
    // close SyncML session     
    syncMLSession.Close();
}


/**
 * CSyncProfileManager::SetUsernameL()
 * Set the username to connect to sync server for the specified SyncML profile.
 */
void CSyncProfileManager::setCredentialsL(const TDesC8* aUsername,
                                          const TDesC8* aPassword)
{
    RSyncMLSession syncMLSession;
    TInt err=KErrNone;

    LOG.debug("Sync Profile: setCredentialsL");

    if (aUsername == NULL && aPassword == NULL) {
        return;
    }

    TInt index;
    TSmlProfileId profileId = getProfileID(index);
    if (profileId < 0) {
        // The profile cannot be found, we must create one

        TInt errCreate;
        TBuf8<8> emptyServer((const unsigned char*)"");
        if (aUsername == NULL) {
            TBuf8<8> username((const unsigned char*)"");
            TRAP(errCreate, createProfileL(username, *aPassword,
                                           emptyServer, _L("Ask")));
        } else if (aPassword == NULL) {
            TBuf8<8> password((const unsigned char*)"");
            TRAP(errCreate, createProfileL(*aUsername, password,
                                           emptyServer, _L("Ask")));
        } else {
            TRAP(errCreate, createProfileL(*aUsername, *aPassword,
                                           emptyServer, _L("Ask")));
        }

        User::LeaveIfError(errCreate);
        return;
    }

    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);

    // Modify the profile
    RArray<TSmlProfileId> profileArr;
    TRAP(err,syncMLSession.ListProfilesL(profileArr,ESmlDataSync));
    User::LeaveIfError(err);
    RSyncMLDataSyncProfile syncProfile;
    TRAP(err, syncProfile.OpenL(syncMLSession, profileArr[index],ESmlOpenReadWrite));
    User::LeaveIfError(err);
    TRAP(err, {
        if (aUsername) {
            syncProfile.SetUserNameL(*aUsername);
        }
        if (aPassword) {
            syncProfile.SetPasswordL(*aPassword);
        }
        syncProfile.UpdateL();
    });
    User::LeaveIfError(err);
    syncProfile.Close();
    
    // close SyncML session
    syncMLSession.Close();
}


/**
 * CSyncProfileManager::EnableTaskL()
 * Enable sync for the specified application, ESyncContacts or ESyncCalendar.
 */
void CSyncProfileManager::enableTaskL(TSyncApp aSyncApp, bool enable)
{
    RSyncMLSession syncMLSession;
    TSmlProfileId  profileId = -1;
    RSyncMLDataSyncProfile syncProfile;
    TInt err=KErrNone;
    
    TBuf<32> KSyncApp;
    switch(aSyncApp)
    {
        case ESyncCalendar:
        {
            KSyncApp.Copy(_L("C:Calendar")); //C:Calendar is the client source
            break;
        }
        case ESyncContacts:
        {
            KSyncApp.Copy(_L("C:Contacts.cdb")); //C:Contacts.cdb is the client source 
            break;
        }
        default:
        {
            User::Leave(KErrWrongSyncApp);
            break;
        }
    }

    TInt index;
    profileId = getProfileID(index);

    if(profileId == -1)
        User::Leave(KErrNoProfile);

    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
    
    // find the specified task
    RArray<TSmlTaskId> taskArr;
    
    TRAP(err,syncProfile.OpenL(syncMLSession,profileId,ESmlOpenReadWrite));
    User::LeaveIfError(err);
    TRAP(err,syncProfile.ListTasksL(taskArr));
    User::LeaveIfError(err);
    
    TInt count2 = taskArr.Count();
    for(int i=0;i<count2;i++)
    {
        RSyncMLTask syncTask;
        TRAP(err,syncTask.OpenL(syncProfile,taskArr[i]));
        User::LeaveIfError(err);
        if(syncTask.ClientDataSource().Compare(KSyncApp)==0)
        {
            // enable task
            TRAP(err, {
                syncTask.SetEnabledL(enable ? (TBool)ETrue : (TBool)EFalse);
                syncTask.UpdateL();
            });
            User::LeaveIfError(err);
            syncTask.Close();
            break;
        }
        syncTask.Close();
    }
    // REMEMBER to update profile or changes won't take effect!!
    TRAP(err,syncProfile.UpdateL());
    User::LeaveIfError(err);
    // close sync profile
    syncProfile.Close();
    // close SyncML session
    syncMLSession.Close();
}

/*
void CSyncProfileManager::TestL(void)
{
    //CSyncMLTransportPropertiesArray* propArr;
    
    RSyncMLSession syncMLSession;
    RSyncMLDataSyncProfile syncProfile;
    TSmlProfileId  profileId = -1;
    TInt err;
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
        
    // find the specified profile
    RArray<TSmlProfileId> profileArr;
    TRAP(err,syncMLSession.ListProfilesL(profileArr,ESmlDataSync));
    User::LeaveIfError(err);
    TInt count = profileArr.Count();
    for(int i=0;i<count;i++)
    {
        TRAP(err,syncProfile.OpenL(syncMLSession,profileArr[i],ESmlOpenRead));
        User::LeaveIfError(err);
        if(syncProfile.DisplayName().Compare(_L("Pava"))== 0)
        {
            CEikonEnv::InfoWinL(_L("Found profile!"),_L(""));
            profileId = syncProfile.Identifier();
            syncProfile.Close();
            break;
        }
        syncProfile.Close();
    }
    
    if(profileId == -1)
        User::Leave(KErrNoProfile);
    
    syncProfile.OpenL(syncMLSession,profileId,ESmlOpenRead);
    
    // find all connections
    RArray<TSmlTransportId> transportArr;
    TRAP(err,syncProfile.ListConnectionsL(transportArr));
    User::LeaveIfError(err);
    TInt count2 = transportArr.Count();
    for(int i=0;i<count2;i++)
    {
        RSyncMLTransport syncTransport;
        
        TRAP(err,syncTransport.OpenL(syncMLSession,transportArr[i]));
        User::LeaveIfError(err);
        
        CEikonEnv::InfoWinL(_L("transport name"),syncTransport.DisplayName());
        
        TInt count3 = syncTransport.Properties().Count();
        TBuf<32> propCount;
        propCount.AppendNum(count3);
        CEikonEnv::InfoWinL(_L("properties count"),propCount);
        for(i=0;i<count3;i++)
        {
            TBuf<KSmlMaxTransportPropertyNameLen> propName;
            propName.Copy(syncTransport.Properties().At(i).iName);
            CEikonEnv::InfoWinL(_L("name:"),propName);
            //
            // found properties:
            // NSmlIapId (-1: always ask; otherwise IapId)
            // NSmlIapId2
            // NSmlIapId3
            // NSmlIapId4
            // NSmlUseRoaming
            // NSmlHTTPAuth
            // NSmlHTTPUsername
            // NSmlHTTPPassword
            //
            
            if(propName.Compare(_L("NSmlIapId"))==0)
            {
                CEikonEnv::InfoWinL(_L("gotcha"),_L(""));
                
                RSyncMLConnection syncConnection;
                syncConnection.OpenL(syncProfile,syncTransport.Identifier());
                TBuf<KSmlMaxTransportPropertyNameLen> propValue;
                propValue.Copy(syncConnection.GetPropertyL(syncTransport.Properties().At(i).iName));
                CEikonEnv::InfoWinL(_L("NSmlIapId:"),propValue);
                syncConnection.Close(); 
            }
        }
        syncTransport.Close();
        
    }
    
    syncProfile.Close();
    syncMLSession.Close();
}
*/

/**
 * CSyncProfileManager::SetIapIdForConnProperties()
 * Set the private member iIapId to the IAP id of the specified IAP.
 * It is used by CreateProfileL().
 * If aIapName is set to Ask, then the user will be asked every time;
 * if aIapName is set to <iapname>, then that particular IAP will be used,
 * if the IAP is not found, the default IAP is set;
 * if aIapName is set to Default, the default IAP set into packet data pref
 * is used. NOTE: the default set is a static set, if user change the
 * default IAP, the change is not propagated to the SyncML profile. 
 */
void CSyncProfileManager::setIapIdForConnPropertiesL(const TDesC& aIapName)
{
    
    if(aIapName.Compare(_L("Ask"))==0)
    {
        iIapId.Zero();
        iIapId.Copy(_L8("-1"));
    } 
    else if(aIapName.Compare(_L("Default"))==0)
    {
        // remember that this is a static set:
        // if the default IAP is changed, the change
        // is not reflected in the SyncML profile
        TUint32 iapId;
        // open the IAP communications database
        CCommsDatabase* commDB = CCommsDatabase::NewL();
        CleanupStack::PushL(commDB);
        // create a view in rank order
        CCommsDbConnectionPrefTableView* view;
        view = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionOutgoing);
        // Point to the first entry, the one set as default
        if (view->GotoFirstRecord() == KErrNone)
        {
            CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
            view->ReadConnectionPreferenceL(pref);
            iapId=pref.iBearer.iIapId;
        
            iIapId.Zero();
            iIapId.AppendNum(iapId);
        
        }
        CleanupStack::PopAndDestroy(); // view
        CleanupStack::PopAndDestroy(); // commDB
    }
    else
    {
        TBool found = EFalse;
        
        //find specified IAP id
        CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        CleanupStack::PushL(commDb);
        CApSelect* select = CApSelect::NewLC(*commDb, KEApIspTypeAll,
                                             EApBearerTypeAll,
                                             KEApSortUidAscending);
        TInt32 iapId= 0;
        TBool ok = select->MoveToFirst();
        for (TUint32 i = 0; ok&&(i<select->Count()); i++)
        {
            if ( select->Name() == aIapName)
            {
                found = ETrue;
                iapId = select->Uid();
                iIapId.Zero();
                iIapId.AppendNum(iapId);
            }
            else
            {
                ok = select->MoveNext ();
            }
        }
        CleanupStack::PopAndDestroy(2); //commdb and select
        if(!found)
            setIapIdForConnPropertiesL(_L("Default"));
    }
}

