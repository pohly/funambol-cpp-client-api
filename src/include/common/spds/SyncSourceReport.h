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

#ifndef INCL_SYNCSOURCEREPORT
#define INCL_SYNCSOURCEREPORT

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/utils.h"
#include "spds/constants.h"
#include "event/constants.h"
#include "spds/ItemReport.h"


// Possible states of syncsource (state member)
typedef enum SourceState{
        SOURCE_ACTIVE       = 0,
        SOURCE_INACTIVE     = 1,
        SOURCE_ERROR        = 2,
} SourceState;

/*
 *
 */
class SyncSourceReport {

private:
    int         lastErrorCode;
    char*       lastErrorMsg;
    char*       sourceName;
    SourceState state;

    ArrayList*  clientAddItems;
    ArrayList*  clientModItems;
    ArrayList*  clientDelItems;

    ArrayList*  serverAddItems;
    ArrayList*  serverModItems;
    ArrayList*  serverDelItems;


    // Return true if status is [200 <-> 299] (successful)
    bool isSuccessful(const int status);

    // Initialize members.
    void initialize();

    /*
     * Assign this object with the given SyncReport
     * @param sr the syncReport object
     */
    void assign(const SyncSourceReport& ssr);

public:
    SyncSourceReport(const char* name = NULL);
    SyncSourceReport(SyncSourceReport& ssr);
    virtual ~SyncSourceReport();

    const int   getLastErrorCode() const;
    const SourceState   getState() const;
    const char*  getLastErrorMsg() const;
    const char*    getSourceName() const;

    void setLastErrorCode(const int code);
    void setState        (const SourceState s);
    void setLastErrorMsg (const char* msg);
    void setSourceName   (const char* name);

    // Returns true if source is active (state = SOURCE_ACTIVE).
    bool checkState();


    // Get internal pointer to the Item report #index.
    ItemReport* getItemReport(const char* target, const char* command, unsigned int index);

    // Return number of ItemReport for a specific list (based on target, command).
    int getItemReportCount          (const char* target, const char* command);
    int getItemReportSuccessfulCount(const char* target, const char* command);
    int getItemReportFailedCount    (const char* target, const char* command);

    // target  = CLIENT/SERVER
    // command = Add/Replace/Delete
    // ID      = LUID of item                \ ItemReport element
    // status  = status code (201/500/...)   / ******************
    void addItem(const char* target, const char* command, const WCHAR* ID, const int status);


    // To switch on the right list, based on target and command.
    ArrayList* getList(const char* target, const char* command) const;


    /*
     * Assign operator
     */
    SyncSourceReport& operator = (const SyncSourceReport& ssr) {
        assign(ssr);
        return *this;
    }
};

#endif

