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



#ifndef INCL_SYNC_SOURCE_CONFIG
    #define INCL_SYNC_SOURCE_CONFIG

    #include "base/fscapi.h"
    #include "spds/constants.h"

    class __declspec(dllexport) SyncSourceConfig {

    protected:

        wchar_t* name     ;
        wchar_t* uri      ;
        wchar_t* syncModes;
        wchar_t* type     ;
        wchar_t* sync     ;

        unsigned long last;

    public:

        /*
         * Constructs a new SyncSourceConfig object
         */
        SyncSourceConfig() EXTRA_SECTION_02;

        /*
         * Constructs a new SyncSourceConfig object from an other.
         */
        SyncSourceConfig(SyncSourceConfig& c) EXTRA_SECTION_02;

        /*
         * Destructor
         */
        ~SyncSourceConfig() EXTRA_SECTION_02;

        /*
         * Returns the SyncSource name.
         */
        const wchar_t* getName(wchar_t* buf = NULL) const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource name
         *
         * @param n the new name
         */
        void setName(const wchar_t* n) EXTRA_SECTION_02;


        /*
         * Returns the SyncSource URI (used in SyncML addressing).
         */
        const wchar_t* getURI(wchar_t* buf = NULL) const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource URI (used in SyncML addressing).
         *
         * @param u the new uri
         */
        void setURI(const wchar_t* u) EXTRA_SECTION_02;

        /*
         * Returns a comma separated list of the possible syncModes for the
         * SyncSource.
         */

        const wchar_t* getSyncModes(wchar_t* buf = NULL) const EXTRA_SECTION_02;

        /*
         * Sets the available syncModes for the SyncSource as comma separated
         * values. Each value must be one of:
         *  . none
         *  . slow
         *  . two-way
         *  . one-way
         *  . refresh
         *
         * @param s the new list
         */
        void setSyncModes(const wchar_t* s) EXTRA_SECTION_02;

        /*
         * Returns the mime type of the items handled by the sync source.
         * If type is null, the internal buffer is returned, otherwise the
         * value is copied into the given buffer (that must be DIM_SYNC_MIME_TYPE
         * big).
         *
         * @param t the buffer were the mime type will be copied into (if not null)
         */
        const wchar_t* getType(wchar_t* buf = NULL) const EXTRA_SECTION_02;

        /*
         * Sets the mime type of the items handled by the sync source.
         *
         * @param t the mime type
         */
        void setType(const wchar_t* t) EXTRA_SECTION_02;

        /*
         * Sets the default syncMode as one of the strings listed in setSyncModes.
         *
         * @param s the sync mode
         */
        const wchar_t* getSync(wchar_t* buf = NULL) const EXTRA_SECTION_02;

        /*
         * Returns the default syncMode as one of the strings above.
         */
        void setSync(const wchar_t* s) EXTRA_SECTION_02;

        /*
         * Sets the last sync timestamp
         *
         * @param timestamp the last sync timestamp
         */
        void setLast(unsigned long timestamp) EXTRA_SECTION_02;

        /*
         * Returns the last sync timestamp
         */
        unsigned long getLast() const EXTRA_SECTION_02;

        /**
         * Initialize this object with the given SyncSourceConfig
         *
         * @pram sc the source config object
         */
        void assign(const SyncSourceConfig& sc);

    };

#endif
