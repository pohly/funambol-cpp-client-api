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
#include "vocl/iCalendar/iCalProperty.h"

iCalProperty::iCalProperty (wchar_t* v) {
     altre         = NULL;
     cn            = NULL;
     cutype        = NULL;
     delegatedFrom = NULL;
     delegatedTo   = NULL;
     dir           = NULL;
     encoding      = NULL;
     formattype    = NULL;
     fbtype        = NULL;
     language      = NULL;
     member        = NULL;
     partstat      = NULL;
     range         = NULL;
     trigrel       = NULL;
     related       = NULL;
     role          = NULL;
     rsvp          = NULL;
     sentby        = NULL;
     tzid          = NULL;
     valuetype     = NULL;
     xParams = NULL;

     value  = (v) ? wstrdup(v) : NULL;
}

iCalProperty::~iCalProperty() {
    if (altre) {
        delete [] altre; altre = NULL;
    }
    if (cn) {
        delete [] cn; cn = NULL;
    }
    if (cutype) {
        delete [] cutype; cutype = NULL;
    }
    if (delegatedFrom) {
        delete [] delegatedFrom; delegatedFrom = NULL;
    }
    if (delegatedTo) {
        delete [] delegatedTo; delegatedTo = NULL;
    }
    if (dir) {
        delete [] dir; dir = NULL;
    }
    if (encoding) {
        delete [] encoding; encoding = NULL;
    }
    if (formattype) {
        delete [] formattype; formattype = NULL;
    }
    if (fbtype) {
        delete [] fbtype; fbtype = NULL;
    }    
    if (language) {
        delete [] language; language = NULL;
    }
    if (member) {
        delete [] member; member = NULL;
    }
    if (partstat) {
        delete [] partstat; partstat = NULL;
    }
    if (range) {
        delete [] range; range = NULL;
    }
    if (trigrel) {
        delete [] trigrel; trigrel = NULL;
    }
    if (related) {
        delete [] related; related = NULL;
    }
    if (role) {
        delete [] role; role = NULL;
    }
    if (rsvp) {
        delete [] rsvp; rsvp = NULL;
    }
    if (sentby) {
        delete [] sentby; sentby = NULL;
    }
    if (tzid) {
        delete [] tzid; tzid = NULL;
    }
    if (valuetype) {
        delete [] valuetype; valuetype = NULL;
    }
    if (xParams) {
        delete xParams; xParams = NULL;
    }
    if (value) {
        delete [] value; value = NULL;
    }
}

