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

//static FILE* logFile = NULL;
//static wchar_t logDir[512] = TEXT("\\synclog.txt");   
//static wchar_t logName[128] = LOG_NAME;
//static wchar_t logPath[256] = TEXT("\\");
FILE* logFile;
wchar_t logDir[512];   
wchar_t logName[128];
wchar_t logPath[256];   

void Log::setLogPath(wchar_t* configLogPath) {
    
    wmemset(logPath, 0, 512);
    if (configLogPath != NULL) {
        wsprintf(logPath, TEXT("%s/"), configLogPath); 
    } else {
        wsprintf(logPath, TEXT("%s"), TEXT("./"));         
    }
}

void Log::setLogName(wchar_t* configLogName) {
    
    wmemset(logName, 0, 128);
    if (configLogName != NULL) {
        wsprintf(logName, TEXT("%s"), configLogName); 
    } else {
        wsprintf(logName, TEXT("%s"), LOG_NAME);         
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


Log::Log(BOOL resetLog, wchar_t* path, wchar_t* name) {

    setLogPath(path);
    setLogName(name);
	if (resetLog) {
        reset();
    }
}

Log::~Log() {
    if (logFile != NULL) {
        fclose(logFile);
    }
}

void Log::error(const wchar_t* msg, ...) {    
    va_list argList;
    va_start (argList, msg);
    printMessage(LOG_ERROR, msg, argList);    
    va_end(argList);
}
void Log::error(const wchar_t* msg, va_list argList) {    
    printMessage(LOG_ERROR, msg, argList);        
}

void Log::info(const wchar_t* msg, ...) {
    if (logLevel >= LOG_LEVEL_INFO) {
        va_list argList;
	    va_start (argList, msg);
        printMessage(LOG_INFO, msg, argList);    
	    va_end(argList);

    }
}
void Log::info(const wchar_t* msg, va_list argList) {    
    printMessage(LOG_INFO, msg, argList);        
}

void Log::debug(const wchar_t* msg, ...) {
    if (logLevel >= LOG_LEVEL_DEBUG) {
	    va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);    
        va_end(argList);
        
    }
}

void Log::debug(const wchar_t* msg, va_list argList) {    
    printMessage(LOG_DEBUG, msg, argList);        
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

void Log::printMessage(const wchar_t* level, const wchar_t* msg, va_list argList) {       
    
	wchar_t* currentTime = getCurrentTime(false);    
    logFile     = _wfopen(logDir, TEXT("a+"));       
	
	fwprintf(logFile, TEXT("%s [%s] - "), currentTime, level); 		
    vfwprintf(logFile, msg, argList);	
	fwprintf(logFile, TEXT("\n")); 
	fclose(logFile);

    delete[] currentTime;	
}

void Log::reset() {
    
    wchar_t* currentTime = getCurrentTime(true);
    wmemset(logDir, 0, 512);
    wsprintf(logDir, TEXT("%s/%s"), logPath, logName);
    logFile     = _wfopen(logDir, TEXT("w+"));      
    fwprintf(logFile, TEXT("%s - # Funambol WindowsMobile Plugin Log\n"), currentTime); fflush(logFile);    
    fclose(logFile);

    delete[] currentTime;    
}


/*
void Log::printMessage(const wchar_t* level, const wchar_t* msg) {           	
	
    wchar_t* currentTime = getCurrentTime(false);
    logFile     = _wfopen(logPath, TEXT("a+"));       
    fwprintf(logFile, TEXT("%s [%s] - %s\n"), currentTime, level, msg); fflush(logFile);
	fclose(logFile);
    delete[] currentTime;
	
}
*/