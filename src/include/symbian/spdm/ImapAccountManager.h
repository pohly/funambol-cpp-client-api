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

#ifndef INCL_SYMBIAN_IMAP_ACCOUNT_MANAGER
#define INCL_SYMBIAN_IMAP_ACCOUNT_MANAGER

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

BEGIN_NAMESPACE

/**
 * IMAP account manager.
 * Provide a small number of functions to create/delete an 
 * IMAP account.Link against imcm.lib msgs.lib apengine.lib commdb.lib.
 */
class CImapAccountManager: public CBase,public MMsvSessionObserver
{

public:

    /**
     * Two-phased constructor.
     */
    static CImapAccountManager* NewL();

    /**
     * Two-phased constructor.
     */
    static CImapAccountManager* NewLC();
    
    /**
     *  Destructor.
     */
    ~CImapAccountManager();
    
    /**
     * Create default IMAP account.
     * Create a default IMAP account; if a profile with the same
     * name already exists, it is duplicated.Can leave.
     * @param aAcountName The name assigned to the account.
     * @param aImapLogin The username used to connect to the email server.
     * @param aImapPassword The password used to connect to the email server.
     * @param aImapServer The IMAP server address.
     * @param aSmtpServerAddress The SMTP server address.
     * @param aEmailAlias The user email alias.
     * @param aIapName The IAP to be used; "Ask" sets "Always ask";
     *                 "<IAPName>" sets the specified IAP, if it doesn't
     *                 exists sets to "Always ask"; "Default" set the
     *                 default IAP as in packet data conn prefs. NOTE:
     *                    if the user change the default IAP, IMAP account
     *                 is not updated!!!
     */
    void CreateAccountL(const TDesC& aAccountName,const TDesC& aImapServer,
                        const TDesC8& aImapLogin,const TDesC8& aImapPassword,
                        const TDesC& aSmtpServerAddress,
                        const TDesC& aEmailAlias,const TDesC& aEmailAddress,
                        const TDesC& aIapName);
    
    /**
     * Delete specified IMAP account.
     * Delete specified IMAP account and associated SMTP account.Can leave.
     * @param aAccountName The name of deleting account.
     */
    void DeleteAccountL(const TDesC& aAccountName);
    
    /**
     * Implements virtual method of MMsvSessionObserver.
     */
    void HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                             TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/);

    //void setUsername(const StringBuffer& username);
    //void setPassword(const StringBuffer& password);

    
private:

    /**
     * C++ default constructor can NOT contain any code, that might leave.
     */
    CImapAccountManager();

    /**
     * Symbian 2nd phase constructor can leave.
     */
    void ConstructL();

    /**
     * Set internal IAP id.
     * Find the IAP id of the specified IAP and set the internal iIapId.
     * @param aIapName The name of the IAP.
     */
    void SetIapIdForConnProperties(const TDesC& aIapName);

    
    /** Pointer to the Client-side MTM registry*/
    CClientMtmRegistry* iClientRegistry;
    /** Pointer to the channel of communication between a client thread and the Message Server thread*/
    CMsvSession*        iSession;
    /** Pointer to the CBase MTM*/
    //CBaseMtm*            iMtm;
    CImap4ClientMtm*    iImap4Mtm;
    CSmtpClientMtm*        iSmtpMtm;
    
    /** Pointer to the CMsvOperation to get progress information about the operation*/
    CMsvOperation*        iOperation;
    /** Imap account Name*/
    TImapAccount        iImapAccount;
    /** Smtp account Name*/
    TSmtpAccount        iSmtpAccount;
    /** IAP Id */
    TUint32                iIapId;
};

END_NAMESPACE

#endif

