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
#include "base/Log.h"
#include "base/util/utils.h"
#include "spdm/ManagementNode.h"


/*
 * Constructor.
 *
 * @param parent - a ManagementNode is usually under the context of a
 *                 parent node.
 * @param name - the node name
 *
 */
ManagementNode::ManagementNode(const BCHAR* parent, const BCHAR* name) {
    context = stringdup(parent);
    this->name = stringdup(name);
}

ManagementNode::ManagementNode(const BCHAR* fullname) {
    if(setFullName(fullname)){
        BCHAR msg[512];

        bsprintf(msg, T("Invalid context: %s"), fullname);
        //TODO call ErrorHandler XXX
        LOG.error(msg);
    }
}

ManagementNode::~ManagementNode() {
    if (context)
        delete [] context;
    if (name)
        delete [] name;
}

/*
 * Returns the full node name (in a new-allocated buffer)
 */
BCHAR * ManagementNode::getFullName(){
    BCHAR *ret = new BCHAR[bstrlen(context)+bstrlen(name)+2];

    bsprintf(ret, T("%s/%s"), context, name);
	return ret;
}

int ManagementNode::setFullName(const BCHAR *fullname) {
    BCHAR* p;
	int len;

    p = bstrrchr((BCHAR*)fullname, CHR('/'));

    if ( !p )
        return -1;

	len = p-fullname;
    context = stringdup(fullname, len);
	p++; len=bstrlen(fullname)-len;
	name = stringdup(p, len);
	
	return 0;
}

const BCHAR *ManagementNode::getName() {
    return name;
}

/*
 * Returns how many children belong to this node.
 *
 */
int ManagementNode::getChildrenCount() {
	return children.size();
}

/*
 * Get a child from the list
 */
ManagementNode * ManagementNode::getChild(int index) {
    return (ManagementNode *)children[index];
}

/*
 * Adds a new child. A clone of the given node is created internally, so that 
 * the caller is free to release node as soon as it wants.
 * 
 * @parent node the new node
 */
void ManagementNode::addChild(ManagementNode& node) {
	children.add(node);
}