wchar_t* iCalProperty::getAltre (wchar_t* buf, int size) {
    if (buf == NULL) {
        return altre;
    }
    if (size >= 0) {
        wcsncpy(buf, altre, size);
    } else {
        wcscpy(buf, altre);
    }
    return buf;
}
wchar_t* iCalProperty::getCn (wchar_t* buf, int size) {
    if (buf == NULL) {
        return cn;
    }
    if (size >= 0) {
        wcsncpy(buf, cn, size);
    } else {
        wcscpy(buf, cn);
    }
    return buf;
}
wchar_t* iCalProperty::getCutype (wchar_t* buf, int size) {
    if (buf == NULL) {
        return cutype;
    }
    if (size >= 0) {
        wcsncpy(buf, cutype, size);
    } else {
        wcscpy(buf, cutype);
    }
    return buf;
}
wchar_t* iCalProperty::getDelegatedFrom (wchar_t* buf, int size) {
    if (buf == NULL) {
        return delegatedFrom;
    }
    if (size >= 0) {
        wcsncpy(buf, delegatedFrom, size);
    } else {
        wcscpy(buf, delegatedFrom);
    }
    return buf;
}
wchar_t* iCalProperty::getDelegatedTo (wchar_t* buf, int size) {
    if (buf == NULL) {
        return delegatedTo;
    }
    if (size >= 0) {
        wcsncpy(buf, delegatedTo, size);
    } else {
        wcscpy(buf, delegatedTo);
    }
    return buf;
}
wchar_t* iCalProperty::getDir (wchar_t* buf, int size) {
    if (buf == NULL) {
        return dir;
    }
    if (size >= 0) {
        wcsncpy(buf, dir, size);
    } else {
        wcscpy(buf, dir);
    }
    return buf;
}
wchar_t* iCalProperty::getEncoding (wchar_t* buf, int size) {
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
wchar_t* iCalProperty::getFormatType (wchar_t* buf, int size) {
    if (buf == NULL) {
        return formattype;
    }
    if (size >= 0) {
        wcsncpy(buf, formattype, size);
    } else {
        wcscpy(buf, formattype);
    }
    return buf;
}
wchar_t* iCalProperty::getFbType (wchar_t* buf, int size) {
    if (buf == NULL) {
        return fbtype;
    }
    if (size >= 0) {
        wcsncpy(buf, fbtype, size);
    } else {
        wcscpy(buf, fbtype);
    }
    return buf;
}
wchar_t* iCalProperty::getLanguage (wchar_t* buf, int size) {
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
wchar_t* iCalProperty::getMember (wchar_t* buf, int size) {
    if (buf == NULL) {
        return member;
    }
    if (size >= 0) {
        wcsncpy(buf, member, size);
    } else {
        wcscpy(buf, member);
    }
    return buf;
}
wchar_t* iCalProperty::getPartStat (wchar_t* buf, int size) {
    if (buf == NULL) {
        return partstat;
    }
    if (size >= 0) {
        wcsncpy(buf, partstat, size);
    } else {
        wcscpy(buf, partstat);
    }
    return buf;
}
wchar_t* iCalProperty::getRange (wchar_t* buf, int size) {
    if (buf == NULL) {
        return range;
    }
    if (size >= 0) {
        wcsncpy(buf, range, size);
    } else {
        wcscpy(buf, range);
    }
    return buf;
}
wchar_t* iCalProperty::getTrigRel (wchar_t* buf, int size) {
    if (buf == NULL) {
        return trigrel;
    }
    if (size >= 0) {
        wcsncpy(buf, trigrel, size);
    } else {
        wcscpy(buf, trigrel);
    }
    return buf;
}
wchar_t* iCalProperty::getRelated (wchar_t* buf, int size) {
    if (buf == NULL) {
        return related;
    }
    if (size >= 0) {
        wcsncpy(buf, related, size);
    } else {
        wcscpy(buf, related);
    }
    return buf;
}
wchar_t* iCalProperty::getRole (wchar_t* buf, int size) {
    if (buf == NULL) {
        return role;
    }
    if (size >= 0) {
        wcsncpy(buf, role, size);
    } else {
        wcscpy(buf, role);
    }
    return buf;
}
wchar_t* iCalProperty::getRsvp (wchar_t* buf, int size) {
    if (buf == NULL) {
        return rsvp;
    }
    if (size >= 0) {
        wcsncpy(buf, rsvp, size);
    } else {
        wcscpy(buf, rsvp);
    }
    return buf;
}
wchar_t* iCalProperty::getSentBy (wchar_t* buf, int size) {
    if (buf == NULL) {
        return sentby;
    }
    if (size >= 0) {
        wcsncpy(buf, sentby, size);
    } else {
        wcscpy(buf, sentby);
    }
    return buf;
}
wchar_t* iCalProperty::getTzID (wchar_t* buf, int size) {
    if (buf == NULL) {
        return tzid;
    }
    if (size >= 0) {
        wcsncpy(buf, tzid, size);
    } else {
        wcscpy(buf, tzid);
    }
    return buf;
}
wchar_t* iCalProperty::getValueType (wchar_t* buf, int size) {
    if (buf == NULL) {
        return valuetype;
    }
    if (size >= 0) {
        wcsncpy(buf, valuetype, size);
    } else {
        wcscpy(buf, valuetype);
    }
    return buf;
}
ArrayList* iCalProperty::getXParam () {
    return xParams;
}
wchar_t* iCalProperty::getValue (wchar_t* buf, int size) {
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

void iCalProperty::setAltre (wchar_t* e) {
    set(&altre, e);
}
void iCalProperty::setCn (wchar_t* e) {
    set(&cn, e);
}
void iCalProperty::setCutype (wchar_t* e) {
    set(&cutype, e);
}
void iCalProperty::setDelegatedFrom (wchar_t* e) {
    set(&delegatedFrom, e);
}
void iCalProperty::setDelegatedTo (wchar_t* e) {
    set(&delegatedTo, e);
}
void iCalProperty::setDir (wchar_t* e) {
    set(&dir, e);
}
void iCalProperty::setEncoding (wchar_t* e) {
    set(&encoding, e);
}
void iCalProperty::setFormatType (wchar_t* e) {
    set(&formattype, e);
}
void iCalProperty::setFbType (wchar_t* e) {
    set(&fbtype, e);
}
void iCalProperty::setLanguage (wchar_t* e) {
    set(&language, e);
}
void iCalProperty::setMember (wchar_t* e) {
    set(&member, e);
}
void iCalProperty::setPartStat (wchar_t* e) {
    set(&partstat, e);
}
void iCalProperty::setRange (wchar_t* e) {
    set(&range, e);
}
void iCalProperty::setTrigRel (wchar_t* e) {
    set(&trigrel, e);
}
void iCalProperty::setRelated (wchar_t* e) {
    set(&related, e);
}
void iCalProperty::setRole (wchar_t* e) {
    set(&role, e);
}
void iCalProperty::setRsvp (wchar_t* e) {
    set(&rsvp, e);
}
void iCalProperty::setSentBy (wchar_t* e) {
    set(&sentby, e);
}
void iCalProperty::setTzID (wchar_t* e) {
    set(&tzid, e);
}
void iCalProperty::setValueType (wchar_t* e) {
    set(&valuetype, e);
}
void iCalProperty::setXParam (ArrayList& list) {
    if (xParams) {
        xParams->clear();
    } else {
        xParams = new ArrayList();
    }

    int s = list.size();
    for (int i=0; i<s; ++i) {
        xParams->add(*list[i]);
    }
}
void iCalProperty::setValue (wchar_t* e) {
    set(&value, e);
}

void iCalProperty::set(wchar_t** property, wchar_t* v) {
    if (*property) {
        delete [] *property;
    }
    *property = (v) ? wstrdup(v) : NULL;
}

ArrayElement * iCalProperty::clone() {
    iCalProperty* ret = new iCalProperty();

    if (altre) ret->setAltre(altre);
    if (cn) ret->setCn(cn);
    if (cutype) ret->setCutype(cutype);
    if (delegatedFrom) ret->setDelegatedFrom(delegatedFrom);
    if (delegatedTo) ret->setDelegatedTo(delegatedTo);
    if (dir) ret->setDir(dir);
    if (encoding) ret->setEncoding(encoding);
    if (formattype) ret->setFormatType(formattype);
    if (fbtype) ret->setFbType(fbtype);
    if (language) ret->setLanguage(language);
    if (member) ret->setMember(member);
    if (partstat) ret->setPartStat(partstat);
    if (range) ret->setRange(range);
    if (trigrel) ret->setTrigRel(trigrel);
    if (related) ret->setRelated(related);
    if (role) ret->setRole(role);
    if (rsvp) ret->setRsvp(rsvp);
    if (sentby) ret->setSentBy(sentby);
    if (tzid) ret->setTzID(tzid);
    if (valuetype) ret->setValueType(valuetype);
    if (xParams) ret->setXParam(*xParams);
    if (value) ret->setValue(value);

    return (ArrayElement *)ret;
}
