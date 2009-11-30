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


#ifndef INCL_VIRTUAL_PROPERTY
#define INCL_VIRTUAL_PROPERTY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/WKeyValuePair.h"
#include "base/util/ArrayList.h"

#include <map>

#define RFC822_LINE_BREAK           TEXT("\r\n")

#include "base/globalsdef.h"

BEGIN_NAMESPACE

ArrayList getEscapedCharMap(const WString & pid, const WString & version);

// ------------ Public functions --------------
char*    convertToQP(const char* input, int start, int maxLineLength = -1);
bool     encodingIsNeed(const char *in, const ArrayList & escapedCharMap = ArrayList());
WCHAR* folding(const WCHAR* inputString, const int maxLine);


// ------------ Class VProperty ---------------

class VProperty : public ArrayElement {

private:

    WCHAR* name;
    void set(WCHAR** p, const WCHAR* v);

    ArrayList* parameters;
    ArrayList* values;

    // This is only used as a buffer for 'getValue()'
    WCHAR* valueBuf;

    WString escape(WString inputString, const ArrayList & escapedCharMap);

    enum FoldMethod {
        // Do not fold - fallback option
        noFolding,
        // Fold by inserting 1 whitespace on a whitespace character only
        foldOnWhitespaceOne,
        // Fold by inserting 2 whitespace on a whitespace character only
        // This option is good for ensuring compliance with faulty
        // parsers that simply strip the first whitespace, as one remains
        foldOnWhitespaceTwo,
        // For ical
        foldAnywhere
    };

    FoldMethod getFoldMethod(const WString & pid, const WString & version);
    int getMaxLineLength(const WString & pid, const WString & version);
    bool isQPEncodingAllowed(const WString & pid, const WString & version);
    int createPropertyHeader(WString & buffer);
    WString createPropertyValueString(bool shouldFormat, int headerLength, int maxLineLength, FoldMethod foldMethod, const ArrayList  & escapedCharMap);
    void addRequiredEncoding(bool shouldFormat, bool qpAllowed, const ArrayList & escapedCharMap);

    void fold(WString & propertyString, int headerLength, int maxLineLength, const WString & lineDelimiter, const WString & foldOn);
 public:

    VProperty(const WCHAR* propName , const WCHAR* propValue  = NULL);
    ~VProperty();
    ArrayElement* clone();
    void setName (const WCHAR* name);
    WCHAR* getName(WCHAR* buf = NULL, int size = -1);

    void addValue(const WCHAR* value);
    bool removeValue(const int index);
    WCHAR* getValue(int index);
    int valueCount();

    // For back-compatibility (to remove)
    WCHAR* getValue(WCHAR* buf = NULL);
    void setValue (const WCHAR* value);
    WCHAR* getPropComponent(int i);

    void addParameter(const WCHAR* paramName, const WCHAR* paramValue);
    void removeParameter(const WCHAR* paramName);
    bool containsParameter(const WCHAR* paramName);
    // Warning: the name does not always uniquely identify
    // the parameter, some of them may occur multiple times.
    // Use getParameterValue(int index) to get the value which
    // corresponds to a specific parameter.
    WCHAR* getParameterValue(const WCHAR* paramName);
    WCHAR* getParameterValue(int index);
    WCHAR* getParameter(int index);
    int parameterCount();
    bool equalsEncoding(const WCHAR* encoding);
    //WCHAR* getPropComponent(int i);
    bool isType(const WCHAR* type);
    WCHAR* toString(WCHAR * pid, WCHAR* version, bool doFolding = true);

 };


END_NAMESPACE

/** @endcond */
#endif
