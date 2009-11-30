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
#include "vocl/VConverter.h"
#include "vocl/VObjectFactory.h"
#include "base/util/WString.h"
#include "base/quoted-printable.h"
#include "base/globalsdef.h"
#include "base/util/StringBuffer.h"

#include <map>

USE_NAMESPACE

VConverter::UnfoldMethod VConverter::getUnfoldMethod(const WString & pid, const WString & version) {

    // Default
    UnfoldMethod method = convertToOneWhitespace;

    // Default exceptions
    if (pid == L"VCARD" && version == L"3.0") {
        method = removeOneFoldingWhitespace;
    } else if (pid == L"VCALENDAR" && version == L"2.0") {
        method = removeOneFoldingWhitespace;
    }

    return method;
}

WCHAR * VConverter::popPropertyLine(WCHAR * buffer, const UnfoldMethod & unfoldMethod) {

    int inputLen  = wcslen(buffer);
    // Reserve all the space that could be possible
    WCHAR * result = new WCHAR[inputLen+1];

    bool done = false;
    int j = 0;
    int i = 0;
    for (i=0; i<inputLen-2 && !done; i++) {
        if (buffer[i]   == '\r' &&
            buffer[i+1] == '\n') {
            // End of line, or end of property
            i+=2;
            WCHAR lookFor = 0;
            if (buffer[i] == ' ') {
                lookFor = ' ';
            } else if (buffer[i] == '\t') {
                lookFor = '\t';
            } else {
                done = true;
            }
            if (lookFor != 0) {
                int wsIndex = i;
                // Skip any additional whitespace characters matching the first
                switch (unfoldMethod) {
                    case convertToOneWhitespace:
                        wsIndex++;
                        // Break omitted
                    case removeAllFoldingWhitespace:
                        // We use that wsIndex to be one ahead for the convertToOneWhitespace
                        // method
                        while (buffer[wsIndex] && buffer[wsIndex] == lookFor) {
                            i++;
                            wsIndex++;
                        }
                        break;
                    case removeOneFoldingWhitespace:
                            i++;
                        break;
                }
            }
        }

        if (!done) {
            result[j] = buffer[i];
            j++;
        }
    }

    if (!done) {
        // Content did not end in a \r\n, assume end of file
        // We never hit the end of the line, because of the for loop condition
        // Get the last few characters
        result[j] = buffer[i];
        j++; i++;
        result[j] = buffer[i];
        j++; i++;
        // Move i one more, to the place after the '\0'.
        i++;
    }

    result[j] = 0;

    memmove(buffer, buffer+i-1, (wcslen(buffer+i-1)+1) * sizeof(*buffer));

    WCHAR * prop = new WCHAR[wcslen(result)+1];
    wcscpy(prop, result);
    delete [] result;

    return prop;
}

WCHAR * VConverter::popQPPropertyLine(WCHAR * buffer, const WString & encoding) {

    int inputLen  = wcslen(buffer);
    WCHAR * qpString = new WCHAR[inputLen];

    bool done = false;
    int j = 0;
    int i = 0;
    for (; i<inputLen-2 && !done; i++) {
        if (buffer[i]   == '=' && 
            buffer[i+1] == '\r' &&
            buffer[i+2] == '\n') {
            // End of line
            i += 2;

        } else if (buffer[i]   == '\r' &&
            buffer[i+1] == '\n') {
            // End of property

            i += 1;
            done = true;
        } else {
            qpString[j] = buffer[i];
            j++;
        }
    }

    if (!done) {
        // Content did not end in a \r\n, assume end of file
        // We never hit the end of the line, because of the for loop condition
        // Get the last few characters
        qpString[j] = buffer[i];
        j++; i++;
        qpString[j] = buffer[i];
        j++; i++;
        // Move i one more, to the place after the '\0'.
        i++;
    }

    qpString[j] = 0;

    memmove(buffer, buffer+i, (wcslen(buffer+i)+1) * sizeof(*buffer));

    char * mbEncoding = toMultibyte(encoding);
    char * mbString = toMultibyte(qpString);
    char * wcString = qp_decode(mbString);
    WCHAR * prop = toWideChar(wcString, mbEncoding);

    delete [] mbEncoding;
    delete [] qpString;
    delete [] mbString;
    delete [] wcString;

    return prop;
}

