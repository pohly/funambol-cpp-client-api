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
#ifndef INCL_DM_CONFIG
#define INCL_DM_CONFIG

#include "base/fscapi.h"
#include "http/constants.h"
#include "spdm/constants.h"
#include "spds/SyncManagerConfig.h"
#include "spds/AccessConfig.h"
#include "spds/SyncSourceConfig.h"
#include "spdm/ManagementNode.h"


/**
 * This class is an extension of SyncManagerConfig that is DM tree aware; this
 * means that configuration properties are read/stored from/to the DM tree.
 */
class DMTClientConfig : public SyncManagerConfig {
    
    protected:
        wchar_t* rootContext;

        DMTClientConfig() EXTRA_SECTION_01;

        BOOL readAccessConfig(ManagementNode& n) EXTRA_SECTION_01;
        void saveAccessConfig(ManagementNode& n) EXTRA_SECTION_01;
        BOOL readSourceConfig(int i, ManagementNode& n) EXTRA_SECTION_01; 
        void saveSourceConfig(int i, ManagementNode& n) EXTRA_SECTION_01;

    public:

        DMTClientConfig(const wchar_t* root) EXTRA_SECTION_01;

        ~DMTClientConfig() EXTRA_SECTION_01;

        BOOL getSyncSourceConfig(
                const wchar_t* name,
                SyncSourceConfig& sc,
                BOOL refresh = FALSE) EXTRA_SECTION_01;
       
        BOOL read() EXTRA_SECTION_01;
        BOOL save() EXTRA_SECTION_01;

};

#endif
