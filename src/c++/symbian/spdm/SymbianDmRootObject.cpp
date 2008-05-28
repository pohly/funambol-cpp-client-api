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


// INCLUDE FILES
#include <coemain.h>
#include <aknutils.h>
#include <AknLists.h>

#include "base/globalsdef.h"
#include "base/util/stringUtils.h"
#include "base/Log.h"

#include "spdm/SymbianDmRootObject.h"

USE_NAMESPACE

CSymbianDMRootObject* CSymbianDMRootObject::NewL(const TUid& aUid)
{
    CSymbianDMRootObject* self = CSymbianDMRootObject::NewLC(aUid);
    CleanupStack::Pop( self );
    return self;
}

CSymbianDMRootObject* CSymbianDMRootObject::NewLC(const TUid& aUid)
{
    CSymbianDMRootObject* self = new ( ELeave ) CSymbianDMRootObject();
    CleanupStack::PushL( self );
    self->ConstructL(aUid);
    return self;
}

void CSymbianDMRootObject::ConstructL(const TUid& aUid)
{    
    iSmlDmAdapter = CSmlDmAdapter::NewL(aUid, *this);
    iSmlDmAdapter->DDFStructureL(*this);
    ChildURIListL(KNullDesC8);
}

CSymbianDMRootObject::CSymbianDMRootObject() {
}

CSymbianDMRootObject::~CSymbianDMRootObject()
{
    delete iNamesArray;
    iNamesArray = NULL;
    delete iSmlDmAdapter;
    iSmlDmAdapter = NULL;
    iGroups.ResetAndDestroy();
    iUrls.ResetAndDestroy();
    iLuids.ResetAndDestroy();
}

void CSymbianDMRootObject::ChildURIListL(const TDesC8& aUrl)
{
    iErrorCode = MSmlDmAdapter::EOk;
    
    CArrayFixSeg<TSmlDmMappingInfo>* lst = new CArrayFixSeg<TSmlDmMappingInfo>(1);
    CleanupStack::PushL(lst);
    
    TInt resultsRef(0), statusRef(0);
    
    if(aUrl.Length())
        iSmlDmAdapter->ChildURIListL(aUrl, KNullDesC8, *lst, resultsRef, statusRef);
    else
        iSmlDmAdapter->ChildURIListL(iName, KNullDesC8, *lst, resultsRef, statusRef);
    
    CleanupStack::PopAndDestroy( lst );
}

_LIT8(KTxEmailRoot    ,"Email");

TBool CSymbianDMRootObject::AddAccountL(const TDesC8& aName) {
    TBool res = NormalAddAccountL(aName);
    if (!res) {
        res = SafeAddAccountL(aName);
    }
    //return res;
    return EFalse;
}

TBool CSymbianDMRootObject::NormalAddAccountL(const TDesC8& aName)
{
    LOG.debug("Adding IMAP account");
    TBool res = EFalse;

    iSmlDmAdapter->StartAtomicL();

    iSmlDmAdapter->AddNodeObjectL(_L8("Email/IMAP12346"),KNullDesC8,0);
    iSmlDmAdapter->UpdateLeafObjectL(_L8("Email/IMAP12346/Mpro"),KNullDesC8,
                                     _L8("IMAP"),_L8("text/plain"),0);
 
    iErrorCode = MSmlDmAdapter::EOk;
    iSmlDmAdapter->CompleteOutstandingCmdsL();

    if(iErrorCode == MSmlDmAdapter::EOk)
    {
        iSmlDmAdapter->CommitAtomicL();

        HBufC8* tmpLuid= GetLuidAllocL(_L8("Email/IMAP12346"));
        CleanupStack::PushL(tmpLuid);

        iSmlDmAdapter->StartAtomicL();
        iErrorCode = MSmlDmAdapter::EOk;
    
        iSmlDmAdapter->UpdateLeafObjectL(_L8("Email/IMAP12346/Name"), *tmpLuid,
                                         aName,_L8("text/plain"),0);
    
        iSmlDmAdapter->CompleteOutstandingCmdsL();
    
        if(iErrorCode == MSmlDmAdapter::EOk) {
            iSmlDmAdapter->CommitAtomicL();
            res = ETrue;
        } else {
            iSmlDmAdapter->RollbackAtomicL();
            res = EFalse;
        }
    
        CleanupStack::PopAndDestroy(tmpLuid);
    }
    else
    {
        iSmlDmAdapter->RollbackAtomicL();
        res = EFalse;
    }
    return res;
}

