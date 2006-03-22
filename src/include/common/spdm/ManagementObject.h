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


#ifndef INCL_MANAGEMENT_OBJECT
#define INCL_MANAGEMENT_OBJECT

#include "base/util/ArrayList.h"
#include "base/util/KeyValuePair.h"
#include "spdm/constants.h"
#include "spdm/LeafManagementNode.h"
#include "spds/SyncSourceConfig.h"

/**
 * This class represents a management object. Properties are stored in an
 * ArrayList of KeyValuePair objects.
 */
class ManagementObject : public LeafManagementNode {

    public:
        // ------------------------------------------ Constructors & destructors
        ManagementObject( const BCHAR*   context,
                          const BCHAR*   name   ) EXTRA_SECTION_02;

        ~ManagementObject() EXTRA_SECTION_02;

        // ------------------------------------------------------------- Methods

        void getPropertyValue(const BCHAR* property, BCHAR* v, int size) EXTRA_SECTION_02;
        void setPropertyValue(const BCHAR* property, BCHAR* value) EXTRA_SECTION_02;

        /**
         * Returns the ManagementObject's properties as an ArrayList of
         * KeyValuePairs
         */
        ArrayList& getProperties() EXTRA_SECTION_02;

        ArrayElement* clone() EXTRA_SECTION_02;

        // -------------------------------------------------------- Data members

    private:
        ArrayList properties;
};

#endif
