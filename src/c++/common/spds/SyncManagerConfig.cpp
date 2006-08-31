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

#include "base/fscapi.h"
#include "base/debug.h"
#include "base/errors.h"
#include "base/Log.h"
#include "spds/SyncManagerConfig.h"


SyncManagerConfig::SyncManagerConfig() {
    sourceConfigs = NULL;
    sourceConfigsCount = 0;
}

SyncManagerConfig::~SyncManagerConfig() {
    if (sourceConfigs) {
        delete [] sourceConfigs;
    }
}

SyncSourceConfig* SyncManagerConfig::getSyncSourceConfigs() {
    return sourceConfigs;
}

unsigned int SyncManagerConfig::getSyncSourceConfigsCount() {
    return sourceConfigsCount;
}

AccessConfig& SyncManagerConfig::getAccessConfig() {
    return accessConfig;
}

void SyncManagerConfig::setAccessConfig(AccessConfig& ac) {
    accessConfig.assign(ac);
}

DeviceConfig& SyncManagerConfig::getDeviceConfig() {
    return deviceConfig;
}

void SyncManagerConfig::setDeviceConfig(DeviceConfig& dc) {
    deviceConfig.assign(dc);
}

BOOL SyncManagerConfig::getSyncSourceConfig(const BCHAR* name, SyncSourceConfig& sc, BOOL refresh) {
    if ((name == NULL) || (bstrlen(name) == 0)) {
        return FALSE;
    }

    for (unsigned int i=0; i<sourceConfigsCount; ++i) {
        if (bstrcmp(sourceConfigs[i].getName(), name) == 0) {
            sc = sourceConfigs[i];
            return TRUE;
        }
    }

    return FALSE;
}

BOOL SyncManagerConfig::getSyncSourceConfig(unsigned int i, SyncSourceConfig& sc, BOOL refresh) {
    if (i >= sourceConfigsCount) {
        return FALSE;
    }

    sc = sourceConfigs[i];
    return TRUE;
}


BOOL SyncManagerConfig::setSyncSourceConfig(SyncSourceConfig& sc) {
    unsigned int i=0;
    for (i=0; i<sourceConfigsCount; ++i) {
        if (bstrcmp(sc.getName(), sourceConfigs[i].getName()) == 0) {
            break;
        }
    }

    if (i >= sourceConfigsCount) {
        //
        // Not found!
        //
        return FALSE;
    }

    sourceConfigs[i].assign(sc);

    //dirty |= DIRTY_SYNC_SOURCE;

    return TRUE;
}


BOOL SyncManagerConfig::isDirty() {
    return accessConfig.getDirty();
}

BOOL SyncManagerConfig::isEncryption() {
    return accessConfig.getEncryption();
}

