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

char logmsg[512];

static FILE* logFile = NULL;
static BCHAR logFullName[512] = "\\" LOG_NAME ;    
static BCHAR logName[128] = LOG_NAME;
static BCHAR logPath[256] = "\\" ;   

//---------------------------------------------------------------------- Static Functions

/*
* return a the time to write into log file. If complete is true, it return 
* the date too, else only hours, minutes, seconds and milliseconds
*/ 
static BCHAR* getCurrentTime(BOOL complete) {
    
    SYSTEMTIME sys_time;   
    GetLocalTime(&sys_time);

    BCHAR fmtComplete[] = T("%04d-%02d-%02d %02d:%02d:%02d:%03d");
    BCHAR fmt[]         = T("%02d:%02d:%02d:%03d");

    BCHAR* ret = new BCHAR [64];

    if (complete) {
        bsprintf(ret, fmtComplete, sys_time.wYear, sys_time.wMonth, sys_time.wDay,
                 sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
    } else {
        bsprintf(ret, fmt, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
    }
    return ret;
}

//---------------------------------------------------------------------- Constructors

Log::Log(BOOL resetLog, BCHAR* path, BCHAR* name) {

    setLogPath(path);
    setLogName(name);
	if (resetLog) {
        reset("Funambol WindowsMobile Plugin Log");
    }
}

Log::~Log() {
    if (logFile != NULL) {
        fclose(logFile);
    }
}

//---------------------------------------------------------------------- Public methods

void Log::setLogPath(BCHAR* configLogPath) {
    
    if (configLogPath != NULL) {
        bsprintf(logPath, T("%s/"), configLogPath); 
    } else {
        bsprintf(logPath, T("%s"), T("./"));
    }
}

void Log::setLogName(BCHAR* configLogName) {
    
    if (configLogName != NULL) {
        bsprintf(logName, T("%s"), configLogName); 
    }
    else {
        bsprintf(logName, T("%s"), LOG_NAME);         
    }
}

void Log::error(const BCHAR* msg, ...) {    
    va_list argList;
    va_start (argList, msg);
    printMessage(LOG_ERROR, msg, argList);    
    va_end(argList);
}

void Log::info(const BCHAR* msg, ...) {
    if (logLevel >= LOG_LEVEL_INFO) {
        va_list argList;
	    va_start (argList, msg);
        printMessage(LOG_INFO, msg, argList);    
	    va_end(argList);

    }
}

void Log::debug(const BCHAR* msg, ...) {
    if (logLevel >= LOG_LEVEL_DEBUG) {
	    va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);    
        va_end(argList);
        
    }
}

void Log::trace(const BCHAR* msg) { 
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

void Log::printMessage(const BCHAR* level, const BCHAR* msg, va_list argList) {       
    
	BCHAR* currentTime = getCurrentTime(false);    
    logFile = fopen(logFullName, "a+");       
	
	fprintf(logFile, "%s [%s] - ", currentTime, level); 		
    vfprintf(logFile, msg, argList);	
	fprintf(logFile, "\n"); 
	fclose(logFile);

    delete[] currentTime;	
}

void Log::printMessageW(const char* level, const wchar_t* msg, va_list argList) {       
    
	BCHAR* currentTime = getCurrentTime(false);    
    logFile = fopen(logFullName, "a+");       
	
	fprintf(logFile, "%s [%s] - ", currentTime, level);
    // Write the wchar_t parameters
    vfwprintf(logFile, msg, argList);	
	fprintf(logFile, "\n"); 
	fclose(logFile);

    delete[] currentTime;	
}


void Log::reset(const BCHAR* title) {
    
    const char *t = (title) ? title : "Funambol SDK C++ Log";

    BCHAR* currentTime = getCurrentTime(true);
    memset(logFullName, 0, 512*sizeof(BCHAR));
    bsprintf(logFullName, T("%s%s"), logPath, logName);
    logFile = fopen(logFullName, T("w+"));      
    fprintf(logFile, T("%s - %s\n"), t, currentTime);
    fclose(logFile);

    delete[] currentTime;    
}


/*
void Log::printMessage(const BCHAR* level, const BCHAR* msg) {           	
	
    BCHAR* currentTime = getCurrentTime(false);
    logFile     = _wfopen(logPath, TEXT("a+"));       
    fwprintf(logFile, TEXT("%s [%s] - %s\n"), currentTime, level, msg); fflush(logFile);
	fclose(logFile);
    delete[] currentTime;
	
}
*/

