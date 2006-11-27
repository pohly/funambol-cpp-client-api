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
#ifndef INCL_PALM_DEVICE_MANAGER
#define INCL_PALM_DEVICE_MANAGER


#include "base/fscapi.h"
#include "spdm/constants.h"
#include "spdm/DeviceManager.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementRecord.h"

#define DM_SYNCML_DB "S4JDM"


class PalmDeviceManager : public DeviceManager  {

    public:
        PalmDeviceManager() EXTRA_SECTION_00;
        ManagementNode* getManagementNode(const char* node) EXTRA_SECTION_00;
        void setManagementNode(ManagementNode& n) EXTRA_SECTION_00;

    private:
        /**
         * Checks if the DM database is already present on the
         * system.
         *
         * @return true if the database already exists, false otherwise
         */
        Boolean checkDatabase() EXTRA_SECTION_00;

        /**
         *  Creates the DM database.
         */
        Boolean createDatabase() EXTRA_SECTION_00;

        /**
         * Finds a record with the given full name. If found, the given
         * DeviceManagementRecord object is filled with the record data and TRUE
         * is returned. Otherwise, it returns FALSE.
         *
         * @param name the node context to search for
         * @param record where to store the record
         */
        BOOL findNodeRecord(char* name, DeviceManagementRecord& record) EXTRA_SECTION_00;

        /**
         * Finds a record with the given context. If found, the given
         * DeviceManagementRecord object is filled with the record data and the
         * record index is returned. Otherwise -1 is returned.
         */
        int findChildRecord(char* name, DeviceManagementRecord& record, unsigned int pos) EXTRA_SECTION_00;

};

#endif