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
#include <smldmadapter.h>

#include "spdm/SymbianDmRootObject.h"

#include "spdm/ImapAccountManager.h"


USE_NAMESPACE

// Change right plug-in UID to this variable
const TUid KUidPluInUID         = { 0x101f6e35 };

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

bool CImapAccountManager::AccountExists(CEmailAccounts* emailAccounts,
                                        const TDesC8& name) {

    RArray<TImapAccount> imapAccountsArr;
    emailAccounts->GetImapAccountsL(imapAccountsArr);
    TInt count = imapAccountsArr.Count();
    bool ret = false;

    for(int i=0; i<count; i++) {
        // find specified imap account
        HBufC8* tmp8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                                           imapAccountsArr[i].iImapAccountName);
        bool equal = (tmp8->Compare(name) == 0);
        delete tmp8;
        if(equal) {
            ret = true;
            break;
        }
    }
    return ret;
}

/**
 * CImapAccountManager::UpdateAccountL()
 * Create a default IMAP account. If an IMAP account with the same
 * name already exists, it is duplicated. 
 */
void CImapAccountManager::UpdateAccountL(const TDesC8* aImapLogin,
                                         const TDesC8* aImapPassword,
                                         const TDesC* aEmailAlias,
                                         const TDesC* aEmailAddress,
                                         const TDesC& aIapName)
{
    LOG.debug("UpdateAccountL");

    const StringBuffer& profileSb = DeviceManagementNode::getProfileName();
    HBufC8* profile = stringBufferToNewBuf8(profileSb);
    CleanupStack::PushL(profile);

    CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();

    // If the account does not exist, then we create it
    // native functionalities
    if (!AccountExists(emailAccounts, *profile)) {
        LOG.debug("Account does not exist, create a new one");
        CSymbianDMRootObject* dmRootObj = CSymbianDMRootObject::NewL(KUidPluInUID);
        dmRootObj->AddAccountL(*profile);
        delete dmRootObj;
    }

    // After creating the account we can modify its settings
    TImapAccount imapAccount;
    TSmtpAccount smtpAccount;

    // retrieve all imap accounts
    RArray<TImapAccount> imapAccountsArr;
    emailAccounts->GetImapAccountsL(imapAccountsArr);
    TInt count = imapAccountsArr.Count();

    bool found = false;
    for(int i=0; i<count; i++) {
        // find specified imap account
        HBufC8* tmp8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(
                                           imapAccountsArr[i].iImapAccountName);
        bool equal = (tmp8->Compare(*profile) == 0);
        delete tmp8;

        if(equal) {
            // retrieve imap account
            imapAccount=imapAccountsArr[i];
            // retrieve associated SMTP account
            emailAccounts->GetSmtpAccountL(imapAccount.iSmtpService,smtpAccount);
            found = true;
            break;
        }
    }
    if (!found) {
        LOG.debug("Cannot set account properties as the account cannot be found");
        CleanupStack::Pop(2);
        return;
    }

    // Get some parameters from the DeviceManagementNode
    const StringBuffer& smtpAddress = DeviceManagementNode::getSmtpServer();
    HBufC* smtp = stringBufferToNewBuf(smtpAddress);
    CleanupStack::PushL(smtp);

    const StringBuffer& imapAddress = DeviceManagementNode::getImapServer();
    HBufC* imap = stringBufferToNewBuf(imapAddress);
    CleanupStack::PushL(imap);

    unsigned int imapPort = DeviceManagementNode::getImapPort();
    unsigned int smtpPort = DeviceManagementNode::getSmtpPort();

    // complete IMAP settings
    CImImap4Settings *imap4Settings = new(ELeave)CImImap4Settings;
    CleanupStack::PushL(imap4Settings);
    emailAccounts->LoadImapSettingsL(imapAccount,*imap4Settings);
    imap4Settings->SetServerAddressL(*imap);
    imap4Settings->SetPort(imapPort);
    if (aImapLogin) {
        imap4Settings->SetLoginNameL(*aImapLogin); 
    }
    if (aImapPassword) {
        imap4Settings->SetPasswordL(*aImapPassword);
    }
    imap4Settings->SetFolderPathL(_L8("Unix path"));
    imap4Settings->SetDeleteEmailsWhenDisconnecting(EFalse);
    imap4Settings->SetDisconnectedUserMode(ETrue);
    imap4Settings->SetAcknowledgeReceipts(EFalse);
    imap4Settings->SetUpdatingSeenFlags(ETrue);
    imap4Settings->SetSyncRateL(0);
    imap4Settings->SetImapIdle(EFalse);
    emailAccounts->SaveImapSettingsL(imapAccount,*imap4Settings);

    // complete SMTP settings
    CImSmtpSettings *smtpSettings = new (ELeave) CImSmtpSettings();
    CleanupStack::PushL(smtpSettings);
    emailAccounts->LoadSmtpSettingsL(smtpAccount,*smtpSettings);
    smtpSettings->SetServerAddressL(*smtp);
    smtpSettings->SetPort(smtpPort);
    if (aEmailAlias) {
        StringBuffer tmp = bufToStringBuffer(*aEmailAlias);
        LOG.debug("Setting Email Alias to: %s", tmp.c_str());
        smtpSettings->SetEmailAliasL(*aEmailAlias);
    }
    if (aEmailAddress) {
        StringBuffer tmp = bufToStringBuffer(*aEmailAddress);
        LOG.debug("Setting Email Address to: %s", tmp.c_str());

        smtpSettings->SetEmailAddressL(*aEmailAddress);
        smtpSettings->SetReplyToAddressL(*aEmailAddress);
        smtpSettings->SetReceiptAddressL(*aEmailAddress);
    }
    smtpSettings->SetSMTPAuth(ETrue);
    if (aImapLogin) {
        smtpSettings->SetLoginNameL(*aImapLogin);
    }
    if (aImapPassword) {
        smtpSettings->SetPasswordL(*aImapPassword);
    }
    emailAccounts->SaveSmtpSettingsL(smtpAccount,*smtpSettings);
    emailAccounts->SetDefaultSmtpAccountL(smtpAccount);

    // delete others
    delete smtpSettings;
    delete imap4Settings;
    delete emailAccounts;
    CleanupStack::Pop(6); 
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

bool CImapAccountManager::setUsername(const StringBuffer& username)
{
    if (username.null()) {
        return false;
    }

    HBufC8* aUsername = stringBufferToNewBuf8(username);
    TRAPD(err, setCredentialsL(aUsername, NULL));
    delete aUsername;
    return err == KErrNone ? true : false;
}

bool CImapAccountManager::setPassword(const StringBuffer& password)
{
    if (password.null()) {
        return false;
    }

    HBufC8* aPassword = stringBufferToNewBuf8(password);
    TRAPD(err, setCredentialsL(NULL, aPassword));
    delete aPassword;
    return err == KErrNone ? true : false;
}

bool CImapAccountManager::setAddress(const StringBuffer& address) {

    if (address.null()) {
        return false;
    }
    HBufC* aAddress = stringBufferToNewBuf(address);
    TRAPD(err, UpdateAccountL(NULL, NULL,
                              NULL, aAddress, _L("Ask"));
    )
    delete aAddress;
    return err == KErrNone ? true : false;
}

bool CImapAccountManager::setDisplayName(const StringBuffer& name) {

    if (name.null()) {
        return false;
    }

    HBufC* aName = stringBufferToNewBuf(name);
    TRAPD(err, UpdateAccountL(NULL, NULL,
                              aName, NULL, _L("Ask"));
    )
    delete aName;
    return err == KErrNone ? true : false;

}

void CImapAccountManager::setCredentialsL(const TDesC8* aUsername,
                                          const TDesC8* aPassword)
{
    if (aUsername == NULL && aPassword == NULL) {
        return;
    }

    TInt errCreate;
    TRAP(errCreate, UpdateAccountL(aUsername, aPassword,
                                   NULL, NULL, _L("Ask")));
    User::LeaveIfError(errCreate);
}


