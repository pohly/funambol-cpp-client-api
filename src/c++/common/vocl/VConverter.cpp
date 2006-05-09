/**
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "vocl/VConverter.h"
#include "vocl/VObjectFactory.h"
#include "base/util/WString.h"

VObject* VConverter::parse(wchar_t* buffer) {

	wchar_t *objType = extractObjectType(buffer);
	wchar_t *objVersion = extractObjectVersion(buffer);
    if(!objType)
        return NULL;        

	VObject* vo = VObjectFactory::createInstance(objType, objVersion); 	
    VProperty *prop;

    wchar_t* buffCopy = new wchar_t[wcslen(buffer) + 1];
    wcscpy(buffCopy, buffer);

    while ( true ) {
        prop = readFieldHeader(buffCopy);
        if (!prop) {
            break;
        }
        if ( readFieldBody(buffCopy, prop )) {
            vo->addProperty(prop);
		}
	}

    delete [] buffCopy; buffCopy  = NULL;

    return vo;
}

VProperty* VConverter::readFieldHeader(wchar_t* buffer) {

    wchar_t* headerIndex = NULL;
    wchar_t* quotaIndex = NULL;
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

    wchar_t* header = new wchar_t[wcslen(buffer) + 1];
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
    wchar_t* headerFolding = NULL;
    if(headerFolding = wcsstr(header, TEXT("\n "))) {
        header[headerFolding - header] = '\0';
    }

    wchar_t seps[] = TEXT(";");
    wchar_t *token;
    bool first = true;

	token = wcstok( header, seps );
	while( token != NULL ) {
        if (first) {

            wchar_t* group = new wchar_t[wcslen(token) + 1];
            if(extractGroup(token, group)) 
                prop->addParameter(TEXT("GROUP"), group);
            else
                delete [] group; group= NULL;
            prop->setName(token);            
            first = false;
        }
        else {
            wchar_t* paramIndex;
            paramIndex = wcschr(token, '=');

            if(paramIndex) {
                wchar_t* paramName = new wchar_t[wcslen(token) + 1];
                token[paramIndex - token] = '\0';
                wcscpy(paramName, token);
                ++paramIndex;
                memmove(token, paramIndex, (wcslen(paramIndex) + 1) * sizeof(*paramIndex));

                wchar_t* paramVal = new wchar_t[wcslen(token) + 1];
                wcscpy(paramVal, token);
                prop->addParameter(paramName, paramVal);
                
                delete [] paramName; paramName = NULL;
                delete [] paramVal; paramVal = NULL;
            }
            else {
                prop->addParameter(token,NULL);
            }
        }
        token = wcstok( NULL, seps );
    }

    delete [] header; header = NULL;
    delete token; token = NULL;

    return prop;
}

bool VConverter::readFieldBody(wchar_t* buffer, VProperty* vprop) {

    bool folding = false;
    wchar_t *c = buffer;
    int i = 0, j = 0;
    wchar_t* value = new wchar_t[wcslen(buffer) + 1];

    wcscpy(value, TEXT(""));
    
    if(vprop->equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
        bool afterEqual = false;
        while (c[i] != '\0') {
            if(afterEqual) {
                if(c[i] == '\n')
                    afterEqual = false;
                else if(c[i] == '\r') {
                    if(c[i++] != '\n') {
                        i++;
                        break;
                    }
                    afterEqual = false;
                }
                else {
                    value[j] = c[i];
                    j++;
                    afterEqual = false;
                }
            }
            else {
                if (c[i] == '=') {
                    afterEqual = true;
                    value[j] = c[i];
                    j++;
                    value[j] = '\0';
                }
                else if(c[i] == '\n') {
                    i++;
                    break;
                }
                else if(c[i] == '\r') {
                    if(c[i++] != '\n') {
                        i++;
                        break;
                    }
                }
                else {
                    value[j] = c[i];
                    j++;
                    value[j] = '\0';
                }
            }
            i++;
        }
    }
    else 
        while (c[i] != '\0') {
            if(folding) {
                if((c[i] == ' ') || c[i] == '\t' ) {
                    folding = false;
                }
                else
                    if((c[i] != '\r') && (c[i] != '\n'))
                        break;
	        }
            else {
                if((c[i] == '\r') || c[i] == '\n' )
                    folding = true;
                else {     
                    value[j] = c[i];
                    j++;
                    value[j] = '\0';
                }
            }
            i++;
        }
  
    
    vprop->setValue(value);
    delete [] value; value = NULL;
    // wcscpy only valid for non-overlapping buffers.
    // This one here can overlap.
    // wcscpy(buffer, c+i);
    memmove(buffer, c+i, (wcslen(c+i) + 1) * sizeof(*c));

	return true;
}

wchar_t* VConverter::extractObjectType(wchar_t* buffer) {

    wchar_t* buffCopy = new wchar_t[wcslen(buffer) + 1];
    wcscpy(buffCopy, buffer);


    wchar_t seps[] = TEXT(":\n");
    wchar_t *token;

    token = wcstok( buffCopy, seps );

    if(!wcscmp(token, TEXT("BEGIN"))) {
        
        token = wcstok( NULL, seps );
        wchar_t* index = wcschr(token,'\r');
        if(index)
            token[index-token] = '\0';
        return token;
    }

    delete [] buffCopy; buffCopy = NULL;

    return NULL;

}
wchar_t* VConverter::extractObjectVersion(wchar_t* buffer) {
    
    wchar_t* buffCopy = new wchar_t[wcslen(buffer) + 1];
    wcscpy(buffCopy, buffer);

    wchar_t seps[] = TEXT(":\n");
    wchar_t* token;
    wchar_t* index;
    
    token = wcstok( buffCopy, seps );

    while( token != NULL ) {

        if(!wcscmp(token, TEXT("VERSION"))) {
            token = wcstok( NULL, seps );
            index = wcschr(token,'\r');
            if(index)
                token[index-token] = '\0';
            return token;
        }
        token = wcstok( NULL, seps );
    }
    
    delete [] buffCopy; buffCopy = NULL;

    return NULL;
}

bool VConverter::extractGroup(wchar_t* propertyName, wchar_t* propertyGroup) {
    
    wchar_t* groupIndex;
    groupIndex = wcschr(propertyName, '.');

    if(!groupIndex)
        return false;

    propertyName[groupIndex - propertyName] = '\0';
    wcscpy(propertyGroup, propertyName);
    wcscpy(propertyName, ++groupIndex);

    return true;
}
