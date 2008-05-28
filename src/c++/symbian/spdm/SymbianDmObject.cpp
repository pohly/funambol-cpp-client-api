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

#include <coemain.h>
#include <aknutils.h>

#include "base/globalsdef.h"
#include "spdm/SymbianDmObject.h"

USE_NAMESPACE

CSymbianDMObject* CSymbianDMObject::NewL(const TDesC8& aNodeName)
{
    CSymbianDMObject* self = CSymbianDMObject::NewLC(aNodeName);
    CleanupStack::Pop( self );
    return self;
}

CSymbianDMObject* CSymbianDMObject::NewLC(const TDesC8& aNodeName)
{
    CSymbianDMObject* self = new ( ELeave ) CSymbianDMObject();
    CleanupStack::PushL( self );
    self->ConstructL(aNodeName);
    return self;
}

void CSymbianDMObject::ConstructL(const TDesC8& aNodeName)
{
    iName.Copy(aNodeName);
}

CSymbianDMObject::CSymbianDMObject()
{
}


CSymbianDMObject::~CSymbianDMObject()
{
    iGroups.ResetAndDestroy();
    iChilds.ResetAndDestroy();
    delete iDefaultValue;
    iDefaultValue = NULL;
    delete iDescription;
    iDescription = NULL;
    delete iResult;
    iResult = NULL;
}

void CSymbianDMObject::SetStatus(MSmlDmAdapter::TError aErrorCode)
{
    iErrorCode = aErrorCode;
}

void CSymbianDMObject::SetResultsL(CBufBase& aObject)
{
    delete iResult;
    iResult = NULL;

    TInt objLength = aObject.Size();
    if(objLength > 0)
    {
        iResult = HBufC8::NewL(objLength);
        TPtr8 hjelpPoint(iResult->Des());
        
        aObject.Read(0,hjelpPoint,objLength);
    }
}

MSmlDmDDFObject& CSymbianDMObject::AddChildObjectL(const TDesC8& aNodeName)
{
    TBuf8<255> Hjelpper;
    Hjelpper.Copy(iName);
    Hjelpper.Append(_L("/"));
    Hjelpper.Append(aNodeName);
    
    CSymbianDMObject* newChild = CSymbianDMObject::NewL(Hjelpper);
    iChilds.Append(newChild);

    return (*newChild);
}

MSmlDmDDFObject& CSymbianDMObject::AddChildObjectGroupL()
{
    CSymbianDMObject* newChild = CSymbianDMObject::NewL(iName);
    iGroups.Append(newChild);
    
    return (*newChild);
}

void CSymbianDMObject::SetAccessTypesL( TSmlDmAccessTypes aAccessTypes )
{
    iAccessTypes = aAccessTypes;
}

void CSymbianDMObject::SetDefaultValueL( const TDesC8& aDefaultValue)
{
    delete iDefaultValue;
    iDefaultValue = NULL;
    iDefaultValue = aDefaultValue.AllocL();
}

void CSymbianDMObject::SetDescriptionL( const TDesC8& aDescription)
{
    
    delete iDescription;
    iDescription = NULL;
    iDescription = aDescription.AllocL();
}

void CSymbianDMObject::SetDFFormatL( TDFFormat aFormat )
{
    iFormat = aFormat;
}

void CSymbianDMObject::SetOccurenceL( TOccurence aOccurence )
{
    iOccurence = aOccurence;
}

void CSymbianDMObject::SetScopeL( TScope aScope )
{
    iScope = aScope;
}

void CSymbianDMObject::SetDFTitleL( const TDesC8& aTitle )
{
    iTitle.Copy(aTitle);
}

void CSymbianDMObject::AddDFTypeMimeTypeL( const TDesC8& aMimeType )
{
    iMimeType.Copy(aMimeType);
}

void CSymbianDMObject::SetAsObjectGroup()
{
    iIsGroup = ETrue;
}
    
