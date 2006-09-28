/*
 * Copyright (C) 2005-2006 Funambol
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

#ifndef INCL_SYNCREPORT
#define INCL_SYNCREPORT

#include "base/fscapi.h"
#include "base/Log.h"
#include "spds/SyncSource.h"
#include "spds/constants.h"
#include "spds/SyncSourceReport.h"
#include "spds/SyncManagerConfig.h"


// To notify if status comes from Client or server
#define CLIENT      T("Client")
#define SERVER      T("Server")


/*
 *
 */
class SyncReport {

private:
    int   lastErrorCode;
    char* lastErrorMsg;


    // Array of report for each SyncSource.
    SyncSourceReport* ssReport;
    unsigned int ssReportCount;


    // Initialize members.
    void initialize();

    /*
     * Assign this object with the given SyncReport
     * @param sr the syncReport object
     */
    void assign(const SyncReport& sr);

public:
    SyncReport();
    SyncReport(SyncReport& sr);
    SyncReport(SyncManagerConfig& config);
    virtual ~SyncReport();

    const int          getLastErrorCode()         const;
    const char*        getLastErrorMsg()          const;
    const unsigned int getSyncSourceReportCount() const;

    void setLastErrorCode(const int code);
    void setLastErrorMsg (const char* msg);

    // Return pointer to internal object given the source name or index
    SyncSourceReport* getSyncSourceReport(const char* name)   const;
    SyncSourceReport* getSyncSourceReport(unsigned int index) const;
    
    // Create ssReport array from config.
    void setSyncSourceReports(SyncManagerConfig& config);
    

    /*
     * Assign operator
     */
    SyncReport& operator = (const SyncReport& sr) {
        assign(sr);
        return *this;
    }
};

#endif

