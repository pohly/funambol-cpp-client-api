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


#include "base/fscapi.h"
#include "base/Log.h"

void TimeFormatter(SYSTEMTIME t, wchar_t* out);

unsigned int time(void* unused) {
    SYSTEMTIME st;
    
    GetSystemTime(&st);
    
    // year + month
    int t = st.wYear + st.wMonth;
    
    wchar_t tttt[100];
    TimeFormatter(st, tttt);

    unsigned int l = wcstol(tttt, NULL, 10);
    
    l += l + t;
    return l;
    
    wsprintf(tttt, TEXT("Low %u"), l);
    LOG.debug(tttt);
    
}
void TimeFormatter(SYSTEMTIME t, wchar_t* out) {
            
    wchar_t day   [10];
    wchar_t hour  [10];
    wchar_t minute[10];
    wchar_t second[10];
         
    // day
    if (t.wDay  < 10)
        wsprintf(day, TEXT("0%i"), t.wDay);
    else
        wsprintf(day, TEXT("%i"), t.wDay);
    
    // hour
    if (t.wHour  < 10)
        wsprintf(hour, TEXT("0%i"), t.wHour);
    else
        wsprintf(hour, TEXT("%i"), t.wHour);

    // minute
    if (t.wMinute < 10)
        wsprintf(minute, TEXT("0%i"), t.wMinute);
    else
        wsprintf(minute, TEXT("%i"), t.wMinute);

    // second
    if (t.wSecond < 10)
        wsprintf(second, TEXT("0%i"), t.wSecond);
    else
        wsprintf(second, TEXT("%i"), t.wSecond);
    
    if (out == NULL)
        out = new wchar_t[20];

    wsprintf(out, TEXT("%s%s%s%s"), day, hour, minute, second);          
        
}