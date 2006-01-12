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
#ifndef INCL_SPDM_ERRORS
#define INCL_SPDM_ERRORS

#define ERR_INVALID_CONTEXT             10000
#define ERR_SOURCE_DEFINITION_NOT_FOUND 10001
#define ERR_DM_TREE_NOT_AVAILABLE       10002

#define ERRMSG_INVALID_CONTEXT       TEXT("Invalid context: %s")
#define ERRMSG_DM_TREE_NOT_AVAILABLE TEXT("Unable to access the DM Tree")
#define ERRMSG_SOURCE_DEFINITION_NOT_FOUND TEXT("Configuration not found for source %s")

#endif
