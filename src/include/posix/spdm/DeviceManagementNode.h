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

#ifndef INCL_POSIX_DEVICE_MANAGEMENT_NODE
#define INCL_POSIX_DEVICE_MANAGEMENT_NODE

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "spdm/ManagementNode.h"


/*
 * File-based implementation of ManagementNode.
 * Each node is mapped to one file, located in
 *    $HOME/.sync4j/<node>
 * with entries of the type
 * <property>\s*=\s*<value>\n
 *
 * Comments look like:
 * \s*# <comment>
 */
class DeviceManagementNode : public ManagementNode {
    ArrayList *lines;
    BOOL modified;

    class line : public ArrayElement {
        char *str;

        public:
            line(const char *newStr = NULL) { str = NULL; setLine(newStr); }
            ~line() { free(str); }
            ArrayElement *clone() { return new line(str); }

            const char *getLine() { return str; }
            void setLine(const char *newStr) { if (str) { free(str); } str = strdup(newStr ? newStr : ""); }
    };

    // the application's working directory
    int cwdfd;

    // change into directory which holds config file,
    // creating directories if necessary
    void gotoDir();

    // return to original directory after a gotoDir()
    void returnFromDir();

    // copy content of "lines" to or from file
    void update(BOOL read);

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
        DeviceManagementNode(const wchar_t* parent, const wchar_t *leafName);
        DeviceManagementNode(const wchar_t* fullName);

        DeviceManagementNode(const DeviceManagementNode &other);
        virtual ~DeviceManagementNode();


        // ----------------------------------------------------- Virtual methods

        /*
         * Returns the value of the given property
         *
         * @param property - the property name
         */
        virtual wchar_t* getPropertyValue(const wchar_t* property);


        /*
         * Sets a property value.
         *
         * @param property - the property name
         * @param value - the property value (zero terminated string)
         */
        virtual void setPropertyValue(const wchar_t* property, const wchar_t* value);

        /*
         * Returns the children's name of the parent node. 
         */
        wchar_t **getChildrenNames();

        /*
         * Find how many children are defined for this node in the underlying
         * config system.
         */
        virtual int getChildrenMaxCount();

        /*
         * Creates a new ManagementNode with the exact content of this object.
         * The new instance MUST be created with the C++ new opertator.
         */
        virtual ArrayElement* clone();


};

#endif
