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

#include "spds/constants.h"

char* retSyncMode(SyncMode sm)
{
	switch (sm)
	{
		case SYNC_NONE:
			return T("000");
			break;
		case SYNC_TWO_WAY:
			return T("200");
			break;
		case SYNC_SLOW:
			return T("201");
			break;
		case SYNC_ONE_WAY_FROM_CLIENT:
			return T("202");
			break;
		case SYNC_REFRESH_FROM_CLIENT:
			return T("203");
			break;
		case SYNC_ONE_WAY_FROM_SERVER:
			return T("204");
			break;
		case SYNC_REFRESH_FROM_SERVER:
			return T("205");
			break;
		case SYNC_TWO_WAY_BY_SERVER:
			return T("206");
			break;
		case SYNC_ONE_WAY_FROM_CLIENT_BY_SERVER:
			return T("207");
			break;
		case SYNC_REFRESH_FROM_CLIENT_BY_SERVER:
			return T("208");
			break;
		case SYNC_ONE_WAY_FROM_SERVER_BY_SERVER:
			return T("209");
			break;
		case SYNC_REFRESH_FROM_SERVER_BY_SERVER:
			return T("210");
			break;
		default:
			return NULL;
			break;
	}
}
