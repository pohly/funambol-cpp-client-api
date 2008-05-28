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

#ifndef __MYDMROOT_OBJECT_H__
#define __MYDMROOT_OBJECT_H__

#include <smldmadapter.h>

#include "base/globalsdef.h"
#include "spdm/SymbianDmObject.h"

BEGIN_NAMESPACE

class CSymbianDMRootObject: public CBase, public MSmlDmDDFObject, public MSmlDmCallback
{

public: // Constructors and destructor
    static CSymbianDMRootObject* NewL(const TUid& aUid);
    static CSymbianDMRootObject* NewLC(const TUid& aUid);
    ~CSymbianDMRootObject();

public:
    void ChildURIListL(const TDesC8& aUrl);
    TBool SelectAccountIndex(TInt& aIndex,const TDesC8& aNameString);
    
    void GetSAllLeafsL(TInt aIndex = 0);
    void GetSAllLeafsL(const TDesC8& aNode);
    
    void UpdateLeafObjectL(const TDesC8& aUrl,const TDesC8& aObject, const TDesC8& aType);
    void ReFetchLeafObjectL(TInt aB_Index);

    TBool AddAccountL(const TDesC8& name);
    TBool NormalAddAccountL(const TDesC8& name);
    TBool SafeAddAccountL(const TDesC8& name);
    TBool DeleteAccountL(const TDesC8& aNameString);

protected:// from MSmlDmDDFObject 
    void SetAccessTypesL( TSmlDmAccessTypes aAccessTypes );
    void SetDefaultValueL( const TDesC8& aDefaultValue);
    void SetDescriptionL( const TDesC8& aDescription);
    void SetDFFormatL( TDFFormat aFormat );
    void SetOccurenceL( TOccurence aOccurence);
    void SetScopeL( TScope aScope );
    void SetDFTitleL( const TDesC8& aTitle );
    void AddDFTypeMimeTypeL( const TDesC8& aMimeType );
    void SetAsObjectGroup();
    MSmlDmDDFObject& AddChildObjectL(const TDesC8& aNodeName);
    MSmlDmDDFObject& AddChildObjectGroupL();

protected: // from MSmlDmCallback
    void SetResultsL( TInt aResultsRef, CBufBase& aObject,const TDesC8& aType );
    void SetResultsL( TInt aResultsRef, RReadStream*& aStream,const TDesC8& aType );
    void SetStatusL( TInt aStatusRef, MSmlDmAdapter::TError aErrorCode );
    void SetMappingL( const TDesC8& aURI, const TDesC8& aLUID );
    void FetchLinkL( const TDesC8& aURI, CBufBase& aData,MSmlDmAdapter::TError& aStatus );
    HBufC8* GetLuidAllocL( const TDesC8& aURI );

private:
    TBool SelectAccountIndexL(CDesCArray* aArray,TInt& aIndex);
    void ConstructL(const TUid& aUid);
    CSymbianDMRootObject();

public:  
    TBuf8<100>  iName,iTitle,iMimeType;
    TBool         iIsGroup;
    TScope         iScope;
    TOccurence     iOccurence;
    TDFFormat     iFormat;
    TSmlDmAccessTypes iAccessTypes;
    
    RPointerArray<HBufC8>  iUrls;
    RPointerArray<HBufC8>  iLuids;
    TBuf8<255>             iResultObj;
    TBuf8<100>             iUseUrl,iUseLuid;
    
    CSmlDmAdapter*              iSmlDmAdapter;
    RPointerArray<CSymbianDMObject>  iGroups;
    MSmlDmAdapter::TError       iErrorCode;
    
    CSymbianDMObject*           iPopulateObj;
    CDesCArrayFlat*             iNamesArray;
};

END_NAMESPACE

#endif // __MYDMROOT_OBJECT_H__

// End of File

