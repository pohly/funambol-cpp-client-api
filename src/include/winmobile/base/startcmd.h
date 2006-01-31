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

#ifndef INCL_STARTCMD_H
#define INCL_STARTCMD_H

// Funambol default program dir
#define PROGRAM_DIR TEXT("Funambol")
// Startsync application name
#define SYNCAPP TEXT("startsync.exe")

const wchar_t *getProgramPath();
unsigned long startcmd(const wchar_t *app, const wchar_t *cmdline) ;
int waitProcess(unsigned long pid, time_t timeout);

#endif

