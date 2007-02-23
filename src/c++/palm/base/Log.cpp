/*
 * Copyright (C) 2003-2007 Funambol
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


#include <StringMgr.h>

#include "base/Log.h"
#include "base/util/utils.h"
#include "sps/DB.h"

Log LOG = Log(false);

WCHAR logmsg[512];

Log::Log(BOOL resetLog) {
    if (resetLog) {
        reset();
    }
}

Log::~Log() {
}

void Log::error(const WCHAR* msg) {
    printMessage(LOG_ERROR, msg);
}

void Log::info(const WCHAR* msg) {
    if (logLevel >= LOG_LEVEL_INFO) {
        printMessage(LOG_INFO, msg);
    }
}

void Log::debug(const WCHAR* msg) {
    if (logLevel >= LOG_LEVEL_DEBUG) {
        printMessage(LOG_DEBUG, msg);
    }
}

/**
 * This function differs from the others since it is intended to output
 * directly in the device debugger. Note that maybe some devices do not have
 * such feature.
 */
void Log::trace(const WCHAR* msg) {
    WCHAR* m = new WCHAR[wcslen(msg) + 10];
    wcsprintf(m, "%s\n", msg);
    DbgMessage(m);
    delete [] m;
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

void Log::reset() {
    DB db = DB(LOG_NAME);
    db.destroy();
    db.create();
}

void Log::printMessage(const WCHAR* level, const WCHAR* msg) {
    WCHAR* m = new WCHAR[wcslen(level) + wcslen(msg) + 20];

    wcsprintf(m, TEXT("%s - %s\n"), level, msg);

    DB db = DB(LOG_NAME);
    Err err = db.find();
    if (err == dmErrCantFind) {
        db.create();
    }

    if (db.openByName() == errNone) {
        db.appendRecord(m, wcslen(m)+1);
    }

    delete [] m;

    db.close();
}