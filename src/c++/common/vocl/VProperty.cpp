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

#include "base/util/utils.h"
#include "base/util/WString.h"
#include "vocl/VProperty.h"
#include "base/quoted-printable.h"

VProperty::VProperty(wchar_t* propname, wchar_t* propvalue) {
    
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

void VProperty::setName (const wchar_t* s) {
    
    set(&name, s);
}


wchar_t* VProperty::getName(wchar_t* buf, int size) {
    
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

void VProperty::addValue(const wchar_t* value) {
	
    // Empty strings are accepted
    if(value) {
        WString wsVal = value;
		values->add((ArrayElement &)wsVal);
    }
}

bool VProperty::removeValue(const int index) {
    
	bool ret = false;
	if (values != NULL) {
		ret = values->remove(index);
    }
	return ret;
}

// Returned value is a pointer to internal buffer,
// copy it if must have a different life cycle.
wchar_t* VProperty::getValue(int index) {
    
    if (valueBuf) {
        delete [] valueBuf; valueBuf = NULL;
    }

    WString* wsValue = (WString*)values->get(index);
    if (wsValue) {
        valueBuf = new wchar_t[wsValue->length() + 1];
        wcscpy(valueBuf, wsValue->c_str());
    }

    return valueBuf;
}

int VProperty::valueCount() {

    return values->size();
}


////////// Only for compatibility
wchar_t* VProperty::getValue(wchar_t* buf) {

    if (!buf) {
        return getValue(0);
    }

    WString* wsValue = (WString*)values->get(0);
    if (wsValue) {
        wcscpy(buf, wsValue->c_str());
    }  
    return buf;
}


void VProperty::setValue(const wchar_t* value) {

    addValue(value);

}

wchar_t* VProperty::getPropComponent(int i) {
    
    return getValue(i-1);
}
//////////////////////////////////


void VProperty::addParameter (const wchar_t* paramName, const wchar_t* paramValue) {
	
    if(paramName) {
    WKeyValuePair *parameter = new WKeyValuePair(paramName, paramValue);
    parameters->add((ArrayElement &)*parameter);

    delete parameter; parameter = NULL;
    }
}

void VProperty::removeParameter(wchar_t* paramName) {
    
    if (parameters != NULL) {
        for (int i=0; i<parameters->size(); i++){
            WKeyValuePair *parameter; 
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName)) {
                parameters->remove(i);
                break;
            }
        }
    }
}
bool VProperty::containsParameter(wchar_t* paramName) {
    
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
wchar_t* VProperty::getParameterValue(wchar_t* paramName) {
    
    if (parameters != NULL) {
	    for (int i=0; i<parameters->size(); i++) {
            WKeyValuePair *parameter; 
            parameter = (WKeyValuePair* )parameters->get(i);
            if(!wcscmp(parameter->getKey(), paramName))
                return (parameter->getValue());
        }
    }

    return NULL;
}
wchar_t* VProperty::getParameterValue(int index) {
    
    if (parameters != NULL) {
        WKeyValuePair *parameter;
        parameter = (WKeyValuePair*)parameters->get(index);
        return parameter ? parameter->getValue() : NULL;
    }

    return NULL;
}

void VProperty::set(wchar_t** p, const wchar_t* v) {
	
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
			}
		}

        if (parameters != NULL) {
            for (int i=0; i<parameters->size(); i++) {
                WKeyValuePair* parameterCopy;
                parameterCopy = (WKeyValuePair*)parameters->get(i)->clone();
                cloneProperty->addParameter(parameterCopy->getKey(), parameterCopy->getValue());
            }
        }
        return cloneProperty;
    }
    return NULL;
}

int VProperty::parameterCount() {

    return parameters->size();
}


/*
 * Returns a wchar_t* string of this VProperty, based on vCard-vCal specifications.
 * Here values of the property are encoded / proper chars are escaped.
 * Note:
 * The returned wchar_t* is new allocated, must be freed by the caller.
 */
wchar_t* VProperty::toString() {

    WString propertyString = TEXT("");

    if (!name){
        goto finally;
    }


    // Set QP parameter if necessary
    if (!equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
        for (int i=0; i<valueCount(); i++) {
            char* charValue = toMultibyte(getValue(i));
            if (qp_isNeed(charValue)) {
                addParameter(TEXT("ENCODING"), TEXT("QUOTED-PRINTABLE"));
		        addParameter(TEXT("CHARSET"), TEXT("UTF-8"));
                delete [] charValue;
                break;
            }
            delete [] charValue;
        }
    }

    //
    // Write Group:
    //
    if (parameterCount()>0 && containsParameter(TEXT("GROUP"))) {
        propertyString.append(getParameterValue(TEXT("GROUP")));
        propertyString.append(TEXT("."));
    }

    //
    // Write name:
    //
    propertyString.append(name);

    //
	// Write parameters:
    //
    if(parameterCount()>0) {
        for (int i=0; i<parameterCount(); i++) {
            WKeyValuePair *parameter;
            parameter = (WKeyValuePair*)parameters->get(i);
            if (parameter->getKey()) {
                if (wcscmp(parameter->getKey(), TEXT("GROUP"))) {
                    continue;
                }
                propertyString.append(TEXT(";"));
                propertyString.append(parameter->getKey());
		    }
            if (parameter->getValue()) {
                propertyString.append(TEXT("="));
                propertyString.append(parameter->getValue());
            }
        }	
    }

    //
    // Write values:
    //
    propertyString.append(TEXT(":"));
    if(valueCount()>0) {
        WString valueString = TEXT("");

        // Base64 encoding: supposed only 1 value                   (?)
        //                  supposed value already in base64 format (?)
        // ******** TO DEVELOP **********
        /*if(property->equalsEncoding(TEXT("BASE64"))) {
            if (values->get(0)) {
                valueString.append(values->get(0));
            }
        }*/

        wchar_t *value, *valueConv;
        for (int i=0; i<valueCount(); i++) {
            if (i>0) {
                valueString.append(TEXT(";"));
            }
            value = getValue(i);
            valueConv = escapeSpecialChars(value, VCARD_SPECIAL_CHARS);         // Escape special chars (";"  "\")
            valueString.append(valueConv);
            delete [] valueConv;
	    }


        // convert values string to QUOTED-PRINTABLE
        if (equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
            char* s  = toMultibyte(valueString.c_str());
            char* qp = convertToQP(s, 0);
            wchar_t* qpValueString = toWideChar(qp);
            if(qpValueString)
			    propertyString.append(qpValueString);
            else
                propertyString.append(valueString);

		    delete [] qpValueString;
            delete [] s; 
            delete [] qp;
        }

        else {
            propertyString.append(valueString);
        }
    }		


finally:
    // memory must be free by caller with delete []
    wchar_t *str = wstrdup(propertyString);
    return str;
}




