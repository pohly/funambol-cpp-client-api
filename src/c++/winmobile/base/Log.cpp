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


#include "base/Log.h"
#include "base/util/utils.h"

Log LOG = Log(false);

wchar_t logmsg[512];

FILE* logFile;
wchar_t logPath[256];   

void setLogPath(wchar_t* configLogPath) {
    
    wmemset(logPath, 0, 256);
    if (configLogPath != NULL) {
        wsprintf(logPath, TEXT("%s/"), configLogPath); 
    } 
}


/*
* return a the time to write into log file. If complete is true, it return 
* the date too, else only hours, minutes, seconds and milliseconds
*/ 
wchar_t* getCurrentTime(BOOL complete) {
    
    SYSTEMTIME sys_time;   
    GetLocalTime(&sys_time);

    wchar_t fmtComplete[] = TEXT("%04d-%02d-%02d %02d:%02d:%02d:%03d");
    wchar_t fmt[]         = TEXT("%02d:%02d:%02d:%03d");

    wchar_t* ret = new wchar_t [64];
    
    if (complete) {
        wsprintf(ret, fmtComplete, sys_time.wYear, sys_time.wMonth, sys_time.wDay,  
                      sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
    } else {
        wsprintf(ret, fmt, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
    }
    return ret;

}


Log::Log(BOOL resetLog) {
    wsprintf(logPath, TEXT("/%s"), LOG_NAME);
	if (resetLog) {
        reset();
    }
}

Log::~Log() {
    if (logFile != NULL) {
        fclose(logFile);
    }
}

void Log::error(const wchar_t* msg) {    
        printMessage(LOG_ERROR, msg);    
}

void Log::info(const wchar_t* msg) {
    if (logLevel >= LOG_LEVEL_INFO) {
        printMessage(LOG_INFO, msg);
    }
}

void Log::debug(const wchar_t* msg) {
    if (logLevel >= LOG_LEVEL_DEBUG) {
        printMessage(LOG_DEBUG, msg);
    }
}

void Log::trace(const wchar_t* msg) { 
}

void Log::setLevel(LogLevel level) {
    logLevel = level;
}

LogLevel Log::getLevel() {
    return logLevel;
}

BOOL Log::isLoggable(LogLevel level) {
    return (level >= logLevel);
}

void Log::printMessage(const wchar_t* level, const wchar_t* msg) {       
    
    wchar_t* currentTime = getCurrentTime(false);
    
    logFile     = _wfopen(logPath, TEXT("a+"));          
    fwprintf(logFile, TEXT("%s [%s] - %s\n"), currentTime, level, msg); fflush(logFile);
    fclose(logFile);

    delete[] currentTime;
}

void Log::reset() {
    
    wchar_t* currentTime = getCurrentTime(true);
    
    logFile     = _wfopen(logPath, TEXT("w+"));      
    fwprintf(logFile, TEXT("%s - # Sync4j Client API Log\n"), currentTime); fflush(logFile);    
    fclose(logFile);

    delete[] currentTime;    
}


