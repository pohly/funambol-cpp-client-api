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
    #include "syncml/core/CTCap.h"

/*
 * ------------------------------- SyncSourceConfig class -----------------------------------
 * This class groups all configuration properties for a SyncSource.
 * SyncSourceConfig is a part of SyncManagerConfig (along with AccessConfig 
 * and an array of DeviceConfig).
 *
 * Class members:
 * --------------
 * name             : the SyncSource name
 * uri              : the SyncSource URI
 * syncModes        : Specifies all supported sync modes for the given source.
 *                    The form is a comma separated list of modes.
 *                    Sync modes can be one of "slow", "two-way", "one-way-server", 
 *                    "one-way-client", "refresh-from-server", "refresh-from-client", "addrchange".
 * type             : The source type used by client. This is one of types specified 
 *                    in 'supportedTypes' property.
 * sync             : This is the current sync mode used by the client. 
 *                    The parameter MUST be one of the sync modes specified in property 'syncModes'.
 * encodings        : Specifies how the content of an item should be encoded. 
 *                    The form of this parameter is a semi-column separated list of formats 
 *                    that must be applied in sequence from the leftmost to the rightmost. 
 *                    For example, if format is "des;b64", when  the item will be output in 
 *                    the message, the content must be first transformed with the “des” encoder 
 *                    and than with the b64 encoder.
 * version          : The version of the source type used by client.
 * supportedTypes   : A string rapresenting the source types (with versions) supported by the SyncSource. 
 *                    The string must be formatted as a sequence of "type:version" separated by commas ','. 
 *                    For example: "text/x-vcard:2.1,text/vcard:3.0".
 *                    The version can be left empty, for example: "text/x-s4j-sifc:".
 *                    Supported types will be sent used for the <DevInf>.
 * ctCap            : Specifies the content type capabilities for this SyncSource.
 *                    Not implemented yet. (T.B.D.)
 * last             : Long value that specifies the last timestamp for this source.
 *
 */
class SyncSourceConfig {

    protected:

        BCHAR* name          ;
        BCHAR* uri           ;
        BCHAR* syncModes     ;
        BCHAR* type          ;
        BCHAR* sync          ;
        BCHAR* encodings     ;
        BCHAR* version       ;
        BCHAR* supportedTypes;
        CTCap  ctCap         ;

        unsigned long last;

    public:

        /*
         * Constructs a new SyncSourceConfig object
         */
        SyncSourceConfig() EXTRA_SECTION_02;

        /*
         * Destructor
         */
        ~SyncSourceConfig() EXTRA_SECTION_02;

        /*
         * Returns the SyncSource name.
         */
        const BCHAR* getName() const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource name
         *
         * @param n the new name
         */
        void setName(const BCHAR* n) EXTRA_SECTION_02;


        /*
         * Returns the SyncSource URI (used in SyncML addressing).
         */
        const BCHAR* getURI() const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource URI (used in SyncML addressing).
         *
         * @param u the new uri
         */
        void setURI(const BCHAR* u) EXTRA_SECTION_02;

        /*
         * Returns a comma separated list of the possible syncModes for the
         * SyncSource.
         */
        const BCHAR* getSyncModes() const EXTRA_SECTION_02;

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
        void setSyncModes(const BCHAR* s) EXTRA_SECTION_02;

        /*
         * Returns the mime type of the items handled by the sync source.
         */
        const BCHAR* getType() const EXTRA_SECTION_02;

        /*
         * Sets the mime type of the items handled by the sync source.
         *
         * @param t the mime type
         */
        void setType(const BCHAR* t) EXTRA_SECTION_02;

        /*
         * Gets the default syncMode as one of the strings listed in setSyncModes.
         */
        const BCHAR* getSync() const EXTRA_SECTION_02;

        /*
         * Returns the default syncMode as one of the strings above.
         */
        void setSync(const BCHAR* s) EXTRA_SECTION_02;
        
        /*
         * Sets the encodings to use to format the content of the items
         * generated by the sync source..
         *
         */
        const BCHAR* getEncoding() const EXTRA_SECTION_02;
        
        /*
         * Returns the default syncMode as one of the strings above.
         */
        void setEncoding(const BCHAR* s) EXTRA_SECTION_02;


        /*
         * Returns the SyncSource version.
         */
        const BCHAR* getVersion() const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource version
         *
         * @param v the new version
         */
        void setVersion(const BCHAR* n) EXTRA_SECTION_02;


        /*
         * Returns the supported source types for this source.
         */
        const BCHAR* getSupportedTypes() const EXTRA_SECTION_02;

        /*
         * Sets the supported source types for this source.
         *
         * @param s: the supported types string:
         *           a sequence of type:version separated by commas
         *           "text/x-s4j-sifc:1.0,text/x-vcard:2.1,..."
         */
        void setSupportedTypes(const BCHAR* s) EXTRA_SECTION_02;

        CTCap getCtCap() const           EXTRA_SECTION_02;
        void setCtCap(CTCap v)           EXTRA_SECTION_02;

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

        /*
         * Assign operator
         */
        SyncSourceConfig& operator = (const SyncSourceConfig& sc) {
            assign(sc);
            return *this;
        }
    };

#endif
