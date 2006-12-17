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

#ifndef INCL_RAW_FILE_SYNC_SOURCE
#define INCL_RAW_FILE_SYNC_SOURCE

#include "spds/FILESyncSource.h"

/**
 * In contrast to the FILESyncSource this class does not
 * wrap the file content and attributes in an XML format
 * when exchanging it with the server. Therefore it can
 * be used as a peer for a servers addressbook or calendar
 * URIs.
 *
 * The type of the items is taken from the sync source
 * type property.
 */
class RawFILESyncSource : public FILESyncSource {
  public:
    RawFILESyncSource(const WCHAR* name, SyncSourceConfig* sc);

    int addItem(SyncItem& item);
    int updateItem(SyncItem& item);

  protected:
    bool setItemData(SyncItem* syncItem);
};


#endif // INCL_RAW_FILE_SYNC_SOURCE
