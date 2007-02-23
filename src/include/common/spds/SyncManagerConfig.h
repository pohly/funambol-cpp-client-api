/*
 * Copyright (C) 2003-2007 Funambol
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
#ifndef INCL_SYNC_CONFIG
#define INCL_SYNC_CONFIG

#include "base/fscapi.h"
#include "spds/AccessConfig.h"
#include "spds/DeviceConfig.h"
#include "spds/SyncSourceConfig.h"

/**
 * This class groups the configuration information needed by the SyncManager.
 * This implementation is just a transient configuration information
 * repository; persisting configuration settings is delegated to subclasses.
 */
class SyncManagerConfig {
    protected:

        AccessConfig accessConfig;
        DeviceConfig deviceConfig;
        SyncSourceConfig* sourceConfigs;

        unsigned int sourceConfigsCount;

        virtual BOOL addSyncSourceConfig(SyncSourceConfig& sc) EXTRA_SECTION_01;

    public:
 
        SyncManagerConfig() EXTRA_SECTION_01;
        virtual ~SyncManagerConfig() EXTRA_SECTION_01;

        virtual SyncSourceConfig* getSyncSourceConfigs() EXTRA_SECTION_01;
        virtual SyncSourceConfig* getSyncSourceConfig(const char*  name, BOOL refresh = FALSE) EXTRA_SECTION_01;
        virtual SyncSourceConfig* getSyncSourceConfig(unsigned int i,    BOOL refresh = FALSE) EXTRA_SECTION_01;
		virtual BOOL setSyncSourceConfig(SyncSourceConfig& sc) EXTRA_SECTION_01;
        virtual unsigned int getSyncSourceConfigsCount() EXTRA_SECTION_01;

		virtual AccessConfig& getAccessConfig() EXTRA_SECTION_01;
		virtual void setAccessConfig(AccessConfig& ac) EXTRA_SECTION_01;

        virtual DeviceConfig& getDeviceConfig() EXTRA_SECTION_01;
		virtual void setDeviceConfig(DeviceConfig& dc) EXTRA_SECTION_01;

        BOOL isDirty() EXTRA_SECTION_01;

        /**
         * Initializes the access and device config with default values from DefaultConfigFactory.
         */
        void setClientDefaults();

        /**
         * Initializes the given source with default values from DefaultConfigFactory.
         */
        void setSourceDefaults(const char* name);
};

#endif
