 /*
 * Copyright (C) 2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
*/

#include "base/util/utils.h"
#include "vocl/WinTask.h"
#include "vocl/VConverter.h"
#include "base/stringUtils.h"
#include "base/timeUtils.h"

using namespace std;



// Constructor
WinTask::WinTask() {
    vCalendar = L"";
}

// Constructor: fills propertyMap parsing the passed data
WinTask::WinTask(const wstring dataString) {
    vCalendar = L"";
    parse(dataString);
}

// Destructor
WinTask::~WinTask() {
}




// Format and return a vCalendar string from the propertyMap.
wstring& WinTask::toString() {

    vCalendar = L"";

    //
    // Conversion: WinTask -> vObject.
    // -------------------------------
    //
    VObject* vo = new VObject();
    VProperty* vp  = NULL;
    wstring element;


    vp = new VProperty(TEXT("BEGIN"), TEXT("VCALENDAR"));
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("VERSION"), VCALENDAR_VERSION);
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("BEGIN"), TEXT("VTODO"));
    vo->addProperty(vp);
    delete vp; vp = NULL;


    // Folder path.
    if (getProperty(L"Folder", element)) {
        vp = new VProperty(L"X-FUNAMBOL-FOLDER");
        vp->addValue(element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    // Tasks are ALWAYS all-day-event!
    vp = new VProperty(TEXT("X-FUNAMBOL-ALLDAY"), TEXT("1"));
    vo->addProperty(vp);
    delete vp; vp = NULL;


    if (getProperty(L"Subject", element)) {
        vp = new VProperty(TEXT("SUMMARY"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Body", element)) {
        vp = new VProperty(TEXT("DESCRIPTION"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    if (getProperty(L"DateCompleted", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        //if (element.size() == 8) { 
        //    element += TEXT("T000000");
        //}
        vp = new VProperty(TEXT("COMPLETED"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"DueDate", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        //if (element.size() == 8) { 
        //    element += TEXT("T235900");
        //}
        vp = new VProperty(TEXT("DUE"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"StartDate", element)) {
        replaceAll(L"-", L"", element);                         // **** To be removed!!! ****
        //if (element.size() == 8) { 
        //    element += TEXT("T000000");
        //}
        vp = new VProperty(TEXT("DTSTART"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    if (getProperty(L"Categories", element)) {
        vp = new VProperty(TEXT("CATEGORIES"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Complete", element)) {
        BOOL isCompleted = _wtoi(element.c_str());
        if (isCompleted) { 
            vp = new VProperty(TEXT("STATUS"), TEXT("COMPLETED"));
            vo->addProperty(vp);
            delete vp; vp = NULL;
        }
    }
    if (getProperty(L"Importance", element)) {
        vp = new VProperty(TEXT("PRIORITY"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"PercentComplete", element)) {
        vp = new VProperty(TEXT("PERCENT-COMPLETE"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(L"Sensitivity", element)) {
        long sensitivity = _wtoi(element.c_str());
        vp = new VProperty(TEXT("CLASS"));
        if(sensitivity == winPrivate) {
            vp->addValue(TEXT("PRIVATE"));
        }
        else if (sensitivity == winConfidential) {
            vp->addValue(TEXT("CONFIDENTIAL"));
        }
        else {  // default value
            vp->addValue(TEXT("PUBLIC"));
        }
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }

    //
    // **** "Status"?   mapping is missing! ****
    // **** "TeamTask"? mapping is missing! ****
    //

    //
    // Recurrence pattern -> RRULE
    //
    if (getProperty(L"IsRecurring", element)) {
        BOOL isRec = _wtoi(element.c_str());
        if(isRec) {
            wstring rRule = recPattern.toString();
            if(rRule != L"") {
                vp = new VProperty(TEXT("RRULE"), rRule.c_str());
                vo->addProperty(vp);
                delete vp; vp = NULL;
            }
        }
        else {
            // Not recurring: send empty "RRULE:"
            vp = new VProperty(TEXT("RRULE"));
            vo->addProperty(vp);
            delete vp; vp = NULL;
        }
    }


    //
    // ReminderSet
    //
    if (getProperty(L"ReminderSet", element)) {
        BOOL bReminder = _wtoi(element.c_str());
        if(bReminder == TRUE) {
            if (getProperty(L"ReminderTime", element)) {

                vp = new VProperty(L"AALARM");
                vp->addValue(element.c_str());                      // "RunTime"
                vp->addValue(L"");                                  // "Snooze Time" (empty)
                vp->addValue(L"0");                                 // "Repeat Count"

                getProperty(L"ReminderSoundFile", element);         // (empty if not found)
                vp->addValue(element.c_str());                      // "Audio Content" = sound file path

                vo->addProperty(vp);
                delete vp; vp = NULL;
            }
        }
        else {
            // No reminder: send empty "AALARM:"
            vp = new VProperty(L"AALARM");
            vo->addProperty(vp);
            delete vp; vp = NULL;
        }
    }


    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (getProperty(TEXT("ActualWork"), element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-ACTUALWORK"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(TEXT("BillingInformation"), element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-BILLINGINFO"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(TEXT("Companies"), element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-COMPANIES"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(TEXT("Mileage"), element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-MILEAGE"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }
    if (getProperty(TEXT("TotalWork"), element)) {
        vp = new VProperty(TEXT("X-FUNAMBOL-TOTALWORK"), element.c_str());
        vo->addProperty(vp);
        delete vp; vp = NULL;
    }


    vp = new VProperty(TEXT("END"), TEXT("VTODO"));
    vo->addProperty(vp);
    delete vp; vp = NULL;

    vp = new VProperty(TEXT("END"), TEXT("VCALENDAR"));
    vo->addProperty(vp);
    delete vp; vp = NULL;


    //
    // Format the vCalendar.
    // ---------------------
    //
    WCHAR* tmp = vo->toString();
    if (tmp) {
        vCalendar = tmp;
        delete [] tmp;
    }
    return vCalendar;
}




//
// Parse a vCalendar string and fills the propertyMap.
//
int WinTask::parse(const wstring dataString) {

    WCHAR* element = NULL;
    DATE startDate = NULL;

    //
    // Parse the vCalendar and fill the VObject.
    // -----------------------------------------
    //
    VObject* vo = VConverter::parse(dataString.c_str());
    if (!vo) {
        //sprintf(lastErrorMsg, ERR_ITEM_VOBJ_PARSE);
        setError(1, ERR_ITEM_VOBJ_PARSE);
        LOG.error(getLastErrorMsg());
        return 1;
    }
    // Check if VObject type and version are the correct ones.
    if (!checkVCalendarTypeAndVersion(vo)) {
        if (vo) delete vo;
        return 1;
    }


    //
    // Conversion: vObject -> WinTask.
    // -------------------------------
    // Note: properties found are added to the propertyMap, so that the 
    //       map will contain only parsed properties after this process.
    //
    if (element = getVObjectPropertyValue(vo, L"SUMMARY")) {
        setProperty(L"Subject", element);
    }
    if (element = getVObjectPropertyValue(vo, L"DESCRIPTION")) {
        setProperty(L"Body", element);
    }
    if (element = getVObjectPropertyValue(vo, L"X-FUNAMBOL-FOLDER")) {
        setProperty(L"Folder", element);
    }

    // Tasks are ALWAYS all-day-events!
    setProperty(L"AllDayEvent", TEXT("1"));

    if (element = getVObjectPropertyValue(vo, L"DTSTART")){
        setProperty(L"StartDate", element);
        stringTimeToDouble(element, &startDate);            // 'startDate' will be used also for RRULE parsing
    }
    if (element = getVObjectPropertyValue(vo, L"DUE")){
        setProperty(L"DueDate", element);
    }
    if (element = getVObjectPropertyValue(vo, L"COMPLETED")){
        setProperty(L"DateCompleted", element);
    }
    if (element = getVObjectPropertyValue(vo, L"CATEGORIES")) {
        setProperty(L"Categories", element);
    }
    if (element = getVObjectPropertyValue(vo, L"CLASS")) {
        WCHAR tmp[10];
        if (!wcscmp(element, TEXT("CONFIDENTIAL")) ) {
            wsprintf(tmp, TEXT("%i"), winConfidential);     // Confidential = 3
        }
        else if (!wcscmp(element, TEXT("PRIVATE")) ) {
            wsprintf(tmp, TEXT("%i"), winPrivate);          // Private = 2
        }
        else {
            wsprintf(tmp, TEXT("%i"), winNormal);           // Normal = 0
        }
        setProperty(L"Sensitivity", tmp);
    }
    if (element = getVObjectPropertyValue(vo, L"PERCENT-COMPLETE")) {
        setProperty(L"PercentComplete", element);
    }
    if (element = getVObjectPropertyValue(vo, L"PRIORITY")) {
        setProperty(L"Importance", element);
    }
    if (element = getVObjectPropertyValue(vo, L"STATUS")) {
        if(!wcscmp(element, TEXT("COMPLETED"))) {
            setProperty(L"Complete", TEXT("1"));
        }
    }

    //
    // **** "Status"?   mapping is missing! ****
    // **** "TeamTask"? mapping is missing! ****
    //


    //
    // AALARM
    // The value consists of: RunTime, SnoozeTime, RepeatCount, AudioContent
    //
    if(element = getVObjectPropertyValue(vo, L"AALARM")) {
        WCHAR* runTimeValue = vo->getProperty(TEXT("AALARM"))->getPropComponent(1);
        if (wcslen(runTimeValue) > 0) {
            setProperty(TEXT("ReminderSet"),  TEXT("1"));
            setProperty(TEXT("ReminderTime"), runTimeValue);

            // Reminder sound file path
            WCHAR* filePath = vo->getProperty(TEXT("AALARM"))->getPropComponent(4);
            if (filePath && wcslen(filePath)>0) {
                setProperty(TEXT("ReminderSoundFile"), filePath);
            }
            else {
                setProperty(TEXT("ReminderSoundFile"), TEXT(""));
            }
        }
        else {
            // RunTime not found -> no reminder
            setProperty(TEXT("ReminderSet"), TEXT("0"));
        }
    }
    else {
        // AALARM not found -> reset reminder!
        // Note: this is done for compatibility with most devices: if alarm not set
        //       AALARM property is not sent.
        setProperty(TEXT("ReminderSet"), TEXT("0"));
    }


    //
    // RRULE -> Recurrence pattern
    // Fill recPattern propertyMap.
    //
    if(element = getVObjectPropertyValue(vo, L"RRULE")) {
        setProperty(L"IsRecurring", L"1");
        recPattern.setStartDate(startDate);
        recPattern.parse(element);
    }
    else {
        // Not recurring.
        setProperty(L"IsRecurring", L"0");
    }



    //
    // ---- Other Funambol defined properties ----
    // Support for other fields that don't have a
    // specific correspondence in vCalendar.
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-ACTUALWORK"))) {
        setProperty(TEXT("ActualWork"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-BILLINGINFO"))) {
        setProperty(TEXT("BillingInformation"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-COMPANIES"))) {
        setProperty(TEXT("Companies"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-MILEAGE"))) {
        setProperty(TEXT("Mileage"), element);
    }
    if (element = getVObjectPropertyValue(vo, TEXT("X-FUNAMBOL-TOTALWORK"))) {
        setProperty(TEXT("TotalWork"), element);
    }


    return 0;
}



// Utility to check the productID and version of VObject passed.
bool WinTask::checkVCalendarTypeAndVersion(VObject* vo) {

    WCHAR* prodID  = vo->getProdID();
    WCHAR* version = vo->getVersion();
    
    if (!prodID) {
        LOG.error(ERR_ITEM_VOBJ_TYPE_NOTFOUND, L"VCALENDAR");
        return false;
    }
    if (wcscmp(prodID, L"VCALENDAR")) {
        LOG.error(ERR_ITEM_VOBJ_WRONG_TYPE, prodID, L"VCALENDAR");
        return false;
    }

    if (!version) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_VERSION_NOTFOUND, VCALENDAR_VERSION);
    }
    else if (wcscmp(version, VCALENDAR_VERSION)) {
        // Just log a warning...
        LOG.info(INFO_ITEM_VOBJ_WRONG_VERSION, version, VCALENDAR_VERSION);
    }
    return true;
}



WinRecurrence* WinTask::getRecPattern() {
    return &recPattern;
}
