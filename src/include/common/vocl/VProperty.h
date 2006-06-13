/**
 * Copyright (C) 2005-2006 Funambol
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

#ifndef INCL_VIRTUAL_PROPERTY
#define INCL_VIRTUAL_PROPERTY

#include "base/fscapi.h"
#include "base/util/WKeyValuePair.h"
#include "base/util/ArrayList.h"


// Quoted-Printable formatted lines should be max 76 chars long.
#define QP_MAX_LINE_LEN             70
#define VCARD_MAX_LINE_LEN          76


// These are special chars to escape in vCard/vCal/vTodo (version 2.1 - 3.0)
#define VCARD21_SPECIAL_CHARS       TEXT(";\\")
#define VCARD30_SPECIAL_CHARS       TEXT(";\\,")
#define RFC822_LINE_BREAK           TEXT("\r\n")


// ------------ Public functions --------------
wchar_t* escapeSpecialChars(const wchar_t* inputString, wchar_t* version);
char*    convertToQP(const char* input, int start);
bool     encodingIsNeed(const BCHAR *in);
wchar_t* folding(const wchar_t* inputString, const int maxLine);
wchar_t* unfolding(const wchar_t* inputString);



// ------------ Class VProperty ---------------

class VProperty : public ArrayElement {

private:

    wchar_t* name;
    void set(wchar_t** p, const wchar_t* v);

    ArrayList* parameters;
    ArrayList* values;

    // This is only used as a buffer for 'getValue()'
    wchar_t* valueBuf;

 public:       
	
    VProperty(wchar_t* propName , wchar_t* propValue  = NULL);
    ~VProperty();
    ArrayElement* clone();
    void setName (const wchar_t* name);
    wchar_t* getName(wchar_t* buf = NULL, int size = -1);

    void addValue(const wchar_t* value);
    bool removeValue(const int index);
    wchar_t* getValue(int index);
    int valueCount();

    // For back-compatibility (to remove)
    wchar_t* getValue(wchar_t* buf = NULL);
    void setValue (const wchar_t* value);
    wchar_t* getPropComponent(int i);

    void addParameter(const wchar_t* paramName, const wchar_t* paramValue);
    void removeParameter(wchar_t* paramName);
    bool containsParameter(wchar_t* paramName);
    // Warning: the name does not always uniquely identify
    // the parameter, some of them may occur multiple times.
    // Use getParameterValue(int index) to get the value which
    // corresponds to a specific parameter.
    wchar_t* getParameterValue(wchar_t* paramName);
    wchar_t* getParameterValue(int index);
    wchar_t* getParameter(int index);
    int parameterCount();
    bool equalsEncoding(wchar_t* encoding);
    //wchar_t* getPropComponent(int i);
    bool isType(wchar_t* type);
    wchar_t* toString(wchar_t* version = NULL);

 };

#endif
