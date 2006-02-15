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

BOOL SyncManagerConfig::getSyncSourceConfig(const wchar_t* name, SyncSourceConfig& sc, BOOL refresh) {
    if ((name == NULL) || (wcslen(name) == 0)) {
        return FALSE;
    }

    for (unsigned int i=0; i<sourceConfigsCount; ++i) {
        if (wcscmp(sourceConfigs[i].getName(), name) == 0) {
            sc.assign(sourceConfigs[i]);
            return TRUE;
        }
    }

    return FALSE;
}


BOOL SyncManagerConfig::setSyncSourceConfig(SyncSourceConfig& sc) {
    unsigned int i=0;
    for (i=0; i<sourceConfigsCount; ++i) {
        if (wcscmp(sc.getName(), sourceConfigs[i].getName()) == 0) {
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

