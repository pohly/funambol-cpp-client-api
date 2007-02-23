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
#ifndef INCL_DEF_CONFIG
#define INCL_DEF_CONFIG

#include "base/fscapi.h"
#include "spds/AccessConfig.h"
#include "spds/DeviceConfig.h"
#include "spds/SyncSourceConfig.h"
#include "spds/SyncManagerConfig.h"

/**
 * This class is factory for other Config objects.
 * It can be inherited to define client specific Config parameters.
 */
class DefaultConfigFactory {

    public:
 
        DefaultConfigFactory()          EXTRA_SECTION_01;
        virtual ~DefaultConfigFactory() EXTRA_SECTION_01;

        /*
         * Returns a default generated AccessConfig.
         * This method can be overrided to get a default client-specific 
         * AccessConfig. 
         * @return: AccessConfig pointer allocated new, so it must 
         *          be freed by the caller.
         */
        static AccessConfig*     getAccessConfig() EXTRA_SECTION_01;

        /*
         * Returns a default generated DeviceConfig.
         * This method should be overrided to get a default client-specific 
         * DeviceConfig. 
         * @return: DeviceConfig pointer allocated new, so it must 
         *          be freed by the caller.
         */
        static DeviceConfig*     getDeviceConfig() EXTRA_SECTION_01;

        /*
         * Returns a default generated SyncSourceConfig.
         * This method should be overrided to get a default client-specific 
         * SyncSourceConfig. 
         * @return: SyncSourceConfig pointer allocated new, so it must 
         *          be freed by the caller.
         */
        static SyncSourceConfig* getSyncSourceConfig(const char*  name) EXTRA_SECTION_01;
};

#endif
