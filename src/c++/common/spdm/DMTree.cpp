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
#include "base/util/utils.h"
#include "base/Log.h"

#include "spdm/spdmutils.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"
#include "spdm/DMTree.h"


/*
 * Basic implementation of DMTree, can be re-defined if platform specific 
 * variant is needed.
 */

/*
 * Constructor
 */
DMTree::DMTree(const wchar_t *rootContext) {
    root = stringdup(rootContext);
}

/*
 * Destructor
 */
DMTree::~DMTree() {
    if (root)
        delete [] root;
}

bool DMTree::isLeaf(const wchar_t *node) {
    DeviceManagementNode dmn(node);
    
    return (dmn.getChildrenMaxCount() == 0);
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
ManagementNode* DMTree::getManagementNode(const wchar_t* node) {

    LOG.debug(node);
    
    ManagementNode *n = new DeviceManagementNode(node);

    int childrenCount = n->getChildrenMaxCount();

    if (childrenCount) {
        wchar_t** childrenNames = n->getChildrenNames();
		
		if (!childrenNames){
			LOG.error(TEXT("Error in getChildrenNames"));
			return NULL;
		}

        for (int i = 0; i < childrenCount; i++) {
            DeviceManagementNode s(node, childrenNames[i]);
            n->addChild(s);
        }
    }

    return n;
}

/*
void DMTree::setManagementNode(ManagementNode& n) {

    DeviceManagementNode dmn;

    LOG.info(TEXT("in setManagementNode"));

    wchar_t nodeName [DIM_MANAGEMENT_PATH];
    wchar_t context  [DIM_MANAGEMENT_PATH];
    wchar_t leafName [DIM_MANAGEMENT_PATH];

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
*/


