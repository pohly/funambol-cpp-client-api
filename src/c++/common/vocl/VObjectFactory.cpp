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

#include "vocl/VObjectFactory.h"
#include "vocl/vCard/Contact.h"
#include "vocl/vCard/Contact30.h"
#include "vocl/iCalendar/Event.h"
#include "vocl/iCalendar/ToDo.h"

VObject* VObjectFactory::createInstance(wchar_t* objType, wchar_t* objVersion) {

    if(!objType)
        return new VObject();
    if (!wcscmp(objType, TEXT("VCARD")) && objVersion && !wcscmp(objVersion, TEXT("2.1"))) 
        return new Contact();
    if (!wcscmp(objType, TEXT("VCARD")) && objVersion && !wcscmp(objVersion, TEXT("3.0"))) 
        return new Contact30();
    else if(!wcscmp(objType, TEXT("VEVENT")))
        return new Event();
    else if(!wcscmp(objType, TEXT("VTODO")))
        return new ToDo();
    
   /*else if(!wcscmp(objType, TEXT("VJOURNAL"))
        return new Journal();
    else if(!wcscmp(objType, TEXT("VFREEBUSY"))
        return new Freebusy();
    else if(!wcscmp(objType, TEXT("VTIMEZONE"))
        return new Timezone();
    */
    else return new VObject();
}
