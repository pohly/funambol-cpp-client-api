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
#ifndef INCL_WIN32_DEVICE_MANAGEMENT_NODE
#define INCL_WIN32_DEVICE_MANAGEMENT_NODE



#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "spdm/ManagementNode.h"

class DeviceManagementNode: public ManagementNode {

    private:
        wchar_t *fullContext;

    protected:
        wchar_t * convertSlashes(const wchar_t* str);
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
        DeviceManagementNode(const wchar_t* parent, const wchar_t* name);
        DeviceManagementNode(const wchar_t* fullName);  
        ~DeviceManagementNode();

        wchar_t* getPropertyValue(const wchar_t* property);
        void setPropertyValue(const wchar_t* property, const wchar_t* value);

        wchar_t **getChildrenNames();
        
        int getChildrenMaxCount();

		ArrayElement* clone();

};

#endif
