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


#include "vocl/VObjectFactory.h"
#include "vocl/vCard/Contact.h"
#include "vocl/vCard/Contact30.h"
#include "vocl/iCalendar/Event.h"
#include "vocl/iCalendar/ToDo.h"
#include "base/globalsdef.h"

USE_NAMESPACE

VObject* VObjectFactory::createInstance(const WString & objType, const WString & objVersion) {

    if (objType == TEXT("VCARD")) {
        if (objVersion == TEXT("2.1")) {
            return new Contact();
        } else if (objVersion == TEXT("3.0")) {
            return new Contact30();
        }
    } else if(objType == TEXT("VEVENT")) {
        return new Event();
    } else if(objType == TEXT("VTODO")) {
        return new ToDo();
   /*} else if(objType == TEXT("VJOURNAL")) {
        return new Journal();
    } else if(objType == TEXT("VFREEBUSY")) {
        return new Freebusy();
    } else if(objType == TEXT("VTIMEZONE")) {
        return new Timezone();
    */
    }

    return new VObject(objType, objVersion);
}
