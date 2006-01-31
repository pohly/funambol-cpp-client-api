/*
 * Copyright (C) 2005-2006 Funambol
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

#include <windows.h>
#include <Winbase.h>
#include "base/startcmd.h"
#include "base/Log.h"

// retrieve the Funambol program path and set it in a static
// buffer. Return the buffer
const wchar_t *getProgramPath()
{
    static wchar_t path[MAX_PATH] = TEXT("");

    if (!path[0]) {
        SHGetSpecialFolderPath(NULL, path, CSIDL_PROGRAM_FILES , FALSE);
        wcscat(path, TEXT("/"));
        wcscat(path, PROGRAM_DIR);
        wcscat(path, TEXT("/"));
    }
    return path;
}

/**
 * Start a command in a new process and return the pid
 * or 0 in cas of error.
 */
unsigned long startcmd(const wchar_t *app, const wchar_t *cmdline) 
{
    const wchar_t *path = getProgramPath();
    PROCESS_INFORMATION procinfo;
    
    wchar_t *cmd = new wchar_t[wcslen(path)+wcslen(app)+5];
    wsprintf(cmd, TEXT("%s/%s"), path, app);

    wchar_t dbg[200];
    wsprintf(dbg, L"Running: %s %s\n", cmd, cmdline);
    LOG.error(dbg);
    if( CreateProcess( cmd, cmdline, 
                       NULL, NULL, FALSE, 0,
                       NULL, NULL, NULL, &procinfo ) ) {
        return procinfo.dwProcessId; 
    }
    else
        return 0;
}

/*
 * Return 0: process terminated
 *        1: timeout
 *       -1: no such process/invalid handle
 */
int waitProcess(unsigned long pid, time_t timeout)
{
    HANDLE phandle = OpenProcess( 0, FALSE, pid );

    if (phandle) {
        switch ( WaitForSingleObject( phandle, timeout ) ) {
            case WAIT_TIMEOUT:   return 1; break;
            case WAIT_OBJECT_0:  return 0; break;
            default:             return -1;
        }
    }
    return -1;
}
