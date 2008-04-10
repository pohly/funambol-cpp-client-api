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

#include <eikenv.h>
#include "base/SymbianLog.h"
#include "base/util/symbianUtils.h"
#include "base/util/stringUtils.h"
#include "base/util/timeUtils.h"


// Formats for date&time printed into log.
_LIT(KFormatDateAndTime, "%*D%*N%Y%1-%2-%3 %:0%J%:1%T%:2%S");
_LIT(KFormatOnlyTime,    "%:0%J%:1%T%:2%S");


SymbianLog::SymbianLog(bool resetLog, const char* path, const char* name) 
{
    //msgBox(_L("init log"));
    iLogName.Assign(charToNewBuf(SYMBIAN_LOG_NAME));

    // Connect to the file server session.
    fsSession.Connect();       
    
    TInt err = KErrNone;
    if (resetLog) {
        err = file.Replace(fsSession, iLogName, EFileWrite|EFileShareAny);
    }
    else {
        err = file.Open(fsSession, iLogName, EFileWrite|EFileShareAny);
        if (err == KErrNotFound) {
            err = file.Create(fsSession, iLogName, EFileWrite|EFileShareAny);
        }
        else {
            // Log file existing, and not resetted. Will append data.
            TInt pos;
            file.Seek(ESeekEnd, pos);
        }
    }
    User::LeaveIfError(err);
    
    // Write the Header
    StringBuffer header = createHeader();
    RBuf8 data;
    data.Assign(stringBufferToNewBuf8(header));
    file.Write(data);

    file.Close();
    fsSession.Close();
    return;
}

SymbianLog::~SymbianLog() {   
}

void SymbianLog::setLogPath(const char* configLogPath) {
// TODO: implement
}

void SymbianLog::setLogName(const char* configLogName) {
// TODO: implement
}



StringBuffer SymbianLog::createHeader(const char* title) 
{
    const char *t = (title)? title : SYMBIAN_LOG_HEADER;
    
    StringBuffer header = createCurrentTime(true);      // e.g. "2008/04/02 10:58:03 GMT +2:00"
    header += " - ";
    header += t;
    header += "\n";

    return header;
}

StringBuffer SymbianLog::createCurrentTime(bool complete) 
{
    if (iFormattedBias.length() == 0) {
        // Create the string e.g. "GMT +2:00" only once (it's always the same)
        createFormattedBias();
    }
    
    TTime local;
    local.HomeTime();
    
    TBuf<50> formattedTime;
    if (complete) { local.FormatL(formattedTime, KFormatDateAndTime); }
    else          { local.FormatL(formattedTime, KFormatOnlyTime);    }
    
    StringBuffer ret;
    const char* date = bufToNewChar(formattedTime);
    ret.sprintf("%s %s", date, iFormattedBias.c_str());
    delete [] date;
    return ret;
}


void SymbianLog::createFormattedBias() 
{
    TTime local, utc;
    TTimeIntervalMinutes bias;
    
    local.HomeTime();
    utc.UniversalTime();
    local.MinutesFrom(utc, bias);

    TInt totalMinutes = bias.Int();
    TInt hours   = totalMinutes / 60;
    TInt minutes = totalMinutes % 60;
    
    if (totalMinutes >= 0) { iFormattedBias.sprintf("GMT +%d:%02d", hours, minutes); }
    else                   { iFormattedBias.sprintf("GMT %d:%02d",  hours, minutes); }
}



void SymbianLog::error(const char*  msg, ...) 
{
    PLATFORM_VA_LIST argList;
    PLATFORM_VA_START (argList, msg);
    printMessage(LOG_ERROR, msg, argList);
    PLATFORM_VA_END(argList);
}
void SymbianLog::info (const char*  msg, ...) 
{
    if (isLoggable(LOG_LEVEL_INFO)) {
        PLATFORM_VA_LIST argList;
        PLATFORM_VA_START (argList, msg);
        printMessage(LOG_INFO, msg, argList);
        PLATFORM_VA_END(argList);
    }
}
void SymbianLog::debug(const char*  msg, ...) 
{
    if (isLoggable(LOG_LEVEL_DEBUG)) {
        PLATFORM_VA_LIST argList;
        PLATFORM_VA_START (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        PLATFORM_VA_END(argList);
    }
}
void SymbianLog::developer(const char*  msg, ...) 
{
    if (isLoggable(LOG_LEVEL_INFO)) {
        PLATFORM_VA_LIST argList;
        PLATFORM_VA_START (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);
        PLATFORM_VA_END(argList);
    }
}

void SymbianLog::printMessage(const char* level, const char* msg, PLATFORM_VA_LIST argList) 
{
    StringBuffer currentTime = createCurrentTime(false);
    
    fsSession.Connect();
    
    TInt err = file.Open(fsSession, iLogName, EFileWrite|EFileShareAny);
    User::LeaveIfError(err);
    
    TInt pos = 0;
    err = file.Seek(ESeekEnd, pos);
    User::LeaveIfError(err);

    // Write the data
    StringBuffer line, data;
    line.sprintf("%s -%s- %s", currentTime.c_str(), level, msg);
    data.vsprintf(line.c_str(), argList);
    data.append("\n");
    
    RBuf8 buf;
    buf.Assign(stringBufferToNewBuf8(data));
    file.Write(buf);
    
    file.Close();
    fsSession.Close();
}

void SymbianLog::reset(const char* title) 
{
    fsSession.Connect();
    
    TInt err = file.Replace(fsSession, iLogName, EFileWrite|EFileShareAny);
    User::LeaveIfError(err);
    
    // Write the Header
    StringBuffer header = createHeader(title);
    RBuf8 buf;
    buf.Assign(stringBufferToNewBuf8(header));
    file.Write(buf);
    
    file.Close();
    fsSession.Close();
}


size_t SymbianLog::getLogSize() {

    fsSession.Connect();
    
    TInt size = 0;
    TInt err = file.Size(size);
    User::LeaveIfError(err);

    file.Close();
    fsSession.Close();
    return (size_t)size;
}



Log *Log::logger;

Log &Log::instance() {
    if (!logger) {
        logger = new SymbianLog();
    }
    return *logger;
}
