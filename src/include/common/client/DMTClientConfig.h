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
#include "spds/DeviceConfig.h"
#include "spds/SyncSourceConfig.h"
#include "spdm/ManagementNode.h"

class DMTree;

/**
 * This class is an extension of SyncManagerConfig that is DM tree aware; this
 * means that configuration properties are read/stored from/to the DM tree.
 */
class DMTClientConfig : public SyncManagerConfig {
    private:
        DMTree* dmt;
        ManagementNode* syncMLNode;
        ManagementNode* sourcesNode;

        void initialize();

    protected:
        BCHAR* rootContext;

        DMTClientConfig() EXTRA_SECTION_01;

        BOOL readAccessConfig(ManagementNode& n) EXTRA_SECTION_01;
        void saveAccessConfig(ManagementNode& n) EXTRA_SECTION_01;
        BOOL readDeviceConfig(ManagementNode& n) EXTRA_SECTION_01;
        void saveDeviceConfig(ManagementNode& n) EXTRA_SECTION_01;
        BOOL readSourceConfig(int i, ManagementNode& n) EXTRA_SECTION_01;
        void saveSourceConfig(int i, ManagementNode& n) EXTRA_SECTION_01;

    public:

        DMTClientConfig(const BCHAR* root) EXTRA_SECTION_01;

        ~DMTClientConfig() EXTRA_SECTION_01;

        virtual BOOL getSyncSourceConfig(
                const BCHAR* name,
                SyncSourceConfig& sc,
                BOOL refresh = FALSE) EXTRA_SECTION_01;

        virtual BOOL read() EXTRA_SECTION_01;
        virtual BOOL save() EXTRA_SECTION_01;

        /**
         * Opens the configuration backend associated with the root context.
         * Calling on an open config does nothing.
         *
         * @return TRUE for success
         */
        virtual BOOL open() EXTRA_SECTION_01;

        /**
         * Provides access to the "syncml" configuration node,
         * can be used to read/write custom configuration options.
         * Config must have been opened before.
         *
         * @return node pointer owned by config and valid while the config is open
         */
        virtual ManagementNode* getSyncMLNode() EXTRA_SECTION_01;

        /**
         * Gets number of sync source configurations, -1 if not open.
         */
        virtual int getNumSources() EXTRA_SECTION_01;

        /**
         * Get the specified sync source configuration.
         *
         * @param index    number of the requested sync source configuration
         * @return node pointer owned by config and valid while the config is open
         */
        virtual ManagementNode* getSyncSourceNode(int index) EXTRA_SECTION_01;

        /**
         * Closes the configuration backend. Frees all resources associated
         * with and invalidates all ManagementNode pointers returned by this
         * config.
         */
        virtual void close() EXTRA_SECTION_01;
};

#endif
