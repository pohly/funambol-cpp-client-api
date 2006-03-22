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

#define VPROPETY_BUFFER 500

class VProperty : public ArrayElement {

private:

    wchar_t* name;
    wchar_t* value;
    void set(wchar_t** p, const wchar_t* v);
    ArrayList* parameters;

 public:       
	
    VProperty(wchar_t* propName , wchar_t* propValue  = NULL);
    ~VProperty();
    ArrayElement* clone();
    void setName (const wchar_t* name);
    void setValue (const wchar_t* value);
    wchar_t* getName(wchar_t* buf = NULL, int size = -1);
    wchar_t* getValue(wchar_t* buf = NULL, int size = -1);
    void addParameter(const wchar_t* paramName, const wchar_t* paramValue);
    void removeParameter(wchar_t* paramName);
    bool containsParameter(wchar_t* paramName);
    wchar_t* getParameterValue(wchar_t* paramName);
    wchar_t* getParameter(int index);
    int parameterCount();
    bool equalsEncoding(wchar_t* encoding);
    wchar_t* getPropComponent(int i);
    bool isType(wchar_t* type);
    wchar_t* toString();

 };

#endif
