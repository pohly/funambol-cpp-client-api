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

#include <unistd.h>

Log LOG = Log(false);
char logmsg[512];

static FILE* logFile = NULL;
static BOOL logFileStdout = FALSE;

static char logName[1024] = LOG_NAME;
static char logPath[1024] = "/tmp" ;   

// a copy of stderr before it was redirected
static int fderr = -1;


void setLogFile(const char *path, const char* name, BOOL redirectStderr) {
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
    logFileStdout = FALSE;
    
    if (!strcmp(name, "-")) {
        // write to stdout
        logFileStdout = TRUE;
    } else if (path) {
        char *filename = new char[strlen(path) + strlen(name) + 3];
        
        sprintf(filename, "%s/%s", path, name);
        logFile = fopen(filename, "a+" );
        delete [] filename;
    } else {
        logFile = fopen(name, "a+" );
    }
    
    if (redirectStderr && logFile) {
        if (fderr == -1) {
            // remember original stderr
            fderr = dup(2);
        }
        // overwrite stderr with log file fd,
        // closing the current stderr if necessary
        dup2(fileno(logFile), 2);
    } else {
        if (fderr != -1) {
            // restore original stderr
            dup2(fderr, 2);
        }
    }
}

void setLogFile(const char* name, BOOL redirectStderr)
{
    setLogFile(0, name, redirectStderr);
}

/*
* return a the time to write into log file. If complete is true, it return 
* the date too, else only hours, minutes, seconds and milliseconds
*/ 
static char*  getCurrentTime(BOOL complete) {
    time_t t = time(NULL);
    struct tm *sys_time = localtime(&t);

    char *fmtComplete = T("%04d-%02d-%02d %02d:%02d:%02d");
    char *fmt         = T("%02d:%02d:%02d");

    char*  ret = new char [64];

    if (complete) {
        sprintf(ret, fmtComplete, sys_time->tm_year, sys_time->tm_mon, sys_time->tm_mday,  
                sys_time->tm_hour, sys_time->tm_min, sys_time->tm_sec);
    } else {
        sprintf(ret, fmt, sys_time->tm_hour, sys_time->tm_min, sys_time->tm_sec);
    }
    return ret;
}


//---------------------------------------------------------------------- Constructors

Log::Log(BOOL resetLog, char*  path, char*  name) {

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

//---------------------------------------------------------------------- Public methods

void Log::setLogPath(char*  configLogPath) {
    if (configLogPath != NULL) {
        sprintf(logPath, T("%s/"), configLogPath); 
    } else {
        sprintf(logPath, T("%s"), T("./"));
    }
}

void Log::setLogName(char*  configLogName) {
    
    if (configLogName != NULL) {
        sprintf(logName, T("%s"), configLogName); 
    }
    else {
        sprintf(logName, T("%s"), LOG_NAME);         
    }
}

void Log::error(const char*  msg, ...) {    
    va_list argList;
    va_start (argList, msg);
    printMessage(LOG_ERROR, msg, argList);    
    va_end(argList);
}

void Log::info(const char*  msg, ...) {
    if (logLevel >= LOG_LEVEL_INFO) {
        va_list argList;
	    va_start (argList, msg);
        printMessage(LOG_INFO, msg, argList);    
	    va_end(argList);

    }
}

void Log::debug(const char*  msg, ...) {
    if (logLevel >= LOG_LEVEL_DEBUG) {
	    va_list argList;
        va_start (argList, msg);
        printMessage(LOG_DEBUG, msg, argList);    
        va_end(argList);
        
    }
}

void Log::trace(const char*  msg) { 
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

void Log::printMessage(const char*  level, const char*  msg, va_list argList) {       
    
    WCHAR* currentTime = NULL;

    currentTime = getCurrentTime(false);
    if (!logFileStdout && !logFile) {
        reset();
    }
    FILE *out = logFile ? logFile : stdout ;
    fprintf(out, "%s [%s] - ", currentTime, level );
    vfprintf(out, msg, argList);
    fputs("\n", out); 
    fflush(out);

    delete[] currentTime;
}

// This is used by clients to log using WCHAR. Redirected to printmessage for
// posix.
void Log::printMessageW(const char*  level, const char*  msg, va_list argList)
{       printMessage(level, msg, argList); }

void Log::reset(const char*  title) {
    if (!logFileStdout && !logFile) {
        setLogFile(logPath, logName, false);
    }
    
    if (logFile) {
        ftruncate(fileno(logFile), 0);
    }
}