VObject* VConverter::parse(const WCHAR* buffer) {

    WString objType = extractObjectType(buffer);
    WString objVersion = extractObjectVersion(buffer);
    if(!objType)
        return NULL;

    VObject* vo = VObjectFactory::createInstance(objType.c_str(), objVersion.c_str());
    VProperty *prop;

    WCHAR* buffCopy;
    buffCopy = new WCHAR[wcslen(buffer)+1];
    wcscpy(buffCopy, buffer);

    ArrayList escapedCharMap = getEscapedCharMap(objType, objVersion);
    UnfoldMethod unfoldMethod = getUnfoldMethod(objType, objVersion);
    WCHAR * fieldSeparators = TEXT(";,");/*getFieldSeparators(objType, objVersion);*/

    while ( true ) {
        prop = readFieldHeader(buffCopy, fieldSeparators);
        if (!prop) {
            break;
        }
        if ( readFieldBody(buffCopy, prop, escapedCharMap, unfoldMethod)) {
            vo->addProperty(prop);
        }
        delete prop;
    }

    if (buffCopy)  { delete [] buffCopy;   buffCopy   = NULL; }

    return vo;
}

VProperty* VConverter::readFieldHeader(WCHAR * buffer, WCHAR * fieldSeparators) {

    // TODO: Handle escaped quotes in parameters

    WCHAR* headerIndex = NULL;
    WCHAR* quotaIndex = NULL;
    quotaIndex = wcschr(buffer, '"');
    headerIndex = wcschr(buffer, ':');


    if(!headerIndex)
        return NULL;
    bool quota = false;
    // If the header contains a quotation mark,
    // then rescan it starting directly after the _quotation mark_
    // (not after the end of the header, as in the original code)
    // to find the real end of the header.
    //
    // The reason for this code apparently is that the simple search above
    // might have found a headerIndex which points into the middle of
    // the quoted string.
    //
    // A better solution would be to always scan the header properly.
    if(quotaIndex && quotaIndex < headerIndex) {
        quota = true;
        int len = int(wcslen(buffer));
        for(int i = int(quotaIndex - buffer) + 1; i < len; i++) {
            if(buffer[i] == '"')
                quota = !quota;
            if(buffer[i] == ':' && !quota) {
                headerIndex = &buffer[i];
                break;
            }
        }
    }

    if(quota)
        return NULL;

    VProperty* prop = new VProperty(NULL);

    WCHAR* header = new WCHAR[wcslen(buffer) + 1];
    buffer[headerIndex - buffer] = '\0';
    wcscpy(header, buffer);
    // Shift the remaing string to the front of the buffer.
    // Using wcscpy() for that is incorrect because the standard
    // does not guarantee in which order bytes are moved!
    // wcscpy(buffer, ++headerIndex);
    ++headerIndex;
    memmove(buffer, headerIndex, (wcslen(headerIndex) + 1) * sizeof(*headerIndex));

    //if the header is folded (in .ics files)
    //we need to remove the folding
    WCHAR* headerFolding = NULL;
    if ((headerFolding = wcsstr(header, TEXT("\n ")))) {
        header[headerFolding - header] = '\0';
    }

    WCHAR *token;
    bool first = true;

    token = wcstok( header, fieldSeparators );
    while( token != NULL ) {
        if (first) {

            WCHAR* group = new WCHAR[wcslen(token) + 1];
            if(extractGroup(token, group))
                prop->addParameter(TEXT("GROUP"), group);
            else
                delete [] group; group= NULL;
            prop->setName(token);
            first = false;
        }
        else {
            WCHAR* paramIndex;
            paramIndex = wcschr(token, '=');

            if(paramIndex) {
                WCHAR* paramName = new WCHAR[wcslen(token) + 1];
                token[paramIndex - token] = '\0';
                wcscpy(paramName, token);
                ++paramIndex;
                memmove(token, paramIndex, (wcslen(paramIndex) + 1) * sizeof(*paramIndex));

                WCHAR* paramVal = new WCHAR[wcslen(token) + 1];
                wcscpy(paramVal, token);
                prop->addParameter(paramName, paramVal);

                delete [] paramName; paramName = NULL;
                delete [] paramVal; paramVal = NULL;
            }
            else {
                prop->addParameter(token,NULL);
            }
        }
        token = wcstok( NULL, fieldSeparators );
    }

    delete [] header; header = NULL;
    delete token; token = NULL;

    return prop;
}

