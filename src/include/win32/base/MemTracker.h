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

#ifndef INCL_WIN32_MEMTRACKER
#define INCL_WIN32_MEMTRACKER


#define MAX_LENGHT_FILE     256

#include <stdio.h>
#include "base/Log.h"
#include "base/util/utils.h"



/*
 * Class to menage memory leaks tracking.
 * Each memory allocation information is stored into 'allocList' 
 * (redefining new operator)
 * Need to set MALLOC_DEBUG into preprocessor definitions.
*/
class MemTracker {

private:

    // The list of AllocInfo elements.
    ArrayList allocList;

    // to enable-disble memory leaks tracking
    bool tracking;

public:


    MemTracker(bool useMemTracking);
    ~MemTracker();

    void addTrack(DWORD addr,  DWORD asize,  const char *fname, DWORD lnum);
    void removeTrack(DWORD addr);
    void dumpUnfreed();

    void enableMemTracker();
    void disableMemTracker();

    bool isMemTracking();

};




// Class to store alloc informations.
class AllocInfo : public ArrayElement {

public:
    DWORD	address;
    DWORD	size;
    char	file[MAX_LENGHT_FILE];
    DWORD	line;

    ArrayElement* clone();  
};


#endif