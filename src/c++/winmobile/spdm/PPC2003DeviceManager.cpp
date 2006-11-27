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

#include "spdm/common/Utils.h"
#include "spdm/common/ManagementNode.h"
#include "spdm/ppc2003/PPC2003DeviceManager.h"
#include "spdm/ppc2003/DeviceManagementNode.h"

#include "client/common/AccessManagementNode.h"
#include "spdm/common/ParentManagementNode.h"
#include "client/common/SourceManagementNode.h"

/*
 * PPC2003 implementation of DeviceManager
 */

/*
 * Constructor
 */
PPC2003DeviceManager::PPC2003DeviceManager() : DeviceManager() {
    return;
}


/*
 * Returns the root management node for the DeviceManager.
 *
 * The ManagementNode is created with the new operator and must be
 * discarded by the caller with the operator delete.
 */
//ManagementNode* Win32DeviceManager::getRootManagementNode() {
//    return new Win32ManagementNode(TEXT("/"), TEXT(""));
//}

/*
 * Returns the management node identified by the given node pathname
 * (relative to the root management node). If the node is not found
 * NULL is returned; additional info on the error condition can be
 * retrieved calling getLastError() and getLastErrorMessage()
 *
 * The ManagementNode is created with the new operator and must be
 * discarded by the caller with the operator delete.
 */
ManagementNode* PPC2003DeviceManager::getManagementNode(const WCHAR*node) {
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
        int nc = dmn.getChildrenCount(node);
        int i = 0;
        if (nc > 0) {
            children = new WCHAR*[nc];    
        }
        
        dmn.getChildrenName(node, children, &nc);
        
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


void PPC2003DeviceManager::setManagementNode(ManagementNode& n) {
   
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
