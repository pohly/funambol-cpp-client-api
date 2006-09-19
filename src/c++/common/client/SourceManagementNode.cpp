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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "base/fscapi.h"
#include "base/constants.h"
#include "base/util/utils.h"
#include "spdm/constants.h"
#include "client/SourceManagementNode.h"


SourceManagementNode::SourceManagementNode(const char*    context,
                                           const char*    name   )
    : DeviceManagementNode(context, name) {
}

SourceManagementNode::SourceManagementNode(const char*     context,
                                           const char*     name   ,
                                           SyncSourceConfig& c      )
    : DeviceManagementNode(context, name) {
    config.assign(c);
}

SourceManagementNode::~SourceManagementNode() {}


void SourceManagementNode::getPropertyValue(const char*  property, char*  v, int size) {
    if (property == NULL) {
        return;
    }

    if (strcmp(property, PROPERTY_SOURCE_NAME) == 0) {
        config.getName(v);
    } else if (strcmp(property, PROPERTY_SOURCE_URI)==0) {
        config.getURI(v);
    } else if (strcmp(property, PROPERTY_SOURCE_SYNC_MODES)==0) {
        config.getSyncModes(v);
    } else if (strcmp(property, PROPERTY_SOURCE_SYNC)==0) {
        config.getSync(v);
    } else if (strcmp(property, PROPERTY_SOURCE_LAST_SYNC)==0) {
        timestampToAnchor(config.getLast(), v);
    } else if (strcmp(property, PROPERTY_SOURCE_TYPE)==0) {
        config.getType(v);
    }
}


void SourceManagementNode::setPropertyValue(const char*  property, const char*  value) {

}

SyncSourceConfig& SourceManagementNode::getSourceConfig() {
    return config;
}

void SourceManagementNode::setSourceConfig(SyncSourceConfig& c) {
    config.assign(c);
}

ArrayElement* SourceManagementNode::clone()  {
    return new SourceManagementNode(context, name, config);
}
