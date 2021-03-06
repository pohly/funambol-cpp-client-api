/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */


// Only used by WinMobile
#ifdef _WIN32_WCE

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/globalsdef.h"

USE_NAMESPACE

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


