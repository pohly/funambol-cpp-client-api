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

#ifndef INCL_DMTREE
    #define INCL_DMTREE

    #include "spdm/constants.h"
    #include "spdm/ManagementNode.h"

    class DMTree {

    private:

        BCHAR *root;

    protected:

        virtual bool isLeaf(const BCHAR *node);

    public:
        DMTree(const BCHAR *root);

        virtual ~DMTree();

        /*
         * Returns the management node identified by the given node pathname
         * (relative to the root management node). If the node is not found
         * NULL is returned; additional info on the error condition can be
         * retrieved calling getLastError() and getLastErrorMessage()
         *
         * The ManagementNode is created with the new operator and must be
         * discarded by the caller with the operator delete.
         */
        virtual ManagementNode* getManagementNode(const BCHAR* node);

        void setManagementNode(ManagementNode& n);

    };

#endif

