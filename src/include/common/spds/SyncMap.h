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

 #ifndef INCL_SYNC_MAP
    #define INCL_SYNC_MAP

    #include "base/fscapi.h"
    #include "base/util/ArrayElement.h"

class SyncMap : public ArrayElement {

    private:
        wchar_t* luid;
        wchar_t* guid;


    public:
        // SyncMap() EXTRA_SECTION_01;
        SyncMap(wchar_t* guid = NULL, wchar_t* luid = NULL) EXTRA_SECTION_01;
        ~SyncMap() EXTRA_SECTION_01;

        /*
         * Returns the guid of this mapping. If guid is NULL, the internal
         * buffer address is returned, otherwise the value is copied into the
         * given buffer and its pointer is returned.
         *
         * @param guid - the buffer where the guid is copied to. It must be
         *               big enough
         */
        wchar_t* getGUID(wchar_t* guid) EXTRA_SECTION_01;


        /*
         * Returns the luid of this mapping. If luid is NULL, the internal
         * buffer address is returned, otherwise the value is copied into the
         * given buffer and its pointer is returned.
         *
         * @param luid - the buffer where the luid is copied to. It must be
         *               big enough
         */
        wchar_t* getLUID(wchar_t* luid) EXTRA_SECTION_01;

        /**
         * Sets a new value for the LUID property. The value is copied in the
         * class internal memory.
         *
         * @param luid the new value
         */
        void setLUID(wchar_t* luid) EXTRA_SECTION_01;

        /**
         * Sets a new value for the LUID property (as unsigned int). It internally
         * calls setLUID(wchar_t*)
         *
         * @param luid the new value
         */
        void setLUID(unsigned long luid) EXTRA_SECTION_01;


        /**
         * Sets a new value for the GUID property. The value is copied in the
         * class internal memory.
         *
         * @param guid the new value
         */
        void setGUID(wchar_t* guid) EXTRA_SECTION_01;

        /**
         * Creates a new instance of SyncMap from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone() EXTRA_SECTION_01;
    };

#endif
