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


#include "base/util/utils.h"
#include "base/util/WString.h"
#include "vocl/VProperty.h"
#include "base/quoted-printable.h"
#include "base/globalsdef.h"

USE_NAMESPACE

VProperty::VProperty(const WCHAR* propname, const WCHAR* propvalue) {

    name = (propname) ? wstrdup(propname) : NULL;

    parameters = new ArrayList();
    values     = new ArrayList();
    valueBuf   = NULL;

    if (propvalue) {
        WString wsVal = propvalue;
        values->add((ArrayElement&)wsVal);
    }
}

VProperty::~VProperty() {

    if (name) {
        delete [] name; name = NULL;
    }
    if (parameters) {
        delete parameters; parameters = NULL;
    }
    if (values) {
        delete values; values = NULL;
    }
    if (valueBuf) {
        delete [] valueBuf; valueBuf = NULL;
    }
}

void VProperty::setName (const WCHAR* s) {

    set(&name, s);
}


WCHAR* VProperty::getName(WCHAR* buf, int size) {

    if (buf == NULL) {
        return name;
    }

    if (size >= 0) {
        wcsncpy(buf, name, size);
    }
    else {
        wcscpy(buf, name);
    }

    return buf;
}

void VProperty::addValue(const WCHAR* value) {

    // Empty strings are accepted
    if(value) {
        WString wsVal = value;
        values->add((ArrayElement &)wsVal);
    }
}

bool VProperty::removeValue(const int index) {

    bool ret = false;
    if (values != NULL) {
        values->removeElementAt(index);
                ret = true;
    }
    return ret;
}

// Returned value is a pointer to internal buffer,
// copy it if must have a different life cycle.
WCHAR* VProperty::getValue(int index) {

    if (valueBuf) {
        delete [] valueBuf; valueBuf = NULL;
    }

    WString* wsValue = (WString*)values->get(index);
    if (wsValue) {
        valueBuf = new WCHAR[wsValue->length() + 1];
        wcscpy(valueBuf, wsValue->c_str());
    }

    return valueBuf;
}

int VProperty::valueCount() {

    return values->size();
}


////////// Only for compatibility
WCHAR* VProperty::getValue(WCHAR* buf) {

    if (!buf) {
        return getValue(0);
    }

    WString* wsValue = (WString*)values->get(0);
    if (wsValue) {
        wcscpy(buf, wsValue->c_str());
    }
    return buf;
}


void VProperty::setValue(const WCHAR* value) {

    addValue(value);

}

WCHAR* VProperty::getPropComponent(int i) {

    return getValue(i-1);
}
//////////////////////////////////


void VProperty::addParameter (const WCHAR* paramName, const WCHAR* paramValue) {

    if(paramName) {
    WKeyValuePair *parameter = new WKeyValuePair(paramName, paramValue);
    parameters->add((ArrayElement &)*parameter);

    delete parameter; parameter = NULL;
    }
}

void VProperty::removeParameter(const WCHAR* paramName) {

    if (parameters != NULL) {
        for (int i=0; i<parameters->size(); i++){
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName)) {
                parameters->removeElementAt(i);
                break;
            }
        }
    }
}
bool VProperty::containsParameter(const WCHAR* paramName) {

    if (parameters != NULL) {
        for (int i=0; i<parameters->size(); i++){
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName)){
                return true;
            }
        }
    }

    return false;
}
WCHAR* VProperty::getParameterValue(const WCHAR* paramName) {

    if (parameters != NULL) {
        for (int i=0; i<parameters->size(); i++) {
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName))
                return ((WCHAR *)parameter->getValue());
        }
    }

    return NULL;
}
WCHAR* VProperty::getParameterValue(int index) {

    if (parameters != NULL) {
        WKeyValuePair *parameter;
        parameter = (WKeyValuePair*)parameters->get(index);
        return parameter ? (WCHAR *)parameter->getValue() : NULL;
    }

    return NULL;
}

void VProperty::set(WCHAR** p, const WCHAR* v) {

    if (*p) {
        delete [] *p;
    }
    *p = (v) ? wstrdup(v) : NULL;
}

ArrayElement* VProperty::clone() {

    if(name) {

        VProperty *cloneProperty = new VProperty(name);

        if(values != NULL) {
            for (int i=0; i<valueCount(); i++) {
                WString* valueCopy = (WString*)values->get(i)->clone();
                cloneProperty->addValue(valueCopy->c_str());
                delete valueCopy;
            }
        }

        if (parameters != NULL) {
            for (int i=0; i<parameters->size(); i++) {
                WKeyValuePair* parameterCopy;
                parameterCopy = (WKeyValuePair*)parameters->get(i)->clone();
                cloneProperty->addParameter(parameterCopy->getKey(), parameterCopy->getValue());
                delete parameterCopy;
            }
        }
        return cloneProperty;
    }
    return NULL;
}

