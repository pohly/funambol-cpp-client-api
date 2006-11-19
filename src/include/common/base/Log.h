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

#ifndef INCL_LOG
    #define INCL_LOG

    #include "fscapi.h"

    #define LOG_ERROR T("ERROR")
    #define LOG_INFO  T("INFO" )
    #define LOG_DEBUG T("DEBUG")

    #define LOG_NAME T("synclog.txt")

    typedef enum {
        LOG_LEVEL_NONE  = 0,
        LOG_LEVEL_INFO  = 1,
        LOG_LEVEL_DEBUG = 2
    } LogLevel;

    extern char logmsg[];

    class Log {

    private:

        void printMessage(const char*  level, const char*  msg, va_list argList);

        /*
         * Which log level is set?
         */
        LogLevel logLevel;

    public:
    
        Log(BOOL reset = FALSE, const char*  path = NULL, const char*  name = NULL);
        ~Log();
        
        void setLogPath(const char*  configLogPath);
        void setLogName(const char*  configLogName);
        
        void error(const char*  msg, ...);
        void info(const char*  msg, ...);
        void debug(const char*  msg, ...);
        void trace(const char*  msg);

        void reset(const char*  title = NULL);

        void setLevel(LogLevel level);

        LogLevel getLevel();

        BOOL isLoggable(LogLevel level);

        // FIXME!
        void printMessageW(const char* level, const WCHAR* msg, va_list argList);

    };

    extern Log LOG;
#endif
