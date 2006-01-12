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

#ifndef INCL_MANAGEMENT_NODE
#define INCL_MANAGEMENT_NODE

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "base/util/ArrayList.h"
#include "spdm/constants.h"

/*
 * This class represents a management node, so that a configuration
 * object under the device manager control.
 * This is an abstract class that defines the interface of platform
 * specific concrete implementations.
 *
 * See the design documents for more information.
 */
class ManagementNode : public ArrayElement {

    protected:
        wchar_t *name;
        wchar_t *context;
        //
        // Children are dinamically allocated inside this class and given to
        // the list. The list will delete all created objects at descruction
        // time.
        //
        ArrayList children;

        /*
         * Set node attributes (name, context, fullcontext) from a Full Name string
         *
         */
        int setFullName(const wchar_t *name);

    public:

        // -------------------------------------------- Constructors & Destructors

        /*
         * Constructor.
         *
         * @param parent - a ManagementNode is usually under the context of a
         *                 parent node.
         * @param name - the node name
         *
         */
        ManagementNode(const wchar_t* parent, const wchar_t* name) EXTRA_SECTION_02;
        /*
         * Constructor.
         *
         * @param fullcontext - the complete path to the node. The last
         *                      component is used as name, the rest as context
         *
         */
        ManagementNode(const wchar_t* fullcontext) EXTRA_SECTION_02;

        /* Base class destructor */
        virtual ~ManagementNode() EXTRA_SECTION_02;


        // ----------------------------------------------------- Virtual methods

        /*
         * Returns this node's child, at index specified
         *
         * @param index - the index of the child to get
         *
         * @return the node or NULL on failure.
         *         Caller MUST NOT delete the object
         */
        virtual ManagementNode * getChild(int index);

        /**
         * Add a new child to this node.
         *
         * @param child - the ManagementNode to add
         */
		virtual void addChild(ManagementNode &child);

        /*
         * Returns how many children belong to this node (how many have been added)
         */
		virtual int getChildrenCount();

        /*
         * Returns the full node name
         *
         */
        virtual wchar_t *getFullName();

        // ---------------------------------------------------- Abstract methods

        /*
         * Find how many children are defined for this node in the underlying
		 * config system.
         */
        virtual int getChildrenMaxCount() = 0;

        /* Returns the names of the children nodes, in a new-allocated
         * string array
         *
         * @return NULL on failure
         */
        virtual wchar_t **getChildrenNames() = 0;

        /*
         * Returns the value of the given property
         *
         * @param property - the property name
         *
         * @return - the property value. MUST be deleted by the caller with delete []
         */
        virtual wchar_t *getPropertyValue(const wchar_t* property) = 0;

        /*
         * Sets a property value.
         *
         * @param property - the property name
         * @param value - the property value (zero terminated string)
         */
        virtual void setPropertyValue(const wchar_t* property, const wchar_t* value) = 0;

        /*
         * Creates a new ManagementNode with the exact content of this object.
         * The new instance MUST be created with the C++ new opertator.
         */
        virtual ArrayElement* clone() = 0;

};

#endif