int VProperty::parameterCount() {

    return parameters->size();
}

int VProperty::createPropertyHeader(WString & buffer) {

    if (parameterCount()>0 && containsParameter(TEXT("GROUP"))) {
        buffer.append(getParameterValue(TEXT("GROUP")));
        buffer.append(TEXT("."));
    }

    buffer.append(name);

    for (int i=0; i<parameterCount(); i++) {
        WKeyValuePair *parameter;
        parameter = (WKeyValuePair*)parameters->get(i);

        // Ignore group and content-value
        if (!parameter->getKey() ||
            (wcscmp(parameter->getKey(), TEXT("GROUP")) != 0 &&
            wcscmp(parameter->getKey(), TEXT("CONTENT-VALUE")) != 0)
            ) {

            if (parameter->getKey() && parameter->getValue()) {
                // Both key and value
                buffer.append(TEXT(";"));
                buffer.append(parameter->getKey());
                buffer.append(TEXT("="));
                buffer.append(parameter->getValue());
            } else if (parameter->getValue()) {
                // Just value
                buffer.append(TEXT(";"));
                buffer.append(parameter->getValue());
            } else if (parameter->getKey()) {
                // Just key
                buffer.append(TEXT(";"));
                buffer.append(parameter->getKey());
            }
        }
    }

    buffer.append(TEXT(":"));

    return buffer.length();
}

void VProperty::addRequiredEncoding(bool shouldFormat, bool qpAllowed, const ArrayList & escapedCharMap) {

    if (shouldFormat && !qpAllowed) {
        if(!equalsEncoding(TEXT("BASE64")) &&
           !equalsEncoding(TEXT("B")) &&
           !equalsEncoding(TEXT("b")) ) {
            for (int i=0; i<valueCount(); i++) {
                char* charValue = toMultibyte(getValue(i));
                if (encodingIsNeed(charValue, escapedCharMap)) {
                    addParameter(TEXT("ENCODING"), TEXT("b"));
                    delete [] charValue;
                    break;
                }
                delete [] charValue;
            }
        }
    } else if (qpAllowed) {
        if (!equalsEncoding(TEXT("QUOTED-PRINTABLE")) ) {
            for (int i=0; i<valueCount(); i++) {
                char* charValue = toMultibyte(getValue(i));
                if (encodingIsNeed(charValue, escapedCharMap)) {
                    addParameter(TEXT("ENCODING"), TEXT("QUOTED-PRINTABLE"));
                    addParameter(TEXT("CHARSET"), TEXT("UTF-8"));
                    delete [] charValue;
                    break;
                }
                delete [] charValue;
            }
        }
    }
}

WString VProperty::createPropertyValueString(bool shouldFormat, int headerLength, int maxLineLength, FoldMethod foldMethod, const ArrayList & escapedCharMap) {

    WString propertyString = TEXT("");

    bool isQPEncoded = equalsEncoding(TEXT("QUOTED-PRINTABLE"));
    bool isBEncoded  = equalsEncoding(TEXT("BASE64")) ||
               equalsEncoding(TEXT("B")) ||
               equalsEncoding(TEXT("b"));

    // Write values
    int numValues = valueCount();
    if(numValues > 0) {
        WString valueString = TEXT("");
        
        if (shouldFormat) {
            // Get all values in one single string
            for (int i=0; i<numValues; i++) {
                if (i>0) {
                    valueString.append(TEXT(";"));
                }
                // Escape special chars
                WString value = getValue(i);
                if (!isQPEncoded && !isBEncoded) {
                   value = escape(value, escapedCharMap);
                }
                valueString.append(value);
            }
        } else {
            WString value;
            for (int i=0; i<valueCount(); i++) {
                if (i>0) {
                    valueString.append(TEXT(";"));
                }
                value = getValue(i);         
                if (i == 0 && !value.null() && value.length() > 0 && (wcscmp(name, TEXT("PHOTO")) == 0)) {
                    valueString.append(TEXT("\r\n"));
                }   
                valueString.append(value);
            }

        }

        if (shouldFormat) {
            // QUOTED-PRINTABLE encoding (of all values)
            if (isQPEncoded) {

                char* s  = toMultibyte(valueString.c_str());
                char* qp = convertToQP(s, headerLength, maxLineLength);
                delete [] s;
                WCHAR* qpValueString = toWideChar(qp);
                delete [] qp;
                if(qpValueString) {
                    propertyString.append(qpValueString);
                    delete [] qpValueString;

                    // Disable folding
                    foldMethod = noFolding;
                } else {
                    propertyString.append(valueString);
                }

            } else if(isBEncoded) {

                char* s  = toMultibyte(valueString.c_str());
                int len = strlen(s);

                char* base64 = new char[2*len + 1];
                b64_encode(base64, s, len);
                delete [] s;

                WCHAR* b64ValueString = toWideChar(base64);
                delete [] base64;

                propertyString.append(b64ValueString);
                delete [] b64ValueString;
            } else {
                // Default encoding (7bit)
                propertyString.append(valueString);
            }
        } else { // not is to apply any transformation
            propertyString.append(valueString);
        }
    }

    if (foldMethod != noFolding) {
        WString lineDelimeter = TEXT("\r\n ");
        WString foldOn = NULL;
        if (foldMethod == foldOnWhitespaceOne || foldMethod == foldOnWhitespaceTwo) {
            // TODO can fold on tabs too, but should then be indented with tabs, not spaces
            foldOn = TEXT(" ");
        }

        if (foldMethod == foldOnWhitespaceTwo) {
            lineDelimeter.append(TEXT(" "));
        }

        fold(propertyString, headerLength, maxLineLength, lineDelimeter, foldOn);
    }


    return propertyString;
}

