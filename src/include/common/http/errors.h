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
#ifndef INCL_HTTP_ERRORS
#define INCL_HTTP_ERRORS

#define ERR_TRANSPORT_BASE              2000
#define ERR_NETWORK_INIT                ERR_TRANSPORT_BASE
#define ERR_CONNECT                     ERR_TRANSPORT_BASE+ 1
#define ERR_HOST_NOT_FOUND              ERR_TRANSPORT_BASE+ 2
#define ERR_READING_CONTENT             ERR_TRANSPORT_BASE+ 3
#define ERR_WRITING_CONTENT             ERR_TRANSPORT_BASE+ 4
#define ERR_HTTP                        ERR_TRANSPORT_BASE+50
#define ERR_HTTP_MISSING_CONTENT_LENGTH ERR_TRANSPORT_BASE+51
#define ERR_SERVER_ERROR                ERR_TRANSPORT_BASE+52

#define ERR_TRANSPORT_LAST              2999

#define ERRMSG_NETWORK_INIT T("Network initialization error")
#define ERRMSG_CONNECT T("Connection failure")
#define ERRMSG_HOST_NOT_FOUND T("Host not found")
#define ERRMSG_READING_CONTENT T("Error reading content")
#define ERRMSG_WRITING_CONTENT T("Error writing content")
#define ERRMSG_HTTP_MISSING_CONTENT_LENGTH T("Missing Content-Length header")
#define ERRMSG_SERVER_ERROR ("Server error")
#endif
