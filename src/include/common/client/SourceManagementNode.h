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

#ifndef INCL_SOURCE_MANAGEMENT_NODE
#define INCL_SOURCE_MANAGEMENT_NODE

#include "spdm/constants.h"
#include "spdm/DeviceManagementNode.h"
#include "spds/SyncSourceConfig.h"


class SourceManagementNode : public DeviceManagementNode {

    public:
        // ------------------------------------------ Constructors & destructors
        SourceManagementNode( const char*    context,
                              const char*    name   ) EXTRA_SECTION_02;

        SourceManagementNode( const char*     context,
                              const char*     name   ,
                              SyncSourceConfig& config ) EXTRA_SECTION_02;

        ~SourceManagementNode() EXTRA_SECTION_02;

        // ------------------------------------------------------------- Methods

        void getPropertyValue(const char*  property, char*  v, int size) EXTRA_SECTION_02;
        void setPropertyValue(const char*  property, const char*  value) EXTRA_SECTION_02;

        ArrayElement* clone() EXTRA_SECTION_02;

        SyncSourceConfig& getSourceConfig() EXTRA_SECTION_02;
        void setSourceConfig(SyncSourceConfig& c) EXTRA_SECTION_02;

        // -------------------------------------------------------- Data members

    private:
        SyncSourceConfig config;
};

#endif