int VProperty::getMaxLineLength(const WString & pid, const WString & version) {
    // Default
    int maxLineLength = 76;
    if (pid == TEXT("VCALENDAR") && version == TEXT("2.0")) {
        maxLineLength = 75;
    }

    return maxLineLength;
}

bool VProperty::isQPEncodingAllowed(const WString & pid, const WString & version) {
    return !(pid == TEXT("VCARD") && version == TEXT("3.0"));
}

VProperty::FoldMethod VProperty::getFoldMethod(const WString & pid, const WString & version) {

    FoldMethod method = noFolding;

    if (pid == TEXT("VCARD")) {
        if (version == TEXT("2.1")) {
            method = foldOnWhitespaceTwo;
        } else {
            method = foldAnywhere;
        }
    } else if (pid == TEXT("VCALENDAR")) {
        if (version == TEXT("1.0")) {
            method = foldOnWhitespaceTwo;
        } else {
            method = foldAnywhere;
        }
    } else if (pid == TEXT("VNOTE")) {
        if (version == TEXT("1.1")) {
            method = foldOnWhitespaceTwo;
        }
    }

    return method;
}

/*
 * Returns a WCHAR* string of this VProperty, based on vCard-vCal specifications.
 * Here values of the property are encoded / special chars are escaped according to
 * vCard-vCal 2.1/3.0 specifications.
 * @param pid      The component name (product id)
 * @param version: The component version
 *
 * Note:
 * The returned WCHAR* is new allocated, must be freed by the caller.
 */
