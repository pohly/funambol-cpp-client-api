/*
 * Copyright (C) 2003-2007 Funambol
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



#ifndef INCL_PARENT_MANAGEMENT_NODE
    #define INCL_PARENT_MANAGEMENT_NODE
/** @cond DEV */

    #include "base/fscapi.h"
    #include "base/util/ArrayList.h"
    #include "spdm/constants.h"
    #include "spdm/ManagementNode.h"

    /*
     * This class represents a management node with children. It gives a way to
     * handle children nodes. It currently ignores node's properties.
     *
     * See the design documents for more information.
     */
    class __declspec(dllexport) ParentManagementNode : public ManagementNode {

    protected:
        //
        // Children are dinamically allocated inside this class and given to
        // the list. The list will delete all created objects at descruction
        // time.
        //
        ArrayList children;

    public:
        /*
         * Constructor.
         *
         * @param parent - a ManagementNode is usually under the context of a
         *                 parent node.
         * @param name - the node name
         *
         */
        ParentManagementNode(char*  context, char*  name) EXTRA_SECTION_02;
        ~ParentManagementNode() EXTRA_SECTION_02;


        // ----------------------------------------------------- Virtual methods

        /*
         * It always returns an empty string
         *
         * @param property - the property name
         * @param buf - the buffer for the value
         * @param len - the buffer size
         */
        void getPropertyValue(const char*  property, const char* buf, int size) EXTRA_SECTION_02;


        /*
         * It simply ignores the new value
         *
         * @param property - the property name
         * @param value - the property value (zero terminated string)
         */
        void setPropertyValue(const char*  property, const char* value) EXTRA_SECTION_02;

        /*
         * Returns this node's children.
         *
         * The ManagementNode objects are created with the new operator and
         * must be discarded by the caller with the operator delete.
         *
         * @param children - the buffer where ManagementNode* must be stored
         * @param size - the size of the children buffer (number of ManagementNode*) in
         *               input; the number of children in output
         *
         */
        void getChildren(ManagementNode** children, int* size) EXTRA_SECTION_02;

        ManagementNode* get(int i) EXTRA_SECTION_02;

        /*
         * Returns how many children belong to this node.
         *
         */
        int getChildrenCount() EXTRA_SECTION_02;

        /*
         * Adds a new child.
         *
         * @parent node the new node
         */
        void addChild(ManagementNode& node) EXTRA_SECTION_02;

        /*
         * Creates a new ManagementNode with the exact content of this object.
         * The new instance MUST be created with the C++ new opertator.
         */
        ArrayElement* clone() EXTRA_SECTION_02;

        /**
         * Returns the i-th child of this parent or NULL if i is out of bound.
         *
         * @param i the child position (0-based)
         */
        ManagementNode* operator[] (int i) EXTRA_SECTION_02;


        // -------------------------------------------------------- Private data
    };

/** @endcond */
#endif
