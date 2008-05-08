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

#include "spdm/ImapAccountManager.h"

/**
 * CImapAccountManager::NewL()
 * Two-phased constructor.
 */
CImapAccountManager* CImapAccountManager::NewL()
{
    CImapAccountManager* self = CImapAccountManager::NewLC();
    CleanupStack::Pop(self);
    return self;
}

/**
 * CImapAccountManager::NewLC()
 * Two-phased constructor.
 */
CImapAccountManager* CImapAccountManager::NewLC()
{
    CImapAccountManager* self = new(ELeave) CImapAccountManager();
    self->ConstructL();
    CleanupStack::PushL(self);
    return self;
}

/**
 * CImapAccountManager::CImapAccountManager()
 * C++ default constructor can NOT contain any code, that might leave.
 */
CImapAccountManager::CImapAccountManager() 
{
}

/**
 * CImapAccountManager::ConstructL()
 * Symbian 2nd phase constructor can leave.
 */
void CImapAccountManager::ConstructL()
{
    iSession=CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(iSession);
    //iSession->InstallMtmGroup(_L("c:\\system\\mtm\\impc.dat"));
    // Create a client-side MTM registry
    iClientRegistry=CClientMtmRegistry::NewL(*iSession);
    CleanupStack::PushL(iClientRegistry);
    // Create a Client-side MTM object for the specified MTM UID.
    //iMtm=(CImap4ClientMtm*)iClientRegistry->NewMtmL(KUidMsgTypeIMAP4); 
    iImap4Mtm = static_cast<CImap4ClientMtm*>(iClientRegistry->NewMtmL(KUidMsgTypeIMAP4)); 
    iSmtpMtm = static_cast<CSmtpClientMtm*>(iClientRegistry->NewMtmL(KUidMsgTypeSMTP));
    CleanupStack::Pop(2,iSession);
}

/**
 * CImapAccountManager::~CImapAccountManager()
 * Class destructor.
 */
CImapAccountManager::~CImapAccountManager()
{
    delete iImap4Mtm;
    delete iSmtpMtm;
    //delete iMtm;
    delete iClientRegistry;
    delete iSession;
}

/**
 * CImapAccountManager::CreateAccountL()
 * Create a default IMAP account. If an IMAP account with the same
 * name already exists, it is duplicated. 
 */
void CImapAccountManager::CreateAccountL(const TDesC& aAccountName,
                                         const TDesC& aImapServer,
                                         const TDesC8& aImapLogin,
                                         const TDesC8& aImapPassword,
                                         const TDesC& aSmtpServer,
                                         const TDesC& aEmailAlias,
                                         const TDesC& aEmailAddress,
                                         const TDesC& aIapName)
{
    TInt err = KErrNone;
    
    CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();
    
    // add an IAP to the email IAP preferences
    SetIapIdForConnProperties(aIapName);
    TImIAPChoice iap;
    iap.iIAP = 15; // to be changed when working
    iap.iDialogPref = ECommDbDialogPrefDoNotPrompt; // ECommDbDialogPrefDoNotPrompt
    
    CImIAPPreferences* prefs = CImIAPPreferences::NewLC();
    
    // set IMAP settings
    // first populate with default settings
    CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
    CleanupStack::PushL(imap4Settings);
    TRAP(err, {
        emailAccounts->PopulateDefaultImapSettingsL(*imap4Settings, *prefs);
        // then overwrite specific settings
        imap4Settings->SetServerAddressL(aImapServer);
        imap4Settings->SetLoginNameL(aImapLogin); 
        imap4Settings->SetPasswordL(aImapPassword);
        //imap4Settings->SetPort(143);
        prefs->AddIAPL(iap);
        // create IMAP account
        iImapAccount = emailAccounts->CreateImapAccountL(aAccountName,
                                                         *imap4Settings,
                                                         *prefs, EFalse);
    });
    User::LeaveIfError(err);
    
    // set SMTP settings
    CImSmtpSettings *smtpSettings = new (ELeave) CImSmtpSettings();
    CleanupStack::PushL(smtpSettings);
    // first populate with default settings
    TRAP(err, {
        emailAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings, *prefs);
        // then overwrite specific settings
        smtpSettings->SetServerAddressL(aSmtpServer);
        smtpSettings->SetEmailAliasL(aEmailAlias);
        smtpSettings->SetEmailAddressL(aEmailAddress);
        smtpSettings->SetReplyToAddressL(aEmailAddress);
        smtpSettings->SetReceiptAddressL(aEmailAddress);
        //smtpSettings->SetPort(25);
        prefs->AddIAPL(iap);
         // Create SMTP account
        iSmtpAccount= emailAccounts->CreateSmtpAccountL(iImapAccount,
                                                        *smtpSettings,
                                                        *prefs, EFalse);
    });
    User::LeaveIfError(err);
    // set it as default
    TRAP(err,emailAccounts->SetDefaultSmtpAccountL(iSmtpAccount));
    User::LeaveIfError(err);
    //connPrefRecord,dbSession,prefs,smtpSettings,popIAP,settings,emailAccounts
    CleanupStack::PopAndDestroy(4,emailAccounts);

}

/**
 * CImapAccountManager::DeleteAccountL()
 * Delet the specified IMAP account.
 */