WCHAR* VProperty::toString(WCHAR * pid, WCHAR* version, bool doFolding) {

    WString propertyString = TEXT("");

    if (!name) {
        return wstrdup(propertyString);
    }

    bool qpAllowed = isQPEncodingAllowed(pid, version);
    int maxLineLength = getMaxLineLength(pid, version);

    ArrayList escapedCharMap = getEscapedCharMap(pid, version);

    bool shouldFormat = true;
    if (parameterCount()>0 && containsParameter(TEXT("CONTENT-VALUE"))) {
        WCHAR* parVal = getParameterValue(TEXT("CONTENT-VALUE"));
        if (parVal != NULL && wcscmp(parVal, TEXT("UNCHANGED")) == 0) {
            shouldFormat = false;
        }
    }

    addRequiredEncoding(shouldFormat, qpAllowed, escapedCharMap);

    int headerLength = createPropertyHeader(propertyString);

    FoldMethod foldMethod = noFolding;
    if (doFolding) {
        foldMethod = getFoldMethod(pid, version);
    }

    WString valueString = createPropertyValueString(shouldFormat, propertyString.length(), maxLineLength, foldMethod, escapedCharMap);

    if (valueString.null()) {
        if (qpAllowed &&
            !equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {

            // Try again with QP
            addParameter(TEXT("ENCODING"), TEXT("QUOTED-PRINTABLE"));
            addParameter(TEXT("CHARSET"), TEXT("UTF-8"));
            return toString(pid, version);
        } else {
            // Abandon folding
            return toString(pid, version, false);
        }
    } else {
        propertyString.append(valueString);
    }

    return wstrdup(propertyString.c_str());
}




WCHAR* VProperty::getParameter(int index){

    WKeyValuePair *parameter;
    parameter = (WKeyValuePair*)parameters->get(index);
    return (WCHAR *)parameter->getKey();
}

bool VProperty::equalsEncoding(const WCHAR* encoding) {

    if ((encoding != NULL) && ((containsParameter(TEXT("ENCODING")) &&
        !wcscmp(getParameterValue(TEXT("ENCODING")),encoding)) ||
        containsParameter(encoding)))
        return true;
    return false;
}



bool VProperty::isType(const WCHAR* type) {
    if(containsParameter(type))
        return true;
    if(containsParameter(TEXT("TYPE")) && getParameterValue(TEXT("TYPE"))) {

        WCHAR seps[] = TEXT(",");
        WCHAR* token;

        token = wcstok(getParameterValue(TEXT("TYPE")), seps );

        while( token != NULL )
        {
            if(!wcscmp(type, token))
                return true;
            token = wcstok( NULL, seps );
        }
    }

    if(containsParameter(TEXT("type")) && getParameterValue(TEXT("type"))) {

        WCHAR seps[] = TEXT(",");
        WCHAR* token;

        token = wcstok(getParameterValue(TEXT("type")), seps );

        while( token != NULL )
        {
            if(!wcscmp(type, token))
                return true;
            token = wcstok( NULL, seps );
        }
    }
    return false;
}


BEGIN_NAMESPACE

// ------------------ Public functions --------------------

/*
 * Convert a char* string into QUOTED-PRINTABLE format.
 * @param psz     : the input char* string
 * @param start   : the start point of the line
 * @return        : the char* string in QP format (new - must be freed by the caller!)
 *                  (NULL if conversion failed)
 */
char* convertToQP(const char* input, int start, int maxLineLength) {

    int   count   = start;
    int   maxLen  = 3*strlen(input);         // This is the max length for output string
    char *sAppend = NULL;
    char  szTemp[10];
    const char *p;

    // new - must be freed by the caller
    char* qpString = new char[maxLen + 1];
    strcpy(qpString, "");

    bool isAllowed;
    bool canFit;

    if (maxLen>0) {
        sAppend = new char[maxLen + 1];
        strncpy(sAppend, input, maxLen);
        sAppend[maxLen]=0;

        if(!sAppend)
            return NULL;

        for (p = sAppend; *p; p++) {

            // 33 = !
            // 126 = ~
            // Any printable character, except space, tab, and =
            isAllowed = (*p >= 33) && (*p <= 126) && (*p != '=') && (*p != ' ') && (*p != '\t');

            if (isAllowed) {
                // No limit, room for X, or last character
                canFit = (maxLineLength == -1) || (count < maxLineLength - 1) || ((count <= maxLineLength - 1) && *(p+1) == '\0');
            } else {
                // No limit, room for =XX, or last character
                canFit = (maxLineLength == -1) || (count < maxLineLength - 3) || ((count <= maxLineLength - 3) && *(p+1) == '\0');
            }

            if (!canFit) {
                // Line full, add a soft line break
                strcat(qpString, "=\r\n");
                count = 0;
            }
            
            if (isAllowed) {
                sprintf(szTemp, "%c", *p);
                strcat(qpString, szTemp);
                count++;
            } else {
                unsigned char ind = *p;
                sprintf(szTemp, "=%02X", ind);
                strcat(qpString, szTemp);
                count += 3;
            }
        }

        delete [] sAppend;
    }
    return qpString;
}



// Returns true if special encoding is needed for the string 'in'.
bool encodingIsNeed(const char *in, const ArrayList & escapedCharMap) {
    for(int i = 0; i < int(strlen(in)); i++) {
        // If it will be escaped, its okay
        bool escaped = false;
        for (int j = 0; j < escapedCharMap.size(); j++) {
            WKeyValuePair * kvp = dynamic_cast<WKeyValuePair*>(escapedCharMap.get(j));
            if (kvp != NULL) {
                if (wcslen(kvp->getValue()) == 1 && in[i] == kvp->getValue()[0]) {
                    escaped = true;
                    break;
                }
            }
        }
        if (!escaped && ((in[i] < 0x20) || ((unsigned char)in[i] > 0x7f))) {
            return true;
        }
    }

    return false;
}

/**
 * Get a map of escaped characters (escaped -> unescaped)
 * Order matters
 */
ArrayList getEscapedCharMap(const WString & pid, const WString & version) {

    ArrayList escaped;

    if (pid == TEXT("VCARD")) {
        escaped.add(WKeyValuePair(TEXT("\\\\"),TEXT("\\")));
        escaped.add(WKeyValuePair(TEXT("\\;"),TEXT(";")));
        if (version == TEXT("3.0")) {
            escaped.add(WKeyValuePair(TEXT("\\n"),TEXT("\n")));
            escaped.add(WKeyValuePair(TEXT("\\N"),TEXT("\n")));
        }
    } else if (pid == TEXT("VCALENDAR")) {
        escaped.add(WKeyValuePair(TEXT("\\\\"),TEXT("\\")));
        escaped.add(WKeyValuePair(TEXT("\\;"),TEXT(";")));
        if (version == TEXT("2.0")) {
            escaped.add(WKeyValuePair(TEXT("\\,"),TEXT(",")));
            escaped.add(WKeyValuePair(TEXT("\\n"),TEXT("\n")));
            escaped.add(WKeyValuePair(TEXT("\\N"),TEXT("\n")));
        }
    } else if (pid == TEXT("VNOTE")) {
        escaped.add(WKeyValuePair(TEXT("\\\\"),TEXT("\\")));
        escaped.add(WKeyValuePair(TEXT("\\;"),TEXT(";")));
    }

    return escaped;
}

void VProperty::fold(WString & propertyString, int headerLength, int maxLineLength, const WString & lineDelimeter, const WString & foldOn) {
    WString result = TEXT("");
    result.reserve(propertyString.length() * 2);

    int thisLineAllowedLength = maxLineLength - headerLength;
    int pos = 0;
    int lastPos = 0;
    bool foldNow = false;
    int length = propertyString.length();
    while (pos < length) {
        if (!foldOn.null()) {
            int newpos = propertyString.find(foldOn, pos+1);

            if (newpos == WString::npos) {
                if (length - lastPos > thisLineAllowedLength) {
                    // Unfoldable by current strategy
                    result = NULL;
                    return;
                } else {
                    newpos = length;
                    foldNow = true;
                }
            }

            if (newpos - lastPos < thisLineAllowedLength) {
                pos = newpos;
            } else {
                foldNow = true;
            }
        } else {
            foldNow = true;
            pos = min(pos + thisLineAllowedLength, length);
        }

        if (foldNow) {
            result.append(propertyString.substr(lastPos, pos - lastPos));
            if (pos != length) {
                result.append(lineDelimeter);
            }
            lastPos = pos;
            if (!foldOn.null()) {
                // Skip the fold characters
                lastPos += foldOn.length();
            }
            thisLineAllowedLength = maxLineLength;
            foldNow = false;
        }
    }

    propertyString = result;
}

/*
 * Folding of long lines. Output string is splitted into multiple
 * lines, delimited by the RFC-822 line break ("\r\n").
 * @param inputString : input WCHAR string of text
 * @param maxLine     : the length of lines in the output string
 * @return            : output WCHAR string with folded lines (new allocated)
 *
 * Note:
 *      returns new allocated WCHAR*, must be freed by the caller.
 */
WCHAR* folding(const WCHAR* inputString, const int maxLine) {

    // "\r\n" followed by a white space as line ending (RFC 2425)
    WCHAR newLine[4];
    wcscpy(newLine, RFC822_LINE_BREAK);
    wcscat(newLine, TEXT(" \0"));
    int outputLen = 0, i = 0;
    int inputLen  = wcslen(inputString);
    WCHAR* outputString;

    // No folding needed
    if (inputLen <= maxLine) {
        outputString = new WCHAR[inputLen + 1];
        wcscpy(outputString, inputString);
        goto finally;
    }

    outputLen = inputLen + (int)(inputLen/maxLine + 1)*wcslen(newLine);
    outputString = new WCHAR[outputLen + 1];
    outputString[0] = 0;

    for (i=0; i<inputLen; i += maxLine) {
        wcsncat(outputString, inputString+i, maxLine);
        wcscat(outputString, newLine);
    }
    outputString[outputLen] = 0;

finally:
    return outputString;
}


WString VProperty::escape(WString inputString, const ArrayList & escapedCharMap) {

    for (int i = 0; i < escapedCharMap.size(); i++) {
        WKeyValuePair * kvp = dynamic_cast<WKeyValuePair *>(escapedCharMap.get(i));
        if (kvp != NULL) {
            inputString.replaceAll(kvp->getValue(), kvp->getKey());
        }
    }
    
    return inputString;
}

END_NAMESPACE


