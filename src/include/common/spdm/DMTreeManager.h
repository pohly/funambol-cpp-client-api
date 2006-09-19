/*
 * Copyright (C) 2005-2006 Funambol
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
#ifndef INCL_DM_TREE_MANAGER
#define INCL_DM_TREE_MANAGER

#include "base/constants.h"
#include "spdm/ManagementNode.h"

/**
 * DMTreeManager is an abstract class for which implementors have to provide a
 * concrete implementation. It provides the ability to retrieve and store
 * objects into the DM platform specific repository. Note that the object
 * returned by getManagementNode() is created with the standard C++ new operator
 * and must be deleted by the caller with the standard C++ delete operator.
 */
class DMTreeManager {
    public:
        /*
         * Returns the management node identified by the given node pathname
         * (relative to the root management node). If the node is not found
         * NULL is returned.
         *
         * The ManagementNode is created with the new operator and must be
         * discarded by the caller with the operator delete. Depending on
         * which node is given, the result is either an instance
         * of SourceManagementNode or AccessManagementNode.
         */
        virtual ManagementNode* const getManagementNode(const char*  node)=0;

        /*
         * Stores the content of the node permanently in the DMTree
         */
        virtual void setManagementNode(ManagementNode& n)=0;
};

#endif
