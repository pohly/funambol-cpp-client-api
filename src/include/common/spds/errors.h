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

#ifndef INCL_SPDS_ERRORS
#define INCL_SPDS_ERRORS

#define ERR_PROTOCOL_ERROR      400
#define ERR_AUTH_NOT_AUTHORIZED 401
#define ERR_AUTH_EXPIRED        402
#define ERR_NOT_FOUND           404
#define ERR_AUTH_REQUIRED       407
#define ERR_SERVER_FAILURE      500

#define ERR_DT_BASE             800
// data transformer not supported
#define ERR_DT_UNKNOWN          ERR_DT_BASE+0
// data transformer error
#define ERR_DT_FAILURE          ERR_DT_BASE+1

#define ERRMSG_SERVER_FAILURE   T("The server returned error code %d")
#define ERRMSG_DT_UNKNOWN       T("Data transformer '%s' unknown")
#define ERRMSG_DT_FAILURE       T("Data transformer error: %lx")

#endif