wchar_t* VProperty::getParameter(int index){

    WKeyValuePair *parameter;
    parameter = (WKeyValuePair*)parameters->get(index);
    return parameter->getKey();
}

bool VProperty::equalsEncoding(wchar_t* encoding) {

    if ((encoding != NULL) && ((containsParameter(TEXT("ENCODING")) && 
		!wcscmp(getParameterValue(TEXT("ENCODING")),encoding)) || 
        containsParameter(encoding)))
        return true;
    return false;
}



bool VProperty::isType(wchar_t* type) {
    if(containsParameter(type))
        return true;
    if(containsParameter(TEXT("TYPE")) && getParameterValue(TEXT("TYPE"))) {

        wchar_t seps[] = TEXT(",");
        wchar_t* token;

        token = wcstok(getParameterValue(TEXT("TYPE")), seps );

        while( token != NULL )
        {
            if(!wcscmp(type, token))
                return true;
            token = wcstok( NULL, seps );
        }
    }

    if(containsParameter(TEXT("type")) && getParameterValue(TEXT("type"))) {

        wchar_t seps[] = TEXT(",");
        wchar_t* token;

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




// ------------------ Public functions --------------------

/*
 * Convert a char* string into QUOTED-PRINTABLE format.
 * @param psz     : the input char* string
 * @param start   : the start point of the line
 * @return        : the char* string in QP format (new - must be freed by the caller!)
 *                  (NULL if conversion failed)
 */
char* convertToQP(const char* input, int start) {
    
    int   count   = start;
	int   maxLen  = 3*strlen(input);         // This is the max length for output string
	char *sAppend = NULL;
    char  szTemp[10];
    const char *p;

    // new - must be freed by the caller
    char* qpString = new char[maxLen + 1];
	strcpy(qpString, "");

	if (maxLen>0) {
		sAppend = new char[maxLen + 1];
		strncpy(sAppend, input, maxLen);
		sAppend[maxLen]=0;

		if(!sAppend)
			return NULL;
		
		for (p = sAppend; *p; p++) { 
			if (count > QP_MAX_LINE_LEN) {
				strcat(qpString, "=\r\n");
				count = 0;
			}
			else if (*p == '\t' || *p == ' ') {							
				const char *pScan = p;
				while (*pScan && (*pScan == '\t' || *pScan == ' ')) {
					pScan++;
				}
				if (*pScan == '\0') {
					while (*p) {
						unsigned char ind = *p;
						sprintf(szTemp, "=%02X", ind);
						strcat(qpString, szTemp);
						count += 3;
						p++;

						if (count > QP_MAX_LINE_LEN) {
							strcat(qpString, "=\r\n");
							count = 0;
						}
					}
					break;
				}
				else {
					sprintf(szTemp, "%c", *p);
					strcat(qpString, szTemp);
					count++;
				}
				continue;
			}
			else if (('!' <= *p) && ('~' >= *p) && ('=' != *p)) {
				sprintf(szTemp, "%c", *p);
				strcat(qpString, szTemp);
				count++;
			}
			else {
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




/*
* Escape special characters adding a back-slash (i.e. ";" -> "\;")
* @param inputString   : the input string to parse
* @param charsToEscape : string with special characters to escape (i.e. ";<>\\")
* @return              : the new allocated string with escaped chars
* Note: 
*      returns new allocated wchar_t*, must be freed by the caller.
*/
wchar_t* escapeSpecialChars(const wchar_t* inputString, wchar_t* charsToEscape) {

    int i, j, inputLen, outputLen;
    inputLen  = wcslen(inputString);
    
    wchar_t* wc = new wchar_t[2];

    // First find the length of output value
    outputLen = inputLen;
    for (i=0; i<inputLen; i++) {
        wcsncpy(wc, &inputString[i], 1);
        wc[1]=0;
        if (wcsstr(charsToEscape, wc))
            outputLen ++;
    }
    wchar_t* outputString = new wchar_t[outputLen+1];


    // Now escape special characters (add back-slash)
    j=0;
    for (i=0; i<inputLen; i++) {
        wcsncpy(wc, &inputString[i], 1);
        wc[1]=0;
        if (wcsstr(charsToEscape, wc)) {
            outputString[j]   = '\\';
            j++;
        }
        outputString[j] = inputString[i];
        j++;
    }
    
    outputString[outputLen] = 0;
    delete [] wc;
    return outputString;
}

