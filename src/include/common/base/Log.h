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

    #define LOG_ERROR TEXT("ERROR")
    #define LOG_INFO  TEXT("INFO" )
    #define LOG_DEBUG TEXT("DEBUG")

    #define LOG_NAME TEXT("synclog.txt")

    typedef enum {
        LOG_LEVEL_NONE  = 0,
        LOG_LEVEL_INFO  = 1,
        LOG_LEVEL_DEBUG = 2
    } LogLevel;

    extern wchar_t logmsg[];

    class __declspec(dllexport) Log {

    private:

        void printMessage(const wchar_t* level, const wchar_t* msg, va_list argList);

        /*
         * Which log level is set?
         */
        LogLevel logLevel;

    public:
    
        Log(BOOL reset = FALSE, wchar_t* path = NULL, wchar_t* name = NULL);
        ~Log();
        
        void setLogPath(wchar_t* configLogPath);
        void setLogName(wchar_t* configLogName);
        
        void error(const wchar_t* msg, ...);
        void error(const wchar_t* msg, va_list argList);

        void info(const wchar_t* msg, ...);
        void info(const wchar_t* msg, va_list argList);

        void debug(const wchar_t* msg, ...);
        void debug(const wchar_t* msg, va_list argList);
        
        void trace(const wchar_t* msg);

        void reset();

        void setLevel(LogLevel level);

        LogLevel getLevel();

        BOOL isLoggable(LogLevel level);

    };

    extern Log LOG;
#endif
