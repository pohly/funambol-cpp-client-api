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
#include "vocl/vCard/PersonalDetail.h"


PersonalDetail::PersonalDetail() {
    address       = NULL;
    otherAddress  = NULL;
    contactDetail = NULL;
    photo         = NULL;
    spouse        = NULL;
    children      = NULL;
    anniversary   = NULL;
    birthday      = NULL;
    gender        = NULL;
}


PersonalDetail::~PersonalDetail() {
    if (address) {
        delete address; address = NULL;
    }
    if (otherAddress) {
        delete otherAddress; otherAddress = NULL;
    }
    if (contactDetail) {
        delete contactDetail; contactDetail = NULL;
    }
    if (photo) {
        delete photo; photo = NULL;
    }
    if (spouse) {
        delete [] spouse; spouse = NULL;
    }
    if (children) {
        delete [] children; children = NULL;
    }
    if (anniversary) {
        delete [] anniversary; anniversary = NULL;
    }
    if (birthday) {
        delete [] birthday; birthday = NULL;
    }
    if (gender) {
        delete [] gender; gender = NULL;
    }
}

ContactDetail* PersonalDetail::getContactDetail() {
    return contactDetail;
}

void PersonalDetail::setContactDetail(ContactDetail& d) {
    if (contactDetail) delete contactDetail;

    contactDetail = d.clone();
}

Address* PersonalDetail::getAddress() {
    return address;
}

void PersonalDetail::setAddress(Address& a) {
    if (address) {
        delete address;
    }

    address = a.clone();
}

Address* PersonalDetail::getOtherAddress() {
    return otherAddress;
}

void PersonalDetail::setOtherAddress(Address& a) {
    if (otherAddress) delete otherAddress;

    otherAddress = a.clone();
}

wchar_t* PersonalDetail::getSpouse(wchar_t* buf, int size) {
    if (buf == NULL) {
        return spouse;
    }

    if (size >= 0) {
        wcsncpy(buf, spouse, size);
    } else {
        wcscpy(buf, spouse);
    }

    return buf;
}

wchar_t* PersonalDetail::getChildren(wchar_t* buf, int size) {
    if (buf == NULL) {
        return children;
    }

    if (size >= 0) {
        wcsncpy(buf, children, size);
    } else {
        wcscpy(buf, children);
    }

    return buf;
}

wchar_t* PersonalDetail::getAnniversary(wchar_t* buf, int size) {
    if (buf == NULL) {
        return anniversary;
    }

    if (size >= 0) {
        wcsncpy(buf, anniversary, size);
    } else {
        wcscpy(buf, anniversary);
    }

    return buf;
}

wchar_t* PersonalDetail::getBirthday(wchar_t* buf, int size) {
    if (buf == NULL) {
        return birthday;
    }

    if (size >= 0) {
        wcsncpy(buf, birthday, size);
    } else {
        wcscpy(buf, birthday);
    }

    return buf;
}

wchar_t* PersonalDetail::getGender(wchar_t* buf, int size) {
    if (buf == NULL) {
        return gender;
    }

    if (size >= 0) {
        wcsncpy(buf, gender, size);
    } else {
        wcscpy(buf, gender);
    }

    return buf;
}

vCardProperty* PersonalDetail::getPhoto() {
    return photo;
}

void PersonalDetail::setPhoto(vCardProperty& p) {
    if (photo) delete photo;

    photo = p.clone();
}

void PersonalDetail::setSpouse (wchar_t* s) {
    set(&spouse, s);
}

void PersonalDetail::setChildren (wchar_t* c) {
    set(&children, c);
}

void PersonalDetail::setAnniversary (wchar_t* a) {
    set(&anniversary, a);
}

void PersonalDetail::setBirthday (wchar_t* b) {
    set(&birthday, b);
}

void PersonalDetail::setGender (wchar_t* g) {
    set(&gender, g);
}

PersonalDetail* PersonalDetail::clone() {
    PersonalDetail* ret = new PersonalDetail();

    if (address) {
        ret->setAddress(*address);
    }
    if (otherAddress) {
        ret->setOtherAddress(*otherAddress);
    }
    if (anniversary) {
        ret->setAnniversary(anniversary);
    }
    if (birthday) {
        ret->setBirthday(birthday);
    }
    if (children) {
        ret->setChildren(children);
    }
    if (contactDetail) {
        ret->setContactDetail(*contactDetail);
    }
    if (gender) {
        ret->setGender(gender);
    }
    if (photo) {
        ret->setPhoto(*photo);
    }
    if (spouse) {
        ret->setSpouse(spouse);
    }

    return ret;
}

void PersonalDetail::set(wchar_t** p, wchar_t* v) {
    if(*p)
        delete *p;
    *p = wstrdup(v);
}
