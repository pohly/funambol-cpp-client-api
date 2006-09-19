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

#ifndef INCL_WINMOBILE_DEVICE_MANAGEMENT_NODE
#define INCL_WINMOBILE_DEVICE_MANAGEMENT_NODE


#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "spdm/ManagementNode.h"

/*
 * Windows Mobile implementation of ManagementNode.
 */
class DeviceManagementNode: public ManagementNode {

    private:
        TCHAR *fullContext;

    protected:
        void setFullContext(); 

    public:

        // ------------------------------------------ Constructors & destructors

        /**
         * Constructor.
         *
         * @param parent - a ManagementNode is usually under the context of a
         *                 parent node.
         * @param name - the node name
         *
         */
        DeviceManagementNode(const char*  parent, const char*  name);
        DeviceManagementNode(const char*  fullName);  
        ~DeviceManagementNode();

        // --------------------------------------------------- Public methods
        /*
         * Returns the value of the given property
         *
         * @param property - the property name
         */
        char*  getPropertyValue(const char*  property);

        /*
         * Sets a property value.
         *
         * @param property - the property name
         * @param value - the property value (zero terminated string)
         */
        void setPropertyValue(const char*  property, const char*  value);

        /*
         * Returns the children's name of the parent node. 
         */
        char **getChildrenNames();

        /*
         * Find how many children are defined for this node in the underlying
		 * config system.
         */
        int getChildrenMaxCount();

        /*
         * Creates a new ManagementNode with the exact content of this object.
         * The new instance MUST be created with the C++ new opertator.
         */
        ArrayElement* clone();

};

#endif
