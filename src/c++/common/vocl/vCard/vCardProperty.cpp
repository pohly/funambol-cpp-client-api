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

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "vocl/vCard/vCardProperty.h"


/**
 * This object represents a property for VCard and ICalendar object
 * (i.e. its value and its parameters)
 */

/**
 * Creates an empty property
 */
vCardProperty::vCardProperty (wchar_t* v) {
    encoding      = NULL;
    language      = NULL;
    chrset        = NULL;

    value  = (v) ? wstrdup(v) : NULL;
}

vCardProperty::~vCardProperty() {
    if (chrset) {
        delete [] chrset; chrset = NULL;
    }

    if (encoding) {
        delete [] encoding; encoding = NULL;
    }

    if (language) {
        delete [] language; language = NULL;
    }
    if (value) {
        delete [] value; value = NULL;
    }
}

wchar_t* vCardProperty::getEncoding (wchar_t* buf, int size) {
    if (buf == NULL) {
        return encoding;
    }

    if (size >= 0) {
        wcsncpy(buf, encoding, size);
    } else {
        wcscpy(buf, encoding);
    }

    return buf;
}

wchar_t* vCardProperty::getLanguage (wchar_t* buf, int size) {
    if (buf == NULL) {
        return language;
    }

    if (size >= 0) {
        wcsncpy(buf, language, size);
    } else {
        wcscpy(buf, language);
    }

    return buf;
}

wchar_t* vCardProperty::getValue (wchar_t* buf, int size) {
    if (buf == NULL) {
        return value;
    }

    if (size >= 0) {
        wcsncpy(buf, value, size);
    } else {
        wcscpy(buf, value);
    }

    return buf;
}

wchar_t* vCardProperty::getCharset (wchar_t* buf, int size) {
    if (buf == NULL) {
        return chrset;
    }

    if (size >= 0) {
        wcsncpy(buf, chrset, size);
    } else {
        wcscpy(buf, chrset);
    }

    return buf;
}

void vCardProperty::setEncoding (wchar_t* e) {
    set(&encoding, e);
}

void vCardProperty::setLanguage (wchar_t* l) {
    set(&language, l);
}

void vCardProperty::setValue (wchar_t* v) {
    set(&value, v);
}

void vCardProperty::setCharset (wchar_t* c) {
    set(&chrset, c);
}


void vCardProperty::set(wchar_t** property, wchar_t* v) {
    if (*property) {
        delete [] *property;
    }
    *property = (v) ? wstrdup(v) : NULL;
}

vCardProperty* vCardProperty::clone() {
    vCardProperty* ret = new vCardProperty();

    if (chrset)          ret->setCharset      (chrset       );
    if (encoding)        ret->setEncoding     (encoding     );
    if (language)        ret->setLanguage     (language     );
    if (value)           ret->setValue        (value        );

    return ret;
}
