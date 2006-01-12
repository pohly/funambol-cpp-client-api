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
#include "base/constants.h"
#include "base/util/utils.h"
#include "spdm/constants.h"
#include "client/AccessManagementNode.h"


AccessManagementNode::AccessManagementNode(const wchar_t*   context,
                                           const wchar_t*   name   )
    : DeviceManagementNode((wchar_t*)context, (wchar_t*)name) {
}

AccessManagementNode::AccessManagementNode(const wchar_t*   context,
                                           const wchar_t*   name   ,
                                           AccessConfig&    c      )
    : DeviceManagementNode((wchar_t*)context, (wchar_t*)name) {
    config.assign(c);
}


AccessManagementNode::~AccessManagementNode() {
}


void AccessManagementNode::getPropertyValue(const wchar_t* property, wchar_t* v, int size) {
    if (property == NULL) {
        return;
    }

    if (wcscmp(property, PROPERTY_SYNC_URL) == 0) {
        config.getSyncURL(v);
    } else if (wcscmp(property, PROPERTY_USERNAME)==0) {
        config.getUsername(v);
    } else if (wcscmp(property, PROPERTY_PASSWORD)==0) {
        config.getPassword(v);
    } else if (wcscmp(property, PROPERTY_DEVICE_ID)==0) {
        config.getDeviceId(v);
    } else if (wcscmp(property, PROPERTY_SYNC_BEGIN)==0) {
        timestampToAnchor(config.getBeginSync(), v);
    } else if (wcscmp(property, PROPERTY_SYNC_END)==0) {
        timestampToAnchor(config.getEndSync(), v);
    } else if (wcscmp(property, PROPERTY_SERVER_NONCE)==0) {
        config.getServerNonce(v);
    } else if (wcscmp(property, PROPERTY_CLIENT_NONCE)==0) {
        config.getClientNonce(v);
    } else if (wcscmp(property, PROPERTY_SERVER_ID)==0) {
        config.getServerID(v);
    } else if (wcscmp(property, PROPERTY_SERVER_PWD)==0) {
        config.getServerPWD(v);
    } else if (wcscmp(property, PROPERTY_CLIENT_AUTH_TYPE)==0) {
        config.getClientAuthType(v);
    } else if (wcscmp(property, PROPERTY_SERVER_AUTH_TYPE)==0) {
        config.getServerAuthType(v);
    } else if (wcscmp(property, PROPERTY_MAX_MSG_SIZE)==0) {
        timestampToAnchor(config.getMaxMsgSize(),v);
    } else if (wcscmp(property, PROPERTY_MAX_MOD_PER_MSG)==0) {
        timestampToAnchor(config.getMaxModPerMsg(),v);
    }

}


void AccessManagementNode::setPropertyValue(const wchar_t* property, wchar_t* value) {
    if (property == NULL) {
        return;
    }

    if (wcscmp(property, PROPERTY_SYNC_URL)==0) {
        config.setSyncURL(value);
    } else if (wcscmp(property, PROPERTY_USERNAME)==0) {
        config.setUsername(value);
    } else if (wcscmp(property, PROPERTY_PASSWORD)==0) {
        config.setPassword(value);
    } else if (wcscmp(property, PROPERTY_DEVICE_ID)==0) {
        config.setDeviceId(value);
    } else if (wcscmp(property, PROPERTY_SYNC_BEGIN)==0) {
        config.setBeginSync((unsigned long) wcstol(value, NULL, 10));
    } else if (wcscmp(property, PROPERTY_SYNC_END)==0) {
        config.setEndSync((unsigned long) wcstol(value, NULL, 10));
    }
}


AccessConfig& AccessManagementNode::getAccessConfig() {
    return config;
}

void AccessManagementNode::setAccessConfig(AccessConfig& c) {
    config.assign(c);
}

ArrayElement* AccessManagementNode::clone() {
    return new AccessManagementNode(context, name, config);
}
