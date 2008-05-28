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


#ifndef __MYDM_OBJECT_H__
#define __MYDM_OBJECT_H__

#include <smldmadapter.h>

#include "base/globalsdef.h"
#include "base/Log.h"

BEGIN_NAMESPACE

// CLASS DECLARATION
/**
* CHelloWorldBasicAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CSymbianDMObject: public CBase, public MSmlDmDDFObject 
{
public: // Constructors and destructor
    static CSymbianDMObject* NewL(const TDesC8& aNodeName);
    static CSymbianDMObject* NewLC(const TDesC8& aNodeName);
    ~CSymbianDMObject();
public:
    void SetStatus(MSmlDmAdapter::TError aErrorCode);
    void SetResultsL(CBufBase& aObject);
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
private:
    void ConstructL(const TDesC8& aNodeName);
    CSymbianDMObject();
public:  
    TBuf8<100>  iName,iTitle,iMimeType;
    TBool         iIsGroup;
    TScope         iScope;
    TOccurence     iOccurence;
    TDFFormat     iFormat;
    HBufC8*        iDescription;
    HBufC8*        iDefaultValue;
    TSmlDmAccessTypes iAccessTypes;
    
    HBufC8*                    iResult;
    MSmlDmAdapter::TError     iErrorCode;
    
    RPointerArray<CSymbianDMObject> iChilds;
    RPointerArray<CSymbianDMObject> iGroups;
};

END_NAMESPACE

#endif // __MYDM_OBJECT_H__

