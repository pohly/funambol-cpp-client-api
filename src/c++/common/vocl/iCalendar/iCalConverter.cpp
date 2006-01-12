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

#include "vocl/iCalendar/iCalConverter.h"

iCalConverter::iCalConverter() {
    iCalendar = NULL;
    calendar = NULL;
}	

iCalConverter::~iCalConverter() {
    if (iCalendar) {
        delete [] iCalendar; iCalendar = NULL;
    }
    if (calendar) {
        delete calendar; calendar = NULL;
    }
}

void iCalConverter::setSource(wchar_t* inputICalendar) {
    if(iCalendar) {
        delete[] iCalendar;
        iCalendar = NULL;
    }
    if(inputICalendar) {
        iCalendar = new wchar_t[wcslen(inputICalendar) + 1];
        wcscpy(iCalendar, inputICalendar);
    }
    if(calendar) {
        delete calendar; calendar = NULL;
    }
}

void iCalConverter::setSource(Calendar& inputCalendar) {
    if(calendar)        
        delete calendar;
    calendar = (Calendar *)inputCalendar.clone();
    if(iCalendar) {
        delete [] iCalendar; iCalendar = NULL;
    }
}

void iCalConverter::getICalendar(wchar_t* ouputICalendar) {
    if(iCalendar) {
        if(!ouputICalendar)
            return;
        wcscpy(ouputICalendar, iCalendar);
    }
    else
        ouputICalendar = NULL;
}

void iCalConverter::getCalendar(Calendar** outputCalendar) {
    if(calendar) {
        *outputCalendar = (Calendar *)calendar->clone();
    }
    else
        *outputCalendar = NULL;
}