TBool CSymbianDMRootObject::SafeAddAccountL(const TDesC8& aName)
{
    LOG.debug("Adding account (fallback)");

    iSmlDmAdapter->StartAtomicL();

    iLuids.ResetAndDestroy();
    iUrls.ResetAndDestroy();
    ChildURIListL(KNullDesC8);

    // Copy the LUIDs before we create the new account
    RPointerArray<HBufC8>  origLuids;
    for(TInt i = 0;i<iLuids.Count(); ++i) {
        origLuids.Append(iLuids[i]);
    }
    iLuids.Reset();
    iUrls.ResetAndDestroy();

    iSmlDmAdapter->AddNodeObjectL(_L8("Email/IMAP12346"),KNullDesC8,0);
    iSmlDmAdapter->UpdateLeafObjectL(_L8("Email/IMAP12346/Mpro"),KNullDesC8,
                                     _L8("IMAP"),_L8("text/plain"),0);
 
    iErrorCode = MSmlDmAdapter::EOk;
    iSmlDmAdapter->CompleteOutstandingCmdsL();

    iSmlDmAdapter->CommitAtomicL();

    // We got to find the Luid
    ChildURIListL(KNullDesC8);
    bool done = false;
    for(TInt i = 0;i<iLuids.Count(); ++i) {
        HBufC8* luid = iLuids[i];
        bool found = false;
        for(TInt j=0;j<origLuids.Count(); ++j) {
            HBufC8* origLuid = origLuids[j];
            if (origLuid->Compare(*luid) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            // This is the LUID of the account we have just created
            StringBuffer l = buf8ToStringBuffer(*luid);
            iSmlDmAdapter->StartAtomicL();
            iErrorCode = MSmlDmAdapter::EOk;
    
            iSmlDmAdapter->UpdateLeafObjectL(_L8("Email/IMAP12346/Name"),
                                             *luid, aName,_L8("text/plain"),0);
    
            iSmlDmAdapter->CompleteOutstandingCmdsL();
    
            if(iErrorCode == MSmlDmAdapter::EOk) {
                iSmlDmAdapter->CommitAtomicL();
                done = true;
            } else {
                iSmlDmAdapter->RollbackAtomicL();
                done = false;
            }

            break;
        }
    }
    origLuids.ResetAndDestroy();

    return done;
}

TBool CSymbianDMRootObject::DeleteAccountL(const TDesC8& aNameString)
{    
    TBool Ret(EFalse);
        
    delete iNamesArray;
    iNamesArray = NULL;
    iNamesArray = new(ELeave)CDesCArrayFlat(10);
                    
    TBuf8<255> uri; 
    TInt resultsRef(0),statusRef(0);
    
    if(iUrls.Count() == iLuids.Count() &&  iUrls.Count())
    {
        for(TInt i=0; i < iUrls.Count(); i++)
        {
            if(iUrls[i] && iLuids[i])
            {
                uri.Copy(*iUrls[i]);
                uri.Append(aNameString);
                
                iSmlDmAdapter->FetchLeafObjectL(uri,*iLuids[i],_L8("text/plain"),
                                                resultsRef, statusRef);
            }
        }    
    }
    
    if(iNamesArray->MdcaCount())
    {
        TInt selEcted(-1);
        if(SelectAccountIndexL(iNamesArray,selEcted))
        {
            if(selEcted >= 0 && selEcted < iUrls.Count())
            {
                if(iUrls[selEcted] && iLuids[selEcted])
                {
                    Ret = ETrue;
                    statusRef = 0;
                    iSmlDmAdapter->DeleteObjectL(*iUrls[selEcted],*iLuids[selEcted],statusRef);
                }
            }
        }
    }
    
    delete iNamesArray;
    iNamesArray = NULL;

    return Ret;
}

TBool CSymbianDMRootObject::SelectAccountIndex(TInt& aIndex,const TDesC8& aNameString)
{    
    TBool Ret(EFalse);
    
    aIndex = -1;
    
    delete iNamesArray;
    iNamesArray = NULL;
    iNamesArray = new(ELeave)CDesCArrayFlat(10);
                    
    TBuf8<255> uri; 
    TInt resultsRef(0),statusRef(0);
    
    if(iUrls.Count() == iLuids.Count() &&  iUrls.Count())
    {
        for(TInt i=0; i < iUrls.Count(); i++)
        {
            if(iUrls[i] && iLuids[i])
            {
                uri.Copy(*iUrls[i]);
                uri.Append(aNameString);
                
                iSmlDmAdapter->FetchLeafObjectL(uri,*iLuids[i],_L8("text/plain"),
                                                resultsRef, statusRef);
        
            }
        }    
    }
    
    if(iNamesArray->MdcaCount())
    {
        Ret = SelectAccountIndexL(iNamesArray,aIndex);
    }
    
    delete iNamesArray;
    iNamesArray = NULL;

    return Ret;
}

TBool CSymbianDMRootObject::SelectAccountIndexL(CDesCArray* /* aArray */,
                                                TInt& /* aIndex */)
{
    TBool ret(EFalse);
#if 0    
    CAknSinglePopupMenuStyleListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL(list);

    CAknPopupList* popupList = CAknPopupList::NewL(list,R_AVKON_SOFTKEYS_SELECT_CANCEL);
    CleanupStack::PushL(popupList);

    list->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    list->CreateScrollBarFrameL(ETrue);
    list->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    list->Model()->SetItemTextArray(aArray);
    list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
    
    popupList->SetTitleL(_L("Select Account"));
    if(popupList->ExecuteLD())
    {
        ret = ETrue;
        aIndex = list->CurrentItemIndex();
    }

    CleanupStack::Pop();            // popuplist
    CleanupStack::PopAndDestroy();  // list
#endif
    return ret;
}

void CSymbianDMRootObject::GetSAllLeafsL(const TDesC8& aNode)
{    
    delete iNamesArray;
    iNamesArray = NULL;
                
    TBuf8<255> uri; 
    TInt resultsRef(0),statusRef(0);
    
    iUseUrl.Zero();
    iUseLuid.Zero();

    iUseUrl.Copy(aNode);
    
    LOG.debug("\r\n-- FetchLeafObjectL -- Start ----,\r\n");
    
    LOG.debug("Url = ");
    //Log.debug(iUseUrl);
    LOG.debug("\r\n");
            
    if(iGroups.Count())
    {
        if(iGroups[0])
        {
            resultsRef = statusRef = 0;
            
            for(TInt b=0; b < iGroups[0]->iChilds.Count(); b++)
            {
                if(iGroups[0]->iChilds[b])
                {
                    if(iGroups[0]->iChilds[b]->iFormat == MSmlDmDDFObject::ENode)
                    {
                        // node has no data,     
                    }
                    else
                    {
                        LOG.debug("\r\n--------Fetch - B ----\r\n");
                        LOG.debug("Url = ");
                        uri.Copy(iUseUrl);
                        uri.Append(iGroups[0]->iChilds[b]->iName);
                        //LOG.debug(uri);
                        
                        iPopulateObj =     iGroups[0]->iChilds[b];
                        
                        iSmlDmAdapter->FetchLeafObjectL(uri,KNullDesC8,
                                                        iGroups[0]->iChilds[b]->iMimeType,
                                                        resultsRef, statusRef);
                    
                        iPopulateObj = NULL;
                    }
                }
            }
        }
    }
    
    LOG.debug("\r\n-- FetchLeafObjectL -- End ----,\r\n");
}

void CSymbianDMRootObject::GetSAllLeafsL(TInt aIndex)
{    
    delete iNamesArray;
    iNamesArray = NULL;
                
    TBuf8<255> uri; 
    TInt resultsRef(0),statusRef(0);
    
    iUseUrl.Zero();
    iUseLuid.Zero();

    if(iUrls.Count() == iLuids.Count() && aIndex >= 0 && aIndex < iUrls.Count())
    {
        if(iUrls[aIndex] && iLuids[aIndex])
        {
            iUseUrl.Copy(*iUrls[aIndex]);
            iUseLuid.Copy(*iLuids[aIndex]);
        }
    }
    
    if(iUseUrl.Length() && iUseLuid.Length())
    {
        LOG.debug("\r\n-- FetchLeafObjectL -- Start ----,\r\n");
        
        LOG.debug("Url = ");
        //iLogger.Log(iUseUrl);
        //LOG.debug("\r\nLuid = ");
        //LOG.debug(iUseLuid);
        //LOG.debug("\r\n");
                
//                for(TInt a= 0; a < iGroups.Count(); a++)
        if(iGroups.Count())
        {
            if(iGroups[0])
            {
                resultsRef = statusRef = 0;
                
                for(TInt b=0; b < iGroups[0]->iChilds.Count(); b++)
                {
                    if(iGroups[0]->iChilds[b])
                    {
                        if(iGroups[0]->iChilds[b]->iFormat == MSmlDmDDFObject::ENode)
                        {
                            // node has no data,     
                        }
                        else
                        {
                            LOG.debug("\r\n--------Fetch - B ----\r\n");
                            LOG.debug("Url = ");
                            uri.Copy(iUseUrl);
                            uri.Append(iGroups[0]->iChilds[b]->iName);
                            //iLogger.Log(uri);
                            
                            iPopulateObj =     iGroups[0]->iChilds[b];
                            
                            iSmlDmAdapter->FetchLeafObjectL(uri,iUseLuid,
                                                            iGroups[0]->iChilds[b]->iMimeType,
                                                            resultsRef, statusRef);
                        
                            iPopulateObj = NULL;
                        }
                    }
                }
            }
        }
        
        LOG.debug("\r\n-- FetchLeafObjectL -- End ----,\r\n");
    }
}

void CSymbianDMRootObject::ReFetchLeafObjectL(TInt aB_Index)
{
    if(iGroups.Count())
    {
        if(iGroups[0])
        {
            TInt resultsRef(0),statusRef(0);
            
            if(aB_Index >= 0 && aB_Index < iGroups[0]->iChilds.Count())
            {
                if(iGroups[0]->iChilds[aB_Index])
                {
                    if(iGroups[0]->iChilds[aB_Index]->iFormat == MSmlDmDDFObject::ENode)
                    {
                        // node has no data,     
                    }
                    else
                    {
                        TBuf8<255> uri; 
                        uri.Copy(iUseUrl);
                        uri.Append(iGroups[0]->iChilds[aB_Index]->iName);
                
                        iPopulateObj =     iGroups[0]->iChilds[aB_Index];
                        
                        iSmlDmAdapter->FetchLeafObjectL(uri,
                               iUseLuid,iGroups[0]->iChilds[aB_Index]->iMimeType,
                               resultsRef, statusRef);
                    
                        iPopulateObj = NULL;
                    }
                }
            }
        }
    }
}

void CSymbianDMRootObject::UpdateLeafObjectL(const TDesC8& aUrl,
                                             const TDesC8& aObject,
                                             const TDesC8& aType)
{
    if(iUrls.Count() && iLuids.Count() && iSmlDmAdapter)
    {
        if(iUrls[0] && iLuids[0])
        {
            LOG.debug("\r\n-- UpdateLeafObjectL -- Start ----,\r\n");
        
            TInt statusRef(0);
            
            TBuf8<255> uri;
            uri.Copy(iUseUrl);
            uri.Append(aUrl);
            
            //iLogger.Log(uri);
                                    
            iSmlDmAdapter->StartAtomicL();
            iErrorCode = MSmlDmAdapter::EOk;
            iSmlDmAdapter->UpdateLeafObjectL(uri,iUseLuid,aObject,aType,statusRef);
        
            iSmlDmAdapter->CompleteOutstandingCmdsL();
                
            if(iErrorCode == MSmlDmAdapter::EOk)
                iSmlDmAdapter->CommitAtomicL();
            else
                iSmlDmAdapter->RollbackAtomicL();
                
            LOG.debug("\r\n-- UpdateLeafObjectL -- End ----,\r\n");
        }
    }
    else //if(iUseUrl.Length())
    {
        LOG.debug("\r\n-- UpdateLeafObjectL -- Start ----,\r\n");
    
        TInt statusRef(0);
        
        TBuf8<255> uri;
        uri.Copy(iUseUrl);
        uri.Append(aUrl);
        
        //iLogger.Log(uri);
                                
        iSmlDmAdapter->StartAtomicL();
        iErrorCode = MSmlDmAdapter::EOk;
        iSmlDmAdapter->UpdateLeafObjectL(uri,KNullDesC8,aObject,aType,statusRef);
    
        iSmlDmAdapter->CompleteOutstandingCmdsL();
            
        if(iErrorCode == MSmlDmAdapter::EOk)
            iSmlDmAdapter->CommitAtomicL();
        else
            iSmlDmAdapter->RollbackAtomicL();
            
        LOG.debug("\r\n-- UpdateLeafObjectL -- End ----,\r\n");
    }
}

MSmlDmDDFObject& CSymbianDMRootObject::AddChildObjectL(const TDesC8& aNodeName)
{
    iName.Copy(aNodeName);
    return (*this);
}

MSmlDmDDFObject& CSymbianDMRootObject::AddChildObjectGroupL()
{
    CSymbianDMObject* newChild = CSymbianDMObject::NewL(KNullDesC8);
    iGroups.Append(newChild);
    
    return (*newChild);
}

void CSymbianDMRootObject::SetAccessTypesL( TSmlDmAccessTypes aAccessTypes )
{
    iAccessTypes = aAccessTypes;
}

void CSymbianDMRootObject::SetDefaultValueL( const TDesC8& /* aDefaultValue */)
{
}

void CSymbianDMRootObject::SetDescriptionL( const TDesC8& /* aDescription */)
{
}

void CSymbianDMRootObject::SetDFFormatL( TDFFormat aFormat )
{
    iFormat = aFormat;
}

void CSymbianDMRootObject::SetOccurenceL( TOccurence aOccurence )
{
    iOccurence = aOccurence;
}

void CSymbianDMRootObject::SetScopeL( TScope aScope )
{
    iScope = aScope;
}

void CSymbianDMRootObject::SetDFTitleL( const TDesC8& aTitle )
{
    iTitle.Copy(aTitle);
}

void CSymbianDMRootObject::AddDFTypeMimeTypeL( const TDesC8& aMimeType )
{
    iMimeType.Copy(aMimeType);
}

void CSymbianDMRootObject::SetAsObjectGroup()
{
    iIsGroup = ETrue;
}

void CSymbianDMRootObject::SetResultsL(TInt aResultsRef, CBufBase& aObject,
                                       const TDesC8& /* aType */ )
{
    TBuf8<250> hjelpper;
    
    if(iNamesArray)
    {
        TInt objLength = aObject.Size();
        if(objLength > 150)
        {
            objLength = 150;
        }
        
        aObject.Read(0,hjelpper,objLength);
        
        TBuf<150> bigHelp;
        bigHelp.Copy(hjelpper);
        
        iNamesArray->AppendL(bigHelp);
    }
    else if(iPopulateObj)
    {
        iPopulateObj->SetResultsL(aObject);
    }
    else
    {
        LOG.debug("\r\n---Results--\r\n");
        
        hjelpper.Copy(_L8("aResultsRef = "));
        hjelpper.AppendNum(aResultsRef);
        hjelpper.Append(_L8("\r\nType = "));
        
        hjelpper.Zero();
        TInt objLength = aObject.Size();
        if(objLength > 150)
        {
            objLength = 150;
        }
        
        aObject.Read(0,hjelpper,objLength);
        
        iResultObj.Copy(hjelpper);
    }
}

void CSymbianDMRootObject::SetResultsL(TInt aResultsRef,
                                       RReadStream*& /*aStream*/,
                                       const TDesC8& /* aType */ )
{
    TBuf8<250> hjelpper(_L8("aResultsRef = "));
    hjelpper.AppendNum(aResultsRef);
    hjelpper.Append(_L8(", Type = "));
}

void CSymbianDMRootObject::SetStatusL( TInt aStatusRef,MSmlDmAdapter::TError aErrorCode )
{
    if(iNamesArray)
    {
        // do nothing..
    }
    else if(iPopulateObj)
    {
        iPopulateObj->SetStatus(aErrorCode);
    }
    else
    {
        iErrorCode = aErrorCode;
        
        TBuf8<100> hjelpper(_L8("aStatusRef = "));
        hjelpper.AppendNum(aStatusRef);
        hjelpper.Append(_L8(", "));
        StringBuffer status = buf8ToStringBuffer(hjelpper);
        LOG.debug("Status: %s", status.c_str());
        
        switch (aErrorCode)
        {
        case MSmlDmAdapter::EOk:/** The command was successful */
            LOG.debug("EOk");
            break;
        /** The command failed because no setting exists in the store which
         * corresponds to the URI or LUID passed to the adapter */
        case MSmlDmAdapter::ENotFound:
            LOG.debug("ENotFound");
            break;
        /** The command failed because the setting can't take the value being
         * passed to the adapter */
        case MSmlDmAdapter::EInvalidObject:
            LOG.debug("EInvalidObject");
            break;
        /** The command failed because the setting already exists in the store */
        case MSmlDmAdapter::EAlreadyExists:
            LOG.debug("EAlreadyExists");
            break;
        /** The command failed because the setting value is too large to be
         *  accommodated in the store */
        case MSmlDmAdapter::ETooLargeObject:
            LOG.debug("ETooLargeObject");
            break;
        /** The command failed because the disk on which the device store resides is full */
        case MSmlDmAdapter::EDiskFull:
            LOG.debug("EDiskFull");
            break;
        case MSmlDmAdapter::EError:/** The command failed for an unspecified reason */
            LOG.debug("EError");
            break;
        case MSmlDmAdapter::ERollbackFailed:/** The command could not be rolled back successfully */
            LOG.debug("ERollbackFailed");
            break;
        case MSmlDmAdapter::EObjectInUse:/** The command failed because the setting is being used by another client */
            LOG.debug("EObjectInUse");
            break;
        case MSmlDmAdapter::ENoMemory:/** The command failed because no memory could be allocated */
            LOG.debug("ENoMemory");
            break;
        case MSmlDmAdapter::ECommitOK:/** The command succesfully committed */
            LOG.debug("ECommitOK");
            break;
        case MSmlDmAdapter::ERollbackOK:/** The command rolled back succesfully */
            LOG.debug("ERollbackOK");
            break;    
        case MSmlDmAdapter::ECommitFailed:/** The commit of command failed */
            LOG.debug("ECommitFailed");
            break;
        };    
        
        //iLogger.Log(_L8("\r\n"));
    }
}

void CSymbianDMRootObject::SetMappingL( const TDesC8& aURI, const TDesC8& aLUID )
{
    StringBuffer uri = buf8ToStringBuffer(aURI);
    StringBuffer uid = buf8ToStringBuffer(aLUID);
    LOG.debug("uri=%s -- uid=%s", uri.c_str(), uid.c_str());
    
    HBufC8* newUrl = aURI.AllocL();
    iUrls.Append(newUrl);
    
    HBufC8* newLuid = aLUID.AllocL();
    iLuids.Append(newLuid);
}

void CSymbianDMRootObject::FetchLinkL(const TDesC8& aURI, CBufBase& aData,
                                      MSmlDmAdapter::TError& aStatus )
{
    switch (aStatus)
    {
    case MSmlDmAdapter::EOk:/** The command was successful */
        LOG.debug("EOk,");
        break;
    /** The command failed because no setting exists in the store which
     * corresponds to the URI or LUID passed to the adapter */
    case MSmlDmAdapter::ENotFound:
        LOG.debug("ENotFound,");
        break;
    /** The command failed because the setting can't take the value being passed
     * to the adapter */
    case MSmlDmAdapter::EInvalidObject:
        LOG.debug("EInvalidObject,");
        break;
    case MSmlDmAdapter::EAlreadyExists:
        LOG.debug("EAlreadyExists,");
        break;
     /** The command failed because the setting value is too large to be accommodated in the store */
    case MSmlDmAdapter::ETooLargeObject:
        LOG.debug("ETooLargeObject,");
        break;
    /** The command failed because the disk on which the device store resides is full */
    case MSmlDmAdapter::EDiskFull:
        LOG.debug("EDiskFull,");
        break;
    case MSmlDmAdapter::EError:/** The command failed for an unspecified reason */
        LOG.debug("EError,");
        break;
    case MSmlDmAdapter::ERollbackFailed:/** The command could not be rolled back successfully */
        LOG.debug("ERollbackFailed,");
        break;
    case MSmlDmAdapter::EObjectInUse:/** The command failed because the setting is being used by another client */
        LOG.debug("EObjectInUse,");
        break;
    case MSmlDmAdapter::ENoMemory:/** The command failed because no memory could be allocated */
        LOG.debug("ENoMemory,");
        break;
    case MSmlDmAdapter::ECommitOK:/** The command succesfully committed */
        LOG.debug("ECommitOK,");
        break;
    case MSmlDmAdapter::ERollbackOK:/** The command rolled back succesfully */
        LOG.debug("ERollbackOK,");
        break;    
    case MSmlDmAdapter::ECommitFailed:/** The commit of command failed */
        LOG.debug("ECommitFailed,");
        break;
    };
   
    StringBuffer uri = buf8ToStringBuffer(aURI);
    LOG.debug(" aURI = %s", uri.c_str());
    
    TBuf8<255> hjelpper;
    TInt objLength = aData.Size();
    if(objLength > 250)
    {
        objLength = 250;
    }
    
    aData.Read(0,hjelpper,objLength);
}
                         
HBufC8* CSymbianDMRootObject::GetLuidAllocL( const TDesC8& aURI )
{
    HBufC8* Ret(NULL);
   
    StringBuffer uri = buf8ToStringBuffer(aURI);
    LOG.debug("-- GetLuidAllocL -- Start %s ----", uri.c_str());
    
    TInt luidIndex(-1);
    
    for(TInt i=0; i < iUrls.Count(); i++)
    {
        if(iUrls[i])
        {
            if(iUrls[i]->Des().Compare(aURI) == 0)
            {
                luidIndex = i;
            }
        }
    }
    
    if(luidIndex >= 0 && luidIndex < iLuids.Count())
    {
        if(iLuids[luidIndex])
        {
            Ret = iLuids[luidIndex]->Des().AllocL();
        }
    }
    
    if(Ret)
    {
        LOG.debug("Luid found");
    }
    else
    {
        LOG.debug("Luid not found");
        Ret = KNullDesC8().AllocL();
    }
    
    LOG.debug("GetLuidAllocL -- End ----");    
    
    return Ret;
}


