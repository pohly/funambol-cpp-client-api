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
#ifndef INCL_PALM_DEVICE_MANAGEMENT_RECORD
#define INCL_PALM_DEVICE_MANAGER_RECORD

/**
 * This class represents a record of a device management entry. Since in PalmOS
 * systems there is no other storage than databases, each DM node is stored in
 * a Palm database entry. The format into which a node is stored is the
 * following:
 *
 * <nodename>\0<attr1>\0...\0<attrN>\0
 *
 * Where <nodecontex> is the context name of the node (e.g.: /PIM/spds/syncml),
 * <attr1>...<attrN> are the values of the leafs under such context. Note that
 * no node name is provided for leafs, therefore they must be positional.
 * All fields are separated by a \0 (0x00) character. All fields are considered
 * strings.
 *
 * In addition, this class provide also utility methods to retrieve
 * configuration objects (such as AccessConfig, SyncSourceConfig, ...)from an
 * internal record.
 */

#include "base/fscapi.h"
#include "spdm/AccessManagementNode.h"
#include "spdm/ManagementObject.h"
#include "client/SourceManagementNode.h"

class DeviceManagementRecord {

    public:

        // ------------------------------------------ Constructors & destructors

        /**
         * Creates a new DeviceManagementRecord object from the given data.
         *
         * @param index record index
         * @param data the content of the record
         * @param size the sice of the content
         */
        DeviceManagementRecord(int index, WCHAR* data, unsigned int size) EXTRA_SECTION_03;
        DeviceManagementRecord(int index = -1) EXTRA_SECTION_03;
        ~DeviceManagementRecord() EXTRA_SECTION_03;

        /**
         * Returns the number of fields in the record (see the class description
         * for the record structure).
         */
        unsigned int getFieldNumber() EXTRA_SECTION_03;

        /**
         * Returns the n-th fields (1 based). Returns NULL if n is greater than
         * the number of the fields in the record.
         */
        WCHAR* getField(unsigned int n) EXTRA_SECTION_03;

        /**
         * Sets the record data to the given data.
         *
         * @param data the content of the record
         * @param size the sice of the content
         */
        void setData(WCHAR* data, unsigned int size) EXTRA_SECTION_03;

        /**
         * Sets the record data to the given AccessConfig
         *
         * @param n the node
         */
        void setData(AccessManagementNode& n) EXTRA_SECTION_03;

        /**
         * Sets the record data to the given AccessConfig
         *
         * @param n the node
         */
        void setData(SourceManagementNode& n) EXTRA_SECTION_03;

        /**
         * Sets the record data to the given ManagementObject
         *
         * @param n the node
         */
        void setData(ManagementObject& n) EXTRA_SECTION_03;

        /**
         * Returns the record data
         */
        WCHAR* getData() EXTRA_SECTION_03;

        /**
         * Returns the record size
         */
        unsigned int getSize() EXTRA_SECTION_03;

        /**
         * Fills the given AccessMangementNode object with the values in this record.
         * It returns TRUE if the record was compatible an AccessConfig dm object,
         * FALSE otherwise (i.e. number of fields not matching).
         * The fields sequence is as follows:
         *
         *  1) full management node name
         *  2) user name
         *  3) password
         *  4) device id
         *  5) sync URL
         *  6) use proxy
         *  7) proxy host
         *  8) first time sync mode
         *  9) begin sync timestamp
         * 10) end sync timestamp
         */
        BOOL getAccessManagementNode(AccessManagementNode& n) EXTRA_SECTION_03;

        /**
         * Fills the given SourceMangementNode object with the values in this record.
         * It returns TRUE if the record was compatible an SyncSourceConfig dm object,
         * FALSE otherwise (i.e. number of fields not matching).
         * The fields sequence is as follows:
         *
         *  1) full management node name
         *  2) source name
         *  3) source remote URI
         *  4) source type
         *  5) source admitted sync modes
         *  6) sync mode to be used
         *  7) source last synchronization timestamp
         */
        BOOL getSourceManagementNode(SourceManagementNode& n) EXTRA_SECTION_03;

        /**
         * Fills the given ManagementObject with the values in this record.
         * It returns TRUE if the record was compatible an SyncSourceConfig dm object,
         * FALSE otherwise.
         * Fields are stored with the following convention:
         *
         * <property1>\0<value1>\0...\0<propertyN>\0<valueN>
         *
         * @param o the ManagementObject to fill
         */
        BOOL getManagementObject(ManagementObject& o) EXTRA_SECTION_03;

        void setIndex(unsigned int ndx) EXTRA_SECTION_03;
        unsigned int getIndex() EXTRA_SECTION_03;

    private:

        // -------------------------------------------------------- Private data

        WCHAR* record;
        int index;
        unsigned int size;

};

#endif