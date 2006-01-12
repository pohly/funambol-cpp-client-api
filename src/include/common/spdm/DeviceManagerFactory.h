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

#ifndef INCL_DEVICE_MANAGER_FACTORY
    #define INCL_DEVICE_MANAGER_FACTORY

    #include "spdm/DeviceManager.h"

    /*
     * This class is a factory of DeviceManager objects. A concrete implementation
     * must create and return a DeviceManager suitable to work with the platform
     * where the API is used.
     */
    class DeviceManagerFactory {

    public:
        /*
         * Constructor.
         */
        DeviceManagerFactory() EXTRA_SECTION_02;

        /*
         * Creates and returns a new DeviceManager. The DeviceManager object is create
         * with the new operator and must be deleted by the caller with the operator
         * delete
         */
        DeviceManager *getDeviceManager()  EXTRA_SECTION_02;

    };

#endif
