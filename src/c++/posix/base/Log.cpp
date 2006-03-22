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

static char logName[128] = LOG_NAME;
static char logPath[256] = "/tmp" ;   

// a copy of stderr before it was redirected
static int fderr = -1;


static void setLogFile(const char *path, const char* name, BOOL redirectStderr) {
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
    logFileStdout = FALSE;
    
    if (!strcmp(name, "-")) {
        // write to stdout
        logFileStdout = TRUE;
    } else {
        char *filename = new char[strlen(path) + strlen(name) + 3];
        
        sprintf(filename, "%s/%s", path, name);
        logFile = fopen(filename, "a+" );
        delete [] filename;
    }
    
    if (redirectStderr) {
        if (fderr == -1) {
            // remember original stderr
            fderr = dup(2);
        } else {
            // close redirected stderr
            close(2);
        }
        dup2(fileno(logFile), 2);
    } else {
        if (fderr != -1) {
            // restore original stderr
            dup2(fderr, 2);
        }
    }
}

/*
* return a the time to write into log file. If complete is true, it return 
* the date too, else only hours, minutes, seconds and milliseconds
*/ 
static BCHAR* getCurrentTime(BOOL complete) {
    time_t t = time(NULL);
    struct tm *sys_time = localtime(&t);

    BCHAR *fmtComplete = T("%04d-%02d-%02d %02d:%02d:%02d");
    BCHAR *fmt         = T("%02d:%02d:%02d");

    BCHAR* ret = new BCHAR [64];

    if (complete) {
        bsprintf(ret, fmtComplete, sys_time->tm_year, sys_time->tm_mon, sys_time->tm_mday,  
                sys_time->tm_hour, sys_time->tm_min, sys_time->tm_sec);
    } else {
        bsprintf(ret, fmt, sys_time->tm_hour, sys_time->tm_min, sys_time->tm_sec);
    }
    return ret;
}


//---------------------------------------------------------------------- Constructors

Log::Log(BOOL resetLog, BCHAR* path, BCHAR* name) {

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

void Log::setLogPath(BCHAR* configLogPath) {
    memset(logPath, 0, 512*sizeof(BCHAR));
    if (configLogPath != NULL) {
        bsprintf(logPath, T("%s/"), configLogPath); 
    } else {
        bsprintf(logPath, T("%s"), T("./"));
    }
    setLogFile(logPath, logName, false);
}

void Log::setLogName(BCHAR* configLogName) {
    
    memset(logName, 0, 128*sizeof(BCHAR));
    if (configLogName != NULL) {
        bsprintf(logName, T("%s"), configLogName); 
    }
    else {
        bsprintf(logName, T("%s"), LOG_NAME);         
    }
    setLogFile(logPath, logName, false);
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
    
    wchar_t* currentTime = NULL;

    currentTime = getCurrentTime(false);
    if (!logFileStdout && !logFile) {
        setLogName(LOG_NAME);
    }
    FILE *out = logFile ? logFile : stdout ;
    fprintf(out, "%s [%s] - ", currentTime, level );
    vfprintf(out, msg, argList);
	fputs("\n", logFile); 
    fflush(out);

    delete[] currentTime;
}

// This is used by clients to log using wchar_t. Redirected to printmessage for
// posix.
void Log::printMessageW(const BCHAR* level, const BCHAR* msg, va_list argList)
{       printMessage(level, msg, argList); }

void Log::reset(const BCHAR* title) {
    if (!logFileStdout && !logFile) {
        setLogName(LOG_NAME);
    }
    
    if (logFile) {
        ftruncate(fileno(logFile), 0);
    }
}