bool VConverter::readFieldBody(WCHAR * buffer, VProperty* vprop, const ArrayList & escapedCharMap, const UnfoldMethod & unfoldMethod) {

    int i      = 0;
    int j      = 0;
    int len    = 0;
    int offset = 0;
    bool ret   = false;
    WCHAR* value     = NULL;
    WCHAR* c         = NULL;

    // Unfold property
    if(vprop->equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
        value = popQPPropertyLine(buffer, vprop->getParameterValue(TEXT("CHARSET")));
    } else {
        value = popPropertyLine(buffer, unfoldMethod);
    }

    len = wcslen(value);
    if (len == 0) {
        // This field is empty, we MUST consider it adding an empty value
        // so any value on client will be deleted.
        vprop->addValue(TEXT(""));
        ret = true;
    } else {

        c = value;
        WCHAR * aValue = c;
        bool escaped = false;
        for (i=0; i<len; i++) {
            if (c[i] == '\\') {
                escaped = !escaped;
            }

            // End of value
            if (c[i] == ';' && !escaped) {
                c[i] = '\0';
                vprop->addValue(unescape(aValue, escapedCharMap));
                aValue = c+i+1;
            }
        }

        vprop->addValue(unescape(aValue, escapedCharMap));

        ret = true;
    }

    delete [] value;

    return ret;
}

WString VConverter::unescape(WString incoming, const ArrayList & escaped) {

    for (int i = 0; i < escaped.size(); i++) {
        WKeyValuePair * kvp = dynamic_cast<WKeyValuePair*>(escaped.get(i));
        if (kvp != NULL) {
            incoming.replaceAll(kvp->getKey(), kvp->getValue());
        }
    }
    
    return incoming;
}


WCHAR* VConverter::extractObjectProperty(const WCHAR* buffer, const WCHAR *property,
                                           WCHAR * &buffCopy, size_t &buffCopyLen) {

    // Memory handling in extractObjectType() and
    // extractObjectVersion() was broken:
    // they allocated a buffer, then returned a pointer into
    // parts of this buffer as result. The caller cannot
    // free the result in this case. The functions were also
    // duplicating the same code.
    //
    // This partial fix reuses previously allocated
    // memory if the function is called a second time.

    size_t len = wcslen(buffer) + 1;
    if (buffCopyLen < len) {
        if (buffCopy) {
            delete [] buffCopy;
        }
        buffCopy = new WCHAR[len];
        buffCopyLen = len;
    }
    wcscpy(buffCopy, buffer);

    WCHAR seps[] = TEXT(":\n");
    WCHAR *token;

    token = wcstok( buffCopy, seps );
    while (token != NULL) {
        if(!wcscmp(token, property)) {
            token = wcstok( NULL, seps );
            WCHAR* index = wcschr(token,'\r');
            if(index)
                token[index-token] = '\0';
            return token;
        }
        token = wcstok( NULL, seps );
    }

    return NULL;
}

WString VConverter::extractObjectType(const WCHAR* buffer) {
    WCHAR* buffCopy = NULL;
    size_t buffCopyLen = 0;

    WCHAR* tmp = extractObjectProperty(buffer, TEXT("BEGIN"),
                                 buffCopy, buffCopyLen);

    WString ret = tmp;

    if (buffCopy) { delete [] buffCopy; }
    buffCopyLen = 0;

    return ret;
}


WString VConverter::extractObjectVersion(const WCHAR* buffer) {
    WCHAR* buffCopy = NULL;
    size_t buffCopyLen = 0;

    WCHAR* tmp = extractObjectProperty(buffer, TEXT("VERSION"),
                                 buffCopy, buffCopyLen);
    
    WString ret = tmp;
    if (buffCopy) { delete [] buffCopy; }
    buffCopyLen = 0;

    return ret;
}

bool VConverter::extractGroup(WCHAR* propertyName, WCHAR* propertyGroup) {

    WCHAR* groupIndex;
    groupIndex = wcschr(propertyName, '.');

    if(!groupIndex)
        return false;

    propertyName[groupIndex - propertyName] = '\0';
    wcscpy(propertyGroup, propertyName);
    wcscpy(propertyName, ++groupIndex);

    return true;
}
