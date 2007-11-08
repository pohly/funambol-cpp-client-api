/*
 * Copyright (C) 2003-2007 Funambol, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */


// Only used by WinMobile
#ifdef _WIN32_WCE

#include "base/fscapi.h"
#include "base/Log.h"

/**
 * Implementation of the unix time() function for Windows.
 *
 * @param unused not used by this implementation, callers should use always NULL
 * @return the seconds since 01/01/1970.
 */
unsigned int time(void* unused) {
    SYSTEMTIME st;
    FILETIME ft;

    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &ft);
    // Convert FILETIME to time_t
    __int64 llTmp;
    memcpy (&llTmp, &ft, sizeof (__int64));
    llTmp = (llTmp - 116444736000000000) / 10000000;
    return (time_t) llTmp;
}

/**
 * Returns the time in seconds using only the HH:mm:ss. It is useful for the transport agent
 * to understand if retry or not to send the message to the server again. If time is greater then 20 mins
 * it is not needed because the server session is already ended.
 *
 * @return the time in seconds using only the HH:mm:ss
 */

unsigned long getTime() {
    SYSTEMTIME st;
    GetSystemTime(&st);
    unsigned long t = st.wSecond + (st.wMinute * 60) +
                     (st.wHour * 60 * 60) + (st.wDay * 24 * 60 * 60);
    return t;

}

#endif  // #ifdef _WIN32_WCE


