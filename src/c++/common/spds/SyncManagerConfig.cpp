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

SyncSourceConfig* SyncManagerConfig::getSyncSourceConfig(const char* name, BOOL refresh) {
    if ((name == NULL) || (strlen(name) == 0)) {
        return NULL;
    }

    for (unsigned int i=0; i<sourceConfigsCount; ++i) {
        if (strcmp(sourceConfigs[i].getName(), name) == 0) {
            return &sourceConfigs[i];
        }
    }

    return NULL;
}

SyncSourceConfig* SyncManagerConfig::getSyncSourceConfig(unsigned int i, BOOL refresh) {
    if (i >= sourceConfigsCount) {
        return NULL;
    }

    return &sourceConfigs[i];
}


BOOL SyncManagerConfig::setSyncSourceConfig(SyncSourceConfig& sc) {
    unsigned int i=0;
    for (i=0; i<sourceConfigsCount; ++i) {
        if (strcmp(sc.getName(), sourceConfigs[i].getName()) == 0) {
            break;
        }
    }

    if (i >= sourceConfigsCount) {
        //
        // Not found! -> add the SyncSourceConfig.
        //
        return addSyncSourceConfig(sc);
    }

    sourceConfigs[i].assign(sc);

    //dirty |= DIRTY_SYNC_SOURCE;

    return TRUE;
}


/* 
 * Add the passed SyncSourceConfig (the object is copied).
 * If SyncSourceConfig name is already present (match name) the
 * config is replaced with the given one. 
 * Otherwise it is added in the sourceConfig array.
 */
BOOL SyncManagerConfig::addSyncSourceConfig(SyncSourceConfig& sc) {
    
    unsigned int i;

    // copy array in a tmp buffer
    SyncSourceConfig* s = new SyncSourceConfig[sourceConfigsCount];
    for (i=0; i<sourceConfigsCount; i++)
        s[i].assign(sourceConfigs[i]);

    // delete old one, create new (+1 element)
    if (sourceConfigs) {
        delete [] sourceConfigs;
    }
    sourceConfigs = new SyncSourceConfig[sourceConfigsCount+1];

    // copy back.
    for (i=0; i<sourceConfigsCount; i++)
        sourceConfigs[i].assign(s[i]);
    // This is the new one.
    sourceConfigs[sourceConfigsCount].assign(sc);
    sourceConfigsCount ++;

    if (s) {
        delete [] s;
    }

    //dirty |= DIRTY_SYNC_SOURCE;
    return TRUE;
}


BOOL SyncManagerConfig::isDirty() {
    return accessConfig.getDirty();
}
