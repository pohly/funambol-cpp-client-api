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
#include "vocl/VObject.h"
#include "string.h"

VObject::VObject() {
    productID = NULL;
    version = NULL;
    properties = new ArrayList();
}

VObject::~VObject() {
    if (productID) {
        delete [] productID; productID = NULL;
    }
    if (version) {
        delete [] version; version = NULL;
    }
    if (properties) {
        delete properties; properties = NULL;
    }
}

void VObject::set(wchar_t** p, wchar_t* v) {
    if (*p) {
        delete [] *p;
    }
    *p = (v) ? stringdup(v) : NULL;
}

void VObject::setVersion(wchar_t* ver) {
    set(&version, ver);
}

void VObject::setProdID(wchar_t* prodID) {
    set(&productID, prodID);
}

wchar_t* VObject::getVersion() {
    return version;
}

wchar_t* VObject::getProdID() {
    return productID;
}

void VObject::addProperty(VProperty* vProp) {
    properties->add((ArrayElement&) *vProp);
}

int VObject::propertiesCount() {
    return properties->size();
}

bool VObject::removeProperty(int index) {
    if(index < 0 || index >= propertiesCount())
        return false;
    properties->remove(index);
    return true;
}

void VObject::removeProperty(wchar_t* propName) {
    for (int i=0; i<properties->size(); i++) {
        VProperty *property; 
        property = (VProperty* )properties->get(i);
        if(!wcscmp(property->getName(), propName)) {
            properties->remove(i);
            break;
        }
    }
}

bool VObject::containsProperty(wchar_t* propName) {
    for (int i=0; i<properties->size(); i++) {
        VProperty *property; 
        property = (VProperty* )properties->get(i);
        if(!wcscmp(property->getName(), propName)) {
            return true;
        }
    }
    return false;
}

VProperty* VObject::getProperty(int index) {
    return (VProperty*)properties->get(index);
}

VProperty* VObject::getProperty(wchar_t* propName) {
    for (int i=0; i<properties->size(); i++) {
        
        VProperty *property; 
        property = (VProperty* )properties->get(i);
		
        if(!wcscmp(property->getName(), propName)) {
            return property;
        }
    }
    return NULL;
}

wchar_t* VObject::toString() {
    
    wchar_t* strVObject = new wchar_t[VOBJECT_BUFFER];
    wcscpy(strVObject,TEXT(""));
    wchar_t* eof = new wchar_t[EOF_LENGHT];

    if(version && !wcscmp(version,TEXT("3.0")))
        wcscpy(eof, TEXT("\n"));
    else
        wcscpy(eof, TEXT("\r\n"));

    for (int i=0; i<properties->size(); i++) {
        VProperty *property;
        property = (VProperty*)properties->get(i);
        if(property->containsParameter(TEXT("GROUP"))) {
           wcscat(strVObject,property->getParameterValue(TEXT("GROUP")));
           wcscat(strVObject,TEXT("."));
           property->removeParameter(TEXT("GROUP"));
        }
        wcscat(strVObject,property->getName());
        
        for(int k=0; k<property->parameterCount(); k++) {
            wcscat(strVObject,TEXT(";"));
			
            wchar_t* paramName = new wchar_t[wcslen(property->getParameter(k))+1];
            wcscpy(paramName, property->getParameter(k));
			
            wcscat(strVObject,paramName);
            if(property->getParameterValue(paramName)) {
                wcscat(strVObject,TEXT("="));
                wcscat(strVObject,property->getParameterValue(paramName));
            }
            delete [] paramName; paramName = NULL;
        }

        wcscat(strVObject,TEXT(":"));
        if(property->getValue()) {
            if(property->equalsEncoding(TEXT("BASE64"))) {
                wchar_t delim[] = TEXT("\r\n ");
                int fold = 76;
                int sizeOfValue = int(wcslen(property->getValue()));
                int size = sizeOfValue + (int)(sizeOfValue/fold + 2)*int(wcslen(delim));
                int index = 0;

                wchar_t* output = new wchar_t[size + 1];
                wcscpy(output, TEXT("\0"));
                wcscat(output, delim);

                while (index<sizeOfValue)
                {  
                    wcsncat(output,property->getValue()+index,fold);
                    wcscat(output,delim);
                    index+=fold;
                }
                
                wcscat(strVObject,output);
                delete [] output;
            } 
            else
                wcscat(strVObject,property->getValue());
        }
        wcscat(strVObject,eof);
        //if(property->equalsEncoding(TEXT("BASE64")) && !wcscmp(version,TEXT("2.1"))) 
        //    wcscat(strVObject, eof);
    }		
    return strVObject;
}

void VObject::insertProperty(VProperty* property) {

    if (propertiesCount() == 0 || wcscmp(getProperty(propertiesCount()-1)->getName(),TEXT("END")))
        addProperty(property);
    else {
        VProperty* lastProperty = getProperty(TEXT("END"));
        removeProperty(TEXT("END"));
        addProperty(property);
        addProperty(lastProperty);
    }
}

void VObject::addFirstProperty(VProperty* property) {
    properties->add(0,(ArrayElement&)*property);
}

void VObject::removeAllProperies(wchar_t* propName) {
    for(int i = 0, m = propertiesCount(); i < m ; i++)
        if(!wcscmp(getProperty(i)->getName(), propName)) {
            removeProperty(i);
            --i;
            --m;
        }
}


