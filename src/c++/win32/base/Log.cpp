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

FILE* logFile;
char logDir[512];   
char logName[128];
char logPath[256];   

void Log::setLogPath(char* configLogPath) {
    
    memset(logPath, 0, 512*sizeof(char));
    if (configLogPath != NULL) {
        sprintf(logPath, T("%s/"), configLogPath); 
    } else {
        sprintf(logPath, T("%s"), T("./"));
    }
}

void Log::setLogName(char* configLogName) {
    
    memset(logName, 0, 128*sizeof(char));
    if (configLogName != NULL) {
        sprintf(logName, T("%s"), configLogName); 
    } else {
        sprintf(logName, T("%s"), LOG_NAME);         
    }
}

/*
* return a the time to write into log file. If complete is true, it return 
* the date too, else only hours, minutes, seconds and milliseconds
*/ 
char* getCurrentTime(BOOL complete) {
    
    SYSTEMTIME sys_time;   
    GetLocalTime(&sys_time);

    char fmtComplete[] = T("%04d-%02d-%02d %02d:%02d:%02d:%03d");
    char fmt[]         = T("%02d:%02d:%02d:%03d");

    char* ret = new char [64];
    
    if (complete) {
        sprintf(ret, fmtComplete, sys_time.wYear, sys_time.wMonth, sys_time.wDay,
                 sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
    } else {
        sprintf(ret, fmt, sys_time.wHour, sys_time.wMinute, sys_time.wSecond, sys_time.wMilliseconds);
    }
    return ret;

}


Log::Log(BOOL resetLog, char* path, char* name) {

    setLogPath(path);
    setLogName(name);
    memset(logDir, 0, 512*sizeof(char));
    sprintf(logDir, T("%s%s"), logPath, logName);
	if (resetLog) {
        reset(LOG_NAME);
    }
}

Log::~Log() {
    if (logFile != NULL) {
        fclose(logFile);
    }
}

void Log::error(const char* msg, ...) {    
    va_list argList;
    va_start (argList, msg);
    printMessage(LOG_ERROR, msg, argList);    
    va_end(argList);
}


void Log::info(const char* msg, ...) {
    if (logLevel >= LOG_LEVEL_INFO) {
        va_list argList;
	    va_start (argList, msg);
        printMessage(LOG_INFO, msg, argList);    
	    va_end(argList);

    }
}


void Log::debug(const char* msg, ...) {
    if (logLevel >= LOG_LEVEL_DEBUG) {
	    va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);    
        va_end(argList);
        
    }
}



void Log::trace(const char* msg) { 
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

void Log::printMessage(const char* level, const char* msg, va_list argList) {       
    
	char* currentTime = getCurrentTime(false);    
    logFile = fopen(logDir, T("a+"));       
	
	fprintf(logFile, T("%s [%s] - "), currentTime, level); 		
    vfprintf(logFile, msg, argList);	
	fprintf(logFile, T("\n")); 
	fclose(logFile);

    delete[] currentTime;	
}

void Log::reset(const char* title) {
    
    char* currentTime = getCurrentTime(true);
    logFile = fopen(logDir, T("w+"));      
    fprintf(logFile, T("%s - # %s\n\n"), currentTime, title);
    fclose(logFile);

    delete[] currentTime;    
}


