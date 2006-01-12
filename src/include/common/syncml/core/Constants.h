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


#ifndef INCL_CORE_CONSTANTS
#define INCL_CORE_CONSTANTS

#include "base/fscapi.h"

#define DIM_64    64
#define DIM_512  512

#define MIMETYPE_SYNCMLDS_XML               TEXT("application/vnd.syncml+xml")
#define MIMETYPE_SYNCMLDS_WBXML             TEXT("application/vnd.syncml+wbxml")
#define MIMETYPE_SYNCML_DEVICEINFO_XML      TEXT("application/vnd.syncml-devinf+xml")
#define MIMETYPE_SYNCML_DEVICEINFO_WBXML    TEXT("application/vnd.syncml-devinf+wbxml")

#define NAMESPACE_METINF    TEXT("syncml:metinf")
#define NAMESPACE_DEVINF    TEXT("syncml:devinf")
#define FORMAT_B64          TEXT("b64")

#define AUTH_TYPE_MD5    TEXT("syncml:auth-md5"  )
#define AUTH_TYPE_BASIC  TEXT("syncml:auth-basic")
#define AUTH_NONE        TEXT("none"             )
#define AUTH_SUPPORTED_TYPES  TEXT("syncml:auth-md5,syncml:auth-basic")


#endif
