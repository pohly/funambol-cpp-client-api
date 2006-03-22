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

inline int TimeFormatter(const SYSTEMTIME t, BCHAR* out) {
                
    return bsprintf(out, T("%02d%02d%02d%02d"),
                         t.wDay, t.wHour, t.wMinute, t.wSecond);
}

unsigned int time(void* unused) {
    SYSTEMTIME st;
    
    GetSystemTime(&st);
    
    // year + month
    int t = st.wYear + st.wMonth;
    
    BCHAR tttt[20];

    TimeFormatter(st, tttt);

    unsigned int l = bstrtol(tttt, NULL, 10);
    
    l += l + t;
    return l;
    
    bsprintf(tttt, T("Low %u"), l);
    LOG.debug(tttt);
    
}