bool iCalConverter::convert(wchar_t* errorDescription, long* errorCode) {
    if(!errorDescription)
        return NULL;
    wcscpy(errorDescription,TEXT(""));
    *errorCode = ERROR_SUCCESS;

    if(!calendar && !iCalendar)
        return false;
    if(calendar && iCalendar)
        return true;
    if(calendar) {

        iCalendar = calendar->toString();

        if(!calendar->getProdID() || !calendar->getProdID()->getValue()) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("'PRODID' property is missing"));
            delete [] iCalendar; iCalendar = NULL;
            return false;
        }
        
        if(!calendar->getVersion() || !calendar->getVersion()->getValue()) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("'VERSION' property is missing"));
            delete [] iCalendar; iCalendar = NULL;
            return false;
        }
        if(calendar->getEvents())
            for(int i=0; i<calendar->getEvents()->size(); i++) 
                if(!validateEvent(((Event*)calendar->getEvents()->get(i)), errorDescription, errorCode)) {
                    delete [] iCalendar; iCalendar = NULL;
                    return false;
            }
        if(calendar->getToDos())
            for(int i=0; i<calendar->getToDos()->size(); i++) 
                if(!validateTodo(((ToDo*)calendar->getToDos()->get(i)), errorDescription, errorCode)) {
                    delete [] iCalendar; iCalendar = NULL;
                    return false;
                }
  
        return true;
   }
    if (iCalendar) {
        
        calendar = new Calendar();
        VObject* vo = VConverter::parse(iCalendar);
        if(!vo) {
            *errorCode = ERROR_PARSING_ERROR;
            wcscpy(errorDescription, TEXT("Invalid VObject returned"));
            return false;
        }

        int n = vo->propertiesCount();

        if(wcscmp(vo->getProperty(0)->getName(), TEXT("BEGIN")) || 
            !vo->getProperty(0)->getValue() ||
            wcscmp(vo->getProperty(0)->getValue(), TEXT("VCALENDAR"))) {
                *errorCode = ERROR_KEY_PROPERTY_MISSING;
                wcscpy(errorDescription, TEXT("'BEGIN:VCALENDAR' property is missing"));
                return false;
            }

        if(wcscmp(vo->getProperty(n-1)->getName(), TEXT("END")) || 
            !vo->getProperty(n-1)->getValue() ||
            wcscmp(vo->getProperty(n-1)->getValue(), TEXT("VCALENDAR"))) {
                *errorCode = ERROR_KEY_PROPERTY_MISSING;
                wcscpy(errorDescription, TEXT("'END:VCALENDAR' property is missing"));
                return false;
            }
        
        if(!vo->containsProperty(TEXT("VERSION"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("'VERSION' property is missing"));
            return false;
        }

        if(vo->containsProperty(TEXT("VERSION")) && 
            (!vo->getProperty(TEXT("VERSION")) || wcscmp(vo->getProperty(TEXT("VERSION"))->getValue(), TEXT("2.0")))) {
                *errorCode = ERROR_ILLEGAL_VERSION_NUMBER;
                if(vo->getProperty(TEXT("VERSION")))
                    swprintf(errorDescription, TEXT("Illegal version number : %s"), vo->getProperty(TEXT("VERSION"))->getValue());
                else
                    swprintf(errorDescription, TEXT("Illegal version number"));
                return false; 
            }
        else {
            iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("VERSION"))->getValue());
            calendar->setVersion(*prop);
            vo->removeProperty(TEXT("VERSION"));
            delete prop;
        }

        if(!vo->containsProperty(TEXT("PRODID")) || !vo->getProperty(TEXT("PRODID"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("'PRODID' property is missing"));
            return false;
        }
        else {
            iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("PRODID"))->getValue());
            calendar->setProdID(*prop);
            vo->removeProperty(TEXT("PRODID"));
            delete prop;
        }

        if(vo->containsProperty(TEXT("CALSCALE")) || 
            vo->getProperty(TEXT("CALSCALE"))) {
                iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("CALSCALE"))->getValue());
                calendar->setCalScale(*prop);
                vo->removeProperty(TEXT("CALSCALE"));
                delete prop;
            }
        
        if(vo->containsProperty(TEXT("METHOD")) || 
            vo->getProperty(TEXT("METHOD"))) {
                iCalProperty* prop = new iCalProperty(vo->getProperty(TEXT("METHOD"))->getValue());
                calendar->setMethod(*prop);
                vo->removeProperty(TEXT("METHOD"));
                delete prop;
            }

        //extract VEVENTs from vo
        Event* ev;
        while(ev = extractEvent(vo, errorDescription, errorCode)) {
            if (!validateEvent(ev, errorDescription, errorCode)) {
                delete ev; ev = NULL;
                return false;
            }
            calendar->addEvent(ev);
            delete ev; ev = NULL;
        }
        
        //extract VTODOs from vo
        ToDo* task;
        while(task = extractTask(vo, errorDescription, errorCode)) {
            if (!validateTodo(task, errorDescription, errorCode)) {
                delete task; task = NULL;
                return false;
            }
            calendar->addToDo(task);
            delete task; task = NULL;
        }
    } 

    return true;
}

Event* iCalConverter::extractEvent(VObject* vo, wchar_t* errorDescription, long* errorCode) {
    int i,m;
	int beginEvent = -1;
    int endEvent = -1;
    for(i = 0, m = vo->propertiesCount(); i < m ; i++) {
        if(beginEvent == -1 && !wcscmp(vo->getProperty(i)->getName(), TEXT("BEGIN")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(), TEXT("VEVENT")))
            beginEvent = i;    
        if(endEvent == -1 && !wcscmp(vo->getProperty(i)->getName(),TEXT("END")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(),TEXT("VEVENT"))) {
                endEvent = i;
                break;
            }
    }

    if(beginEvent == -1)
        return NULL;

    if(beginEvent > endEvent) {
        *errorCode = ERROR_INVALID_EVENT_BLOCK;
        wcscpy(errorDescription, TEXT("BEGIN:VEVENT property found, but END:VEVENT is missing"));
        return NULL;
    }
    
    Event* ret = new Event();
    for(i = beginEvent; i <= endEvent; i++) {
        ret->addProperty(vo->getProperty(i));
        vo->removeProperty(i);
        --i;
        --endEvent;
    }
    
    extractAlarm((VObject*) ret);

    return ret;
}

ToDo* iCalConverter::extractTask(VObject* vo, wchar_t* errorDescription, long* errorCode) {
    int i,m;
	int beginTask = -1;
    int endTask = -1;
    for(i = 0, m = vo->propertiesCount(); i < m ; i++) {
        if(beginTask == -1 && !wcscmp(vo->getProperty(i)->getName(), TEXT("BEGIN")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(), TEXT("VTODO")))
            beginTask = i;    
        if(endTask == -1 && !wcscmp(vo->getProperty(i)->getName(),TEXT("END")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(),TEXT("VTODO"))) {
                endTask = i;
                break;
            }
    }

    if(beginTask == -1)
        return NULL;

    if(beginTask > endTask) {
        *errorCode = ERROR_INVALID_TODO_BLOCK;
        wcscpy(errorDescription, TEXT("BEGIN:VTODO property found, but END:VTODO is missing"));
        return NULL;
    }
    
    ToDo* ret = new ToDo();
    for(i = beginTask; i <= endTask; i++) {
        ret->addProperty(vo->getProperty(i));
        vo->removeProperty(i);
        --i;
        --endTask;
    }

    extractAlarm((VObject*) ret);

    return ret;
}

void iCalConverter::extractAlarm(VObject* vo){
    int beginAlarm = -1;
    int endAlarm = -1;
    for(int i = 0, m = vo->propertiesCount(); i < m ; i++) {
        if(beginAlarm== -1 && !wcscmp(vo->getProperty(i)->getName(), TEXT("BEGIN")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(), TEXT("VALARM")))
            beginAlarm = i;    
        if(endAlarm == -1 && !wcscmp(vo->getProperty(i)->getName(),TEXT("END")) &&
            vo->getProperty(i)->getValue() &&
            !wcscmp(vo->getProperty(i)->getValue(),TEXT("VALARM"))) {
                endAlarm = i;
                break;
            }
    }

    if(beginAlarm != -1 && endAlarm > beginAlarm)
        for(int i = beginAlarm; i <= endAlarm; i++) {
            vo->removeProperty(i);
            --i;
            --endAlarm;
        }
} 

