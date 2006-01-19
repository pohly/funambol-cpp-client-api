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

// a copy of stderr before it was redirected
static int fderr = -1;

void setLogFile(const char* configLogFile, BOOL redirectStderr) {
    if (logFile) {
        fclose(logFile);
        logFile = NULL;
    }
    logFileStdout = FALSE;
    if (configLogFile != NULL) {
        if (!strcmp(configLogFile, "-")) {
            // write to stdout
            logFileStdout = TRUE;
        } else {
            logFile = fopen(configLogFile, "a+" );
        }
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
wchar_t* getCurrentTime(BOOL complete) {
	 time_t t = time(NULL);
	 struct tm *sys_time = localtime(&t);

	 wchar_t *fmtComplete = TEXT("%04d-%02d-%02d %02d:%02d:%02d");
    wchar_t *fmt         = TEXT("%02d:%02d:%02d");

    wchar_t* ret = new wchar_t [64];
    
    if (complete) {
        wsprintf(ret, fmtComplete, sys_time->tm_year, sys_time->tm_mon, sys_time->tm_mday,  
                      sys_time->tm_hour, sys_time->tm_min, sys_time->tm_sec);
    } else {
        wsprintf(ret, fmt, sys_time->tm_hour, sys_time->tm_min, sys_time->tm_sec);
    }
    return ret;
}


Log::Log(BOOL resetLog) {
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
    
    wchar_t* currentTime = NULL;

    currentTime = getCurrentTime(false);
    if (!logFileStdout && !logFile) {
        setLogFile(LOG_NAME);
    }
    
    fwprintf(logFile ? logFile : stdout,
             TEXT("%s [%s] - %s\n"), currentTime, level, msg);
    fflush(logFile);

    delete[] currentTime;
}

void Log::reset() {
    if (!logFileStdout && !logFile) {
        setLogFile(LOG_NAME);
    }
    
    if (logFile) {
        ftruncate(fileno(logFile), 0);
    }
}