// Patrick Ohly: hack below, see header file

static int hex2int( wchar_t x )
{
    return (x >= '0' && x <= '9') ? x - '0' :
        (x >= 'A' && x <= 'F') ? x - 'A' + 10 :
        (x >= 'a' && x <= 'f') ? x - 'a' + 10 :
        0;
}

void VObject::toNativeEncoding()
{
    BOOL is_30 = !wcscmp(getVersion(), TEXT("3.0"));

    for (int index = propertiesCount() - 1; index >= 0; index--) {
        VProperty *vprop = getProperty(index);
        wchar_t *foreign = vprop->getValue();
        // the native encoding is always shorter than the foreign one
        wchar_t *native = new wchar_t[wcslen(foreign) + 1];

        if (vprop->equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
            int in = 0, out = 0;
            wchar_t curr;

            // this is a very crude quoted-printable decoder,
            // based on Wikipedia's explanation of quoted-printable
            while ((curr = foreign[in]) != 0) {
                in++;
                if (curr == '=') {
                    wchar_t values[2];
                    values[0] = foreign[in];
                    in++;
                    if (!values[0]) {
                        // incomplete?!
                        break;
                    }
                    values[1] = foreign[in];
                    in++;
                    if (values[0] == '\r' && values[1] == '\n') {
                        // soft line break, ignore it
                    } else {
                        native[out] = (hex2int(values[0]) << 4) |
                            hex2int(values[1]);
                        out++;

                        // the conversion to wchar on Windows is
                        // probably missing here
                    }
                } else {
                    native[out] = curr;
                    out++;
                }
            }
            native[out] = 0;
            out++;
        } else {
            wcscpy(native, foreign);
        }

        if (is_30) {
            wchar_t curr;
            int in = 0, out = 0;
            // line break is encoded with either one or two
            // characters on different platforms
            const int linebreaklen = wcslen(SYNC4J_LINEBREAK);

            while ((curr = native[in]) != 0) {
                in++;
                if (curr == '\\') {
                    curr = native[in];
                    in++;
                    switch (curr) {
                     case 'n':
                        // replace with line break
                        wcsncpy(native + out, SYNC4J_LINEBREAK, linebreaklen);
                        out += linebreaklen;
                        break;
                     case ',':
                     case '\\':
                        native[out] = curr;
                        out++;
                        break;
                     case 0:
                        // unexpected end of string
                        break;
                     default:
                        // just copy next character
                        native[out] = curr;
                        out++;
                        break;
                    }
                } else {
                    native[out] = curr;
                    out++;
                }
            }
            native[out] = 0;
            out++;
        }

        vprop->setValue(native);
        delete [] native;
    }
}

void VObject::fromNativeEncoding()
{
    BOOL is_30 = !wcscmp(getVersion(), TEXT("3.0"));

    for (int index = propertiesCount() - 1; index >= 0; index--) {
        VProperty *vprop = getProperty(index);

        if (vprop->equalsEncoding(TEXT("QUOTED-PRINTABLE"))) {
            // remove this, we cannot recreate it
            vprop->removeParameter(TEXT("ENCODING"));
        }

        wchar_t *native = vprop->getValue();
        // in the worst case every comma/linebreak is replaced with
        // two characters and each \n with =0D=0A
        wchar_t *foreign = new wchar_t[6 * wcslen(native) + 1];
        wchar_t curr;
        int in = 0, out = 0;
        // line break is encoded with either one or two
        // characters on different platforms
        const int linebreaklen = wcslen(SYNC4J_LINEBREAK);
        
        if (is_30) {
            // use backslash for special characters
            while ((curr = native[in]) != 0) {
                in++;
                switch (curr) {
                 case ',':
                 case '\\':
                    foreign[out] = '\\';
                    out++;
                    foreign[out] = curr;
                    out++;
                    break;
                 default:
                    if (!wcsncmp(native + in - 1,
                                 SYNC4J_LINEBREAK,
                                 linebreaklen)) {
                        // line break
                        foreign[out] = '\\';
                        out++;
                        foreign[out] = 'n';
                        out++;
                    } else {
                        foreign[out] = curr;
                        out++;
                    }
                    break;
                }
            }
            foreign[out] = 0;
            vprop->setValue(foreign);
        } else {
            if (wcsstr(native, SYNC4J_LINEBREAK)) {
                // replace linebreak with =0D=0A
                while ((curr = native[in]) != 0) {
                    if (!wcsncmp( native + in, SYNC4J_LINEBREAK, linebreaklen)) {
                        wcscpy(foreign + out, TEXT("=0D=0A"));
                        out += 6;
                        in += linebreaklen;
                    } else if(curr == '=') {
                        // escape =
                        wcsprintf(foreign + out, TEXT("=%02X"), '=');
                        out += 3;
                        in++;
                    } else {
                        foreign[out] = curr;
                        out++;
                        in++;
                    }
                }
                foreign[out] = 0;
                vprop->setValue(foreign);

                // we have used quoted-printable encoding
                vprop->addParameter(TEXT("ENCODING"), TEXT("QUOTED-PRINTABLE"));
            }
        }
        delete [] foreign;
    }
}
