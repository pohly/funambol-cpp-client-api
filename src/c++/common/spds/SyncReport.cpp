/*
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "spds/SyncReport.h"

//--------------------------------------------------- Constructor & Destructor

SyncReport::SyncReport() {
    initialize();
}

SyncReport::SyncReport(SyncReport& sr) {
    initialize();
    assign(sr);
}

SyncReport::SyncReport(SyncManagerConfig& config) {
    initialize();
    setSyncSourceReports(config);
}

SyncReport::~SyncReport() {

    if (lastErrorMsg) {
        delete [] lastErrorMsg;
        lastErrorMsg = NULL;
    }
    if (ssReport) {
        delete [] ssReport;
        ssReport = NULL;
    }
}


//------------------------------------------------------------- Public Methods

// Create ssReport array from config.
void SyncReport::setSyncSourceReports(SyncManagerConfig& config) {

    ssReportCount = config.getSyncSourceConfigsCount();
    ssReport = new SyncSourceReport[ssReportCount];

    SyncSourceConfig* sc = NULL;
    for (unsigned int i=0; i<ssReportCount; i++) {
        sc = config.getSyncSourceConfig(i);
        ssReport[i].setSourceName(sc->getName());
    }
}


const int SyncReport::getLastErrorCode() const {
    return lastErrorCode;
}
void SyncReport::setLastErrorCode(const int code) {
    lastErrorCode = code;
}

const char* SyncReport::getLastErrorMsg() const {
    return lastErrorMsg;
}
void SyncReport::setLastErrorMsg(const char* msg) {
    if (lastErrorMsg) {
        delete [] lastErrorMsg;
        lastErrorMsg = NULL;
    }
    lastErrorMsg = stringdup(msg);
}



SyncSourceReport* SyncReport::getSyncSourceReport(const char* name) const {
    if ((name == NULL) || (strlen(name) == 0)) {
        return NULL;
    }
    for (unsigned int i=0; i<ssReportCount; i++) {
        if (!strcmp(ssReport[i].getSourceName(), name)) {
            return (ssReport+i);
        }
    }
    // if here, source not found
    return NULL;
}

SyncSourceReport* SyncReport::getSyncSourceReport(unsigned int index) const {
    if (index >= ssReportCount) {
        return NULL;
    }
    return (ssReport+index);
}


const unsigned int SyncReport::getSyncSourceReportCount() const {
    return ssReportCount;
}


//------------------------------------------------------------- Private Methods

void SyncReport::initialize() {
    lastErrorCode  = ERR_NONE;
    lastErrorMsg   = NULL;
    ssReportCount  = 0;
    ssReport       = NULL;
}

void SyncReport::assign(const SyncReport& sr) {

    setLastErrorCode(sr.getLastErrorCode());
    setLastErrorMsg (sr.getLastErrorMsg());
    ssReportCount = sr.getSyncSourceReportCount();

    ssReport = new SyncSourceReport[ssReportCount];
    for (unsigned int i=0; i<ssReportCount; i++) {
        ssReport[i] = *(sr.getSyncSourceReport(i));
    }
}
