/**
 * Copyright (C) 2003-2007 Funambol
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
#include "vocl/vCard/Email.h"


Email::Email() : TypedProperty() {
}

Email::~Email() {
}

vCardProperty* Email::getEmailAddress() {
    return getProperty();
}

void Email::setEmailAddress(vCardProperty& p) {
    setProperty(p);
}

ArrayElement * Email::clone() {
    Email* ret = new Email();

    if (p) {
        ret->setProperty(*p);
    }

    if (t) {
        ret->setType(t);
    }

    return (ArrayElement *)ret;
}
