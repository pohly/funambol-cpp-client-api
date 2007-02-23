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

#include <windows.h>
#include <Winbase.h>
#include "base/fscapi.h"
#include "base/startcmd.h"
#include "base/Log.h"
#include "base/util/utils.h"

// retrieve the Funambol program path and set it in a static
// buffer. Return the buffer
const WCHAR *getProgramPath()
{
    static WCHAR path[MAX_PATH] = TEXT("");

    if (!path[0]) {
        SHGetSpecialFolderPath(NULL, path, CSIDL_PROGRAM_FILES , FALSE);
        wcscat(path, TEXT("\\"));
        wcscat(path, PROGRAM_DIR);
    }
    return path;
}

/**
 * Start a command in a new process and return the pid
 * or 0 in case of error.
 */
unsigned long startcmd(const WCHAR *app, const WCHAR *cmdline) 
{
    const WCHAR *path = getProgramPath();
    PROCESS_INFORMATION procinfo;
    
    WCHAR *cmd = new WCHAR[wcslen(path)+wcslen(app)+5];
    wsprintf(cmd, TEXT("%s\\%s"), path, app);

    char dbg[200];
    sprintf(dbg, "Running: %ls %ls\n", cmd, cmdline);
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
 * Return 0: process terminated successfully
 *        >0: error code from child process    
 *       -1: no such process/invalid handle
 *       -2: timeout
 *       -3: can't get child exit code
 */
int waitProcess(unsigned long pid, time_t timeout)
{
    HANDLE phandle = OpenProcess( 0, FALSE, pid );

    if (phandle) {
        switch ( WaitForSingleObject( phandle, timeout ) ) {
            case WAIT_TIMEOUT:
                return -2;
                break;
            case WAIT_OBJECT_0:
                DWORD exitcode;
                if ( GetExitCodeProcess(phandle, &exitcode) )
                    return exitcode;
                else
                    return -3;
                break;
            default:
                return -1;
        }
    }
    return -1;
}
