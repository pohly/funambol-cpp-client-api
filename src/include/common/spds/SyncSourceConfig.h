/*
 * Copyright (C) 2003-2007 Funambol
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
 * encodings        : Specifies how the content of an outgoing item should be encoded by the client library
 *                    if the sync source does not set an encoding itself. Valid values are listed in
 *                    SyncItem::encodings.
 * version          : The version of the source type used by client.
 * supportedTypes   : A string rapresenting the source types (with versions) supported by the SyncSource. 
 *                    The string must be formatted as a sequence of "type:version" separated by commas ','. 
 *                    For example: "text/x-vcard:2.1,text/vcard:3.0".
 *                    The version can be left empty, for example: "text/x-s4j-sifc:".
 *                    Supported types will be sent used for the <DevInf>.
 * ctCap            : Specifies the content type capabilities for this SyncSource.
 *                    Not implemented yet. (T.B.D.)
 * encryption       : Specifies if the content of an outgoing item should be ecrypted. 
 *                    If this property is not empty and valid, the 'encodings' value is ignored 
 *                    for outgoing items. The only valid value is "des".
 * last             : Long value that specifies the last timestamp for this source.
 *
 */
class SyncSourceConfig {

    protected:

        char*  name          ;
        char*  uri           ;
        char*  syncModes     ;
        char*  type          ;
        char*  sync          ;
        char*  encodings     ;
        char*  version       ;
        char*  supportedTypes;
        CTCap  ctCap         ;
        char*  encryption    ;

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
        const char*  getName() const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource name
         *
         * @param n the new name
         */
        void setName(const char*  n) EXTRA_SECTION_02;


        /*
         * Returns the SyncSource URI (used in SyncML addressing).
         */
        const char*  getURI() const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource URI (used in SyncML addressing).
         *
         * @param u the new uri
         */
        void setURI(const char*  u) EXTRA_SECTION_02;

        /*
         * Returns a comma separated list of the possible syncModes for the
         * SyncSource.
         */
        const char*  getSyncModes() const EXTRA_SECTION_02;

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
        void setSyncModes(const char*  s) EXTRA_SECTION_02;

        /*
         * Returns the mime type of the items handled by the sync source.
         */
        const char*  getType() const EXTRA_SECTION_02;

        /*
         * Sets the mime type of the items handled by the sync source.
         *
         * @param t the mime type
         */
        void setType(const char*  t) EXTRA_SECTION_02;

        /*
         * Gets the default syncMode as one of the strings listed in setSyncModes.
         */
        const char*  getSync() const EXTRA_SECTION_02;

        /*
         * Returns the default syncMode as one of the strings above.
         */
        void setSync(const char*  s) EXTRA_SECTION_02;
        
        /*
         * Sets the encodings to use to format the content of the items
         * generated by the sync source..
         *
         */
        const char*  getEncoding() const EXTRA_SECTION_02;
        void setEncoding(const char*  s) EXTRA_SECTION_02;


        /*
         * Returns the SyncSource version.
         */
        const char*  getVersion() const EXTRA_SECTION_02;

        /*
         * Sets the SyncSource version
         *
         * @param v the new version
         */
        void setVersion(const char*  n) EXTRA_SECTION_02;


        /*
         * Returns the supported source types for this source.
         */
        const char*  getSupportedTypes() const EXTRA_SECTION_02;

        /*
         * Sets the supported source types for this source.
         *
         * @param s: the supported types string:
         *           a sequence of type:version separated by commas
         *           "text/x-s4j-sifc:1.0,text/x-vcard:2.1,..."
         */
        void setSupportedTypes(const char*  s) EXTRA_SECTION_02;

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
        
         /*
         * Returns the encryption type
         */
        const char* getEncryption() const EXTRA_SECTION_02;
        
        /*
         * Sets the encryption type
         *
         * @param n the encryption type
         */
        void setEncryption(const char* n) EXTRA_SECTION_02;

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