void CImapAccountManager::DeleteAccountL(const TDesC& aAccountName)
{
    TInt err = KErrNone;
    
    CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();
    
    // find all imap accounts
    RArray<TImapAccount> imapAccountsArr;
    TRAP(err,emailAccounts->GetImapAccountsL(imapAccountsArr));
    User::LeaveIfError(err);
    TInt count = imapAccountsArr.Count();
    for(int i=0; i<count; i++)
    {
        // find specified imap account
        if((imapAccountsArr[i].iImapAccountName).Compare(aAccountName)==0)
        {
            // delete associated smtp account
            TSmtpAccount smtpAccount;
            TRAP(err, {
            emailAccounts->GetSmtpAccountL(imapAccountsArr[i].iSmtpService,smtpAccount);
            emailAccounts->DefaultSmtpAccountL(smtpAccount);
            // delete imap account
            emailAccounts->DeleteImapAccountL(imapAccountsArr[i]);
            });
            User::LeaveIfError(err);
        }
    }
    
    CleanupStack::PopAndDestroy(1,emailAccounts); 
}

#if 0
TImapAccount getAccount() {
    CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();
    
    // find all imap accounts
    RArray<TImapAccount> imapAccountsArr;
    TRAP(err,emailAccounts->GetImapAccountsL(imapAccountsArr));
    User::LeaveIfError(err);
    TInt count = imapAccountsArr.Count();
    for(int i=0; i<count; i++)
    {
        // find specified imap account
        if((imapAccountsArr[i].iImapAccountName).Compare(aAccountName)==0)
        {
            // delete associated smtp account
            TSmtpAccount smtpAccount;
            TRAP(err, {
            emailAccounts->GetSmtpAccountL(imapAccountsArr[i].iSmtpService,smtpAccount);
            emailAccounts->DefaultSmtpAccountL(smtpAccount);
            // delete imap account
            emailAccounts->DeleteImapAccountL(imapAccountsArr[i]);
            });
            User::LeaveIfError(err);
        }
    }
    
    CleanupStack::PopAndDestroy(1,emailAccounts); 
}
#endif


/**
 * CImapAccountManager::HandleSessionEventL()
 * Implements virtual method of MMsvSessionObserver.
 */
void CImapAccountManager::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                              TAny* /*aArg1*/,
                                              TAny* /*aArg2*/,
                                              TAny* /*aArg3*/)
{
}

/**
 * CImapAccountManager::SetIapIdForConnProperties()
 * Set the private member iIapId to the IAP id of the specified IAP.
 * It is used by CreateAccountL().
 * If aIapName is set to Ask, then the user will be asked every time;
 * if aIapName is set to <iapname>, then that particular IAP will be used,
 * if the IAP is not found, the default IAP is set;
 * if aIapName is set to Default, the default IAP set into packet data pref
 * is used. NOTE: the default set is a static set, if user change the
 * default IAP, the change is not propagated to the IMAP account.
 */
void CImapAccountManager::SetIapIdForConnProperties(const TDesC& aIapName)
{
        
    if(aIapName.Compare(_L("Ask"))==0) {
        iIapId = 0;
    } 
    else if(aIapName.Compare(_L("Default"))==0) {
        // remember that this is a static set:
        // if the default IAP is changed, the change
        // is not reflected in the IMAP account
        // open the IAP communications database
        CCommsDatabase* commDB = CCommsDatabase::NewL();
        CleanupStack::PushL(commDB);
        // create a view in rank order
        CCommsDbConnectionPrefTableView* view;
        view = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionOutgoing);
        // Point to the first entry, the one set as default
        if (view->GotoFirstRecord() == KErrNone) {
            CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
            view->ReadConnectionPreferenceL(pref);
            iIapId=pref.iBearer.iIapId;
        }
        CleanupStack::PopAndDestroy(); // view
        CleanupStack::PopAndDestroy(); // commDB
    } else {
        TBool found = EFalse;
            
        //find specified IAP id
        CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        CleanupStack::PushL(commDb);
        CApSelect* select = CApSelect::NewLC(*commDb,KEApIspTypeAll,EApBearerTypeAll,KEApSortUidAscending);
        TBool ok = select->MoveToFirst();
        for (TInt32 i = 0; ok&&(i<(TInt32)select->Count()); i++) {
            if ( select->Name() == aIapName) {
                found = ETrue;
                iIapId = select->Uid();
            } else {
                ok = select->MoveNext ();
            }
        }
        CleanupStack::PopAndDestroy(2); //commdb and select
        if(!found)
            SetIapIdForConnProperties(_L("Default"));
    }
}

#if 0
bool CImapAccountManager::setUsername(const StringBuffer& username) {
    if (username.null()) {
        return false;
    }
    HBufC8* aUsername = stringBufferToNewBuf8(username);
    TRAPD(err, setCredentialsL(aUsername, (TDesC8*)NULL));
    delete aUsername;
    return err == KErrNone ? true : false;
}

bool CImapAccountManager::setPassword(const StringBuffer& password) {
    if (password.null()) {
        return false;
    }
    HBufC8* aPassword = stringBufferToNewBuf8(password);
    TRAPD(err, setCredentialsL((TDesC8*)NULL, aPassword));
    delete aPassword;
    return err == KErrNone ? true : false;
}

void CImapAccountManager::setCredentialsL(const TDesC8* aUsername,
                                          const TDesC8* aPassword)
{
    RSyncMLSession syncMLSession;
    TInt err=KErrNone;

    LOG.debug("Imap Profile: setCredentialsL");

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
#endif