bool iCalConverter::validateEvent(Event* ev, wchar_t* errorDescription, long* errorCode) {
  //validate BEGIN, END, UID 
    if(wcscmp(ev->getProperty(0)->getName(), TEXT("BEGIN")) ||
        wcscmp(ev->getProperty(0)->getValue(), TEXT("VEVENT"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("Invalid EVENT: 'BEGIN' property is missing"));
            return false;
        }
    if(wcscmp(ev->getProperty(ev->propertiesCount()-1)->getName(), TEXT("END")) ||
        wcscmp(ev->getProperty(ev->propertiesCount()-1)->getValue(), TEXT("VEVENT"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("Invalid EVENT: 'END' property is missing"));
            return false;
        }
    if(!ev->containsProperty(TEXT("UID"))) { 
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        wcscpy(errorDescription, TEXT("Invalid EVENT: 'UID' property is missing"));
        return false;
    } 
    for(int i = 0; i < ev->propertiesCount(); i++) {                
        if(!wcsstr(EVENT_PROPERTIES_LIST, ev->getProperty(i)->getName()) &&
            wcsstr(ev->getProperty(i)->getName(),TEXT("X-")) != ev->getProperty(i)->getName()) {
                *errorCode = ERROR_ILLEGAL_PROPERTY_NAME;
                swprintf(errorDescription, TEXT("EVENT - Illegal property name : %s"), ev->getProperty(i)->getName());
                return false;
            }
        if(ev->getProperty(i)->getValue() && !validatePropery(ev->getProperty(i), errorDescription, errorCode))
            return false;
    }
    return true;
}
bool iCalConverter::validateTodo(ToDo* task, wchar_t* errorDescription, long* errorCode) {
    //validate BEGIN, END, UID 
    if(wcscmp(task->getProperty(0)->getName(), TEXT("BEGIN")) ||
        wcscmp(task->getProperty(0)->getValue(), TEXT("VTODO"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("Invalid TODO: 'BEGIN' property is missing"));
            return false;
        }
    if(wcscmp(task->getProperty(task->propertiesCount()-1)->getName(), TEXT("END")) ||
        wcscmp(task->getProperty(task->propertiesCount()-1)->getValue(), TEXT("VTODO"))) {
            *errorCode = ERROR_KEY_PROPERTY_MISSING;
            wcscpy(errorDescription, TEXT("Invalid TODO: 'END' property is missing"));
            return false;
        }
    if(!task->containsProperty(TEXT("UID"))) { 
        *errorCode = ERROR_KEY_PROPERTY_MISSING;
        wcscpy(errorDescription, TEXT("Invalid TODO: 'UID' property is missing"));
        return false;
    }
    for(int i = 0; i < task->propertiesCount(); i++) {                
        if(!wcsstr(TODO_PROPERTIES_LIST, task->getProperty(i)->getName()) &&
            wcsstr(task->getProperty(i)->getName(),TEXT("X-")) != task->getProperty(i)->getName()) {
                *errorCode = ERROR_ILLEGAL_PROPERTY_NAME;
                swprintf(errorDescription, TEXT("TODO - Illegal property name : %s"), task->getProperty(i)->getName());
                return false;
            }
            if(task->getProperty(i)->getValue() && !validatePropery(task->getProperty(i), errorDescription, errorCode))
                return false; 
    }
    return true;
}
bool iCalConverter::validatePropery(VProperty* vp, wchar_t* errorDescription, long* errorCode) { 
    
    if(!wcscmp(vp->getName(), TEXT("CLASS"))) {
        if(!wcsstr(CLASS_PROPERTY_VALUE, vp->getValue()) &&
            wcsstr(vp->getValue(),TEXT("X-")) != vp->getValue()) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property CLASS, Invalid value : %s"), vp->getValue());
                return false;
            }
    }
    else if(!wcscmp(vp->getName(), TEXT("CREATED"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property CREATED, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DESCRIPTION"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property DESCRIPTION, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("DTSTART"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property DTSTART, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property DTSTART, Invalid value : %s"), vp->getValue());
                return false;
            }
        } 
        else if(!validateDT(vp->getValue()) && !validateDate(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property DTSTART, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("GEO"))) {
        if(!validateGeo(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property GEO, Invalid value format : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("LAST-MODIFIED"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property LAST-MODIFIED, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("LOCATION"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property LOCATION, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("ORGANIZER"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(!wcsstr(ORGANIZER_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property ORGANIZER, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("PRIORITY"))) {
        if(vp->getValue()) {
            if(wcslen(vp->getValue()) > 1 || !isdigit(vp->getValue()[0])) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property PRIORITY, Invalid value : %s"), vp->getValue());
                return false;
            }
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DTSTAMP"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property DTSTAMP, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("SUMMARY"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property SUMMARY, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("TRANSP"))) {
        if(wcscmp(vp->getName(), TEXT("OPAQUE")) && wcscmp(vp->getName(), TEXT("TRANSPARENT"))) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property TRANSP, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    
    else if(!wcscmp(vp->getName(), TEXT("RECURRENCE-ID"))) {
        for(int i = 0; i < vp->parameterCount(); i++) { 
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i) &&
                wcscmp(vp->getParameter(i), TEXT("RANGE"))) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property RECURRENCE-ID, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property RECURRENCE-ID, Invalid value : %s"), vp->getValue());
                return false;
            }
        } 
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property RECURRENCE-ID, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DTEND"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property DTEND, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
        }       
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property DTEND, Invalid value : %s"), vp->getValue());
                return false;
            }
        } 
        else if(!validateDT(vp->getValue()) && !validateDate(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property DTEND, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("DUE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) { 
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property DUE, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property DUE, Invalid value : %s"), vp->getValue());
                return false;
            }
        } 
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property DUE, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("COMPLETED"))) {
        if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property COMPLETED, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("PERCENT-COMPLETE"))) {
        if(wcslen(vp->getValue()) > 2 || 
            (wcslen(vp->getValue()) == 1 && !isdigit(vp->getValue()[0])) || 
            (wcslen(vp->getValue()) == 2 && (!isdigit(vp->getValue()[0]) || !isdigit(vp->getValue()[1])))) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property PERCENT, Invalid value : %s"), vp->getValue());
            return false;
            }
    }
    else if(!wcscmp(vp->getName(), TEXT("ATTACH"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(ATTACH_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property ATTACH, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
            if(!wcscmp(vp->getParameter(i), TEXT("ENCODING")) 
                && wcscmp(vp->getParameterValue(TEXT("ENCODING")), TEXT("BASE64"))) { 
                    *errorCode = ERROR_UNSUPPORTED_ENCODING;
                    swprintf(errorDescription, TEXT("Property ATTACH, unsupported encoding : %s"), vp->getParameterValue(TEXT("ENCODING")));
                    return false;
                }
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("ATTENDEE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(ATTENDEE_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property ATTENDEE, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("COMMENT"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property COMMENT, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("CONTACT"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property CONTACT, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("EXDATE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(DT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property EXDATE, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            wchar_t seps[] = TEXT(",");
            wchar_t *token;
            token = wcstok( vp->getValue(), seps );
            while( token != NULL ) {
                if(!validateDate(token)) {
                    *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                    swprintf(errorDescription, TEXT("Property EXDATE, Invalid value : %s"), vp->getValue());
                    return false;
                }
                token = wcstok( NULL, seps );
            }            
        } 
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property EXDATE, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("EXRULE"))) {
        if(!validateRecur(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property EXRULE, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("REQUEST-STATUS"))) {
        //statcode ";" statdesc [";" extdata]
        wchar_t* value = new wchar_t[wcslen(vp->getName())+1];
        wcscpy(value, vp->getName());
        if(!isdigit(value[0]) || value[1] != '.' || !wcschr(value, ';')) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property REQUEST-STATUS, Invalid value : %s"), vp->getValue());
            delete [] value;
            return false;
        }
        delete [] value;
    }
    else if(!wcscmp(vp->getName(), TEXT("RELATED-TO"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(wcscmp(TEXT("RELTYPE"), vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property RELATED-TO, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("RESOURCES"))) {
        for(int i = 0; i < vp->parameterCount(); i++) 
            if(!wcsstr(COMMENT_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property RESOURCES, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
    }
    else if(!wcscmp(vp->getName(), TEXT("RDATE"))) {
        for(int i = 0; i < vp->parameterCount(); i++) {
            if(!wcsstr(RDATE_PARAM_LIST, vp->getParameter(i)) &&
                wcsstr(vp->getValue(),TEXT("X-")) != vp->getParameter(i)) {
                    *errorCode = ERROR_ILLEGAL_PARAMETER;
                    swprintf(errorDescription, TEXT("Property RDATE, Invalid parameter : %s"), vp->getParameter(i));
                    return false;
                }
        }
        if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("DATE"))) {
            if(!validateDate(vp->getValue())) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property RDATE, Invalid value : %s"), vp->getValue());
                return false;
            }
        } 
        else if(vp->containsParameter(TEXT("VALUE")) && !wcscmp(vp->getParameterValue(TEXT("VALUE")), TEXT("PERIOD"))) {
            wchar_t* delimiter;
            delimiter = wcschr(vp->getValue(), '/');
            if(!delimiter) {
                *errorCode = ERROR_INVALID_PROPERTY_VALUE;
                swprintf(errorDescription, TEXT("Property RDATE, Invalid value : %s"), vp->getValue());
                return false;
            }
        }
        else if(!validateDT(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property RDATE, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    else if(!wcscmp(vp->getName(), TEXT("RRULE"))) {
        if(!validateRecur(vp->getValue())) {
            *errorCode = ERROR_INVALID_PROPERTY_VALUE;
            swprintf(errorDescription, TEXT("Property RRULE, Invalid value : %s"), vp->getValue());
            return false;
        }
    }
    return true;
}

bool iCalConverter::validateGeo(wchar_t* geo) {
    if(!geo)
        return false;

    //expected format: longitude;latitude
    wchar_t* pDest = NULL; 
    pDest = wcschr(geo, ';');

    if(!pDest)
        return false;

    wchar_t* longitude = new wchar_t[wcslen(geo)+1];
    wcsncpy(longitude, geo, pDest-geo);

    wchar_t* latitude = new wchar_t[wcslen(geo)+1];
    if(++pDest)
        wcscpy(latitude, pDest);
    else
        return false;

	wchar_t* stopstring;
   // double lon = _wtof(longitude);
   // double lat = _wtof(latitude);

	double lon = wcstod(longitude, &stopstring);
	double lat = wcstod(latitude, &stopstring);

    if(lon == 0.0 || lat == 0.0)
        return false;

    return true;
}

bool iCalConverter::validateDT(wchar_t* dt) {
    //1997 06 30 T 23 59 60 Z
    int len = int(wcslen(dt));
    if(len != 15 && len != 16)
        return false;

    if(len == 16) {
        if(dt[15] != 'Z')
            return false;
    }
    
    if(dt[8] != 'T')
        return false;

    wchar_t* date; 
    date = new wchar_t[9];
    wcsncpy(date, dt, 8);
    date[8] = 0;
    if(!validateDate(date)) {
        delete [] date;
        return false;
    }
    delete [] date;

    wchar_t hour[2];
    hour[0] = dt[9];
    hour[1] = dt[10];
    int h = _wtoi(hour);
    if(h < 0 || h > 23)
        return false;

    wchar_t min[2];
    min[0] = dt[11];
    min[1] = dt[12];
    int m = _wtoi(min);
    if(m < 0 || m > 59) 
        return false;
    
    wchar_t sec[2];
    sec[0] = dt[13];
    sec[1] = dt[14];
    int s = _wtoi(sec);
    if(s < 0 || s > 59) 
        return false;
    
    return true;

}

bool iCalConverter::validateDate(wchar_t* date) {
    if(wcslen(date) != 8)
        return false;
    
    wchar_t month[2];
    month[0] = date[4];
    month[1] = date[5];
    int mo = _wtoi(month);
    
    if(mo > 12 || mo < 1)
        return false;

    wchar_t day[2];
    day[0] = date[6];
    day[1] = date[7];

    int d = _wtoi(day);
    if(d > 31 || d < 1)
        return false;

    return true;
}

bool iCalConverter::validateRecur(wchar_t* recur) {
    
    if(wcsstr(recur, TEXT("FREQ")) != recur)
        return false;

    wchar_t seps[] = TEXT(";");
    wchar_t* token;
    wchar_t* delimiter;

    token = wcstok( recur, seps );
    while( token != NULL ) {
        delimiter = wcschr(token, '=');  
        if(!delimiter)
            return false;
        int len = int(wcslen(token));
        wchar_t* item = new wchar_t[len + 1];
        wcsncpy(item, token, delimiter - token);
        item[delimiter - token] = 0;
        
        if(!wcsstr(RECRUL_ITEMS_LIST, item) && wcsstr(item,TEXT("X-")) != item) {
            delete [] item; item = NULL;
            return false;
        }

        if(!wcscmp(item, TEXT("FREQ"))) {
            wchar_t* value = new wchar_t[len + 1];
            wcscpy(value, ++delimiter);
            if(!wcsstr(FREQUENCY_VALUES_LIST, value)) {
                delete [] value; value = NULL;
                return false;
            }
            delete [] value; value = NULL;
        }

        delete [] item; item = NULL;

        token = wcstok( NULL, seps );
    }            
    return true;
}
