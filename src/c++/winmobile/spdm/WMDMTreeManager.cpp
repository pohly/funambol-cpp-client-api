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
#include "base/log.h"

#include "client/AccessManagementNode.h"
#include "client/SourceManagementNode.h"
#include "spdm/spdmutils.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"
#include "spdm/ParentManagementNode.h"
#include "spdm/WMDMTreeManager.h"


/*
 * WindowsMobile implementation of DMTreeManager
 */

/*
 * Constructor
 */
WMDMTreeManager::WMDMTreeManager() {
}

/*
 * Returns the management node identified by the given node pathname
 * (relative to the root management node). If the node is not found
 * NULL is returned; additional info on the error condition can be
 * retrieved calling getLastError() and getLastErrorMessage()
 *
 * The ManagementNode is created with the new operator and must be
 * discarded by the caller with the operator delete.
 */
ManagementNode* const WMDMTreeManager::getManagementNode(const WCHAR* node) {
    WCHAR context[DIM_MANAGEMENT_PATH];
    WCHAR leafName   [DIM_MANAGEMENT_PATH];

    wmemset(context, 0, DIM_MANAGEMENT_PATH);
    wmemset(leafName,    0, DIM_MANAGEMENT_PATH);

    getNodeName(node, leafName, DIM_MANAGEMENT_PATH);
    getNodeContext(node, context, DIM_MANAGEMENT_PATH);
    WCHAR logmsg[512];
    if (wcscmp(leafName, SYNCML) == 0) {
        wcsprintf(logmsg, TEXT("Settings..."), node);
        LOG.debug(logmsg);

        AccessManagementNode* n = new AccessManagementNode(context, SYNCML);

        DeviceManagementNode dmn ;

        if (dmn.getAccessManagementNode(*n) == FALSE) {
            LOG.error(TEXT("invalid"));
        }

        if (LOG.isLoggable(LOG_LEVEL_DEBUG)) {
            n->getAccessConfig().getSyncURL(logmsg); LOG.debug(logmsg);
            n->getAccessConfig().getUsername(logmsg); LOG.debug(logmsg);
            n->getAccessConfig().getPassword(logmsg); LOG.debug(logmsg);
        }

        return n;
    } else if (wcscmp(leafName, SOURCES) == 0) {
        //
        // This is the exact match on .../spdm/sources, which means that we
        // want the children
        //

        SourceManagementNode* s = NULL;
        ParentManagementNode* n = new ParentManagementNode(context, SOURCES);

        DeviceManagementNode dmn;

        WCHAR** children = NULL;
        int nc = dmn.getChildrenCount((WCHAR*)node);
        int i = 0;
        if (nc > 0) {
            children = new WCHAR*[nc];
        }

        dmn.getChildrenName((WCHAR*)node, children, &nc);

        for (i = 0; i < nc; i++) {

            s = new SourceManagementNode(node, children[i]);
            if (dmn.getSourceManagementNode(*s) == FALSE) {
                LOG.error(TEXT("invalid source settings"));
                continue;
            }
            if (LOG.isLoggable(LOG_LEVEL_DEBUG)) {
                s->getSourceConfig().getName(logmsg); LOG.debug(logmsg);
                s->getSourceConfig().getURI(logmsg);  LOG.debug(logmsg);
            }
            n->addChild(*s); delete s;
        }

        return n;
    }
    return NULL;
}


void WMDMTreeManager::setManagementNode(ManagementNode& n) {

    DeviceManagementNode dmn;

    LOG.info(TEXT("in setManagementNode"));

    WCHAR nodeName [DIM_MANAGEMENT_PATH];
    WCHAR context  [DIM_MANAGEMENT_PATH];
    WCHAR leafName [DIM_MANAGEMENT_PATH];

    wmemset(context,     0, DIM_MANAGEMENT_PATH);
    wmemset(leafName,    0, DIM_MANAGEMENT_PATH);

    n.getFullName(nodeName, DIM_MANAGEMENT_PATH-1);
    getNodeName(nodeName, leafName, DIM_MANAGEMENT_PATH);
    getNodeContext(nodeName, context, DIM_MANAGEMENT_PATH);

    resetError();

    if (wcscmp(leafName, SYNCML) == 0) {
        dmn.setAccessManagementNode((AccessManagementNode&)n);

    } else  {
        dmn.setSourceManagementNode((SourceManagementNode&)n);
    }

}
