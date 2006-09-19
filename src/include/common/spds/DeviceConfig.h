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
#ifndef INCL_DEVICE_CONFIG
#define INCL_DEVICE_CONFIG

#include "base/fscapi.h"
#include "spds/constants.h"
#include "syncml/core/VerDTD.h"
#include "base/Log.h"

/*
 * ---------------------------- DeviceConfig class -------------------------------
 * This class groups all configuration properties related to the device.
 * Most of DeviceConfig properties are used to generate the 
 * <DevInf> element for client capabilities.
 * DeviceConfig is a part of SyncManagerConfig (along with AccessConfig 
 * and an array of SyncSourceConfig).
 *
 * Class members:
 * --------------
 * verDTD       : Specifies the major and minor version identifier of the 
 *                Device Information DTD used in the representation 
 *                of the Device Information. The value MUST be "1.1". 
 *                This property is mandatory.
 * man          : Specifies the name of the manufacturer of the device. 
 *                This property is optional.
 * mod          : Specifies the model name or model number of the device. 
 *                This property is optional.
 * oem          : Specifies the OEM (Original Equipment Manufacturer) of the device. 
 *                This property is optional.
 * fwv          : Specifies the firmware version of the device.
 *                This property is optional.
 * swv          : Specifies the software version of the device.
 *                This property is optional.
 * hwv          : Specifies the hardware version of the device. 
 *                This property is optional.
 * devID        : Specifies the identifier of the source synchronization device. 
 *                The content information MUST specify a theoretically, 
 *                globally unique identifier. This property is mandatory.
 * devType      : Specifies the type of the source synchronization device. 
 *                Type values for this element type can be e.g. "pager", 
 *                "handheld", "pda", "phone", "smartphone", "server", "workstation". 
 *                Other values can also be specified. This property is mandatory.
 * dsV          : Specifies the implemented DS version. This property is optional.           
 * utc          : Boolean. Specifies that the device supports UTC based time. 
 *                If utc = TRUE, the server SHOULD send time in UTC format, 
 *                else MUST send in local time. Default value = TRUE.
 * loSupport    : Boolean. Specifies that the device supports handling of large objects. 
 *                Default value = FALSE.
 * nocSupport   : Boolean. Specifies that the device supports number of changes. 
 *                Default value = FALSE.
 * logLevel     : Specifies the logging level on the device. 
 *                It can be one of 0 � 1 � 2 (none, info, debug). 
 *                Default value = 1 (info).
 * maxObjSize   : Specifies the maximum object size allowed by the device.
 *                Default value = 0 (no maxObjSize set).
 * devInfHash   : This is a hash value generated from all properties that are used 
 *                for the <DevInf> element, plus the syncURL property from AccessConfig. 
 *                Initial value = "0".
 */
class DeviceConfig {
    
    private:

        char*         verDTD       ;
        char*         man          ;
        char*         mod          ;
        char*         oem          ;
        char*         fwv          ;
        char*         swv          ;
        char*         hwv          ;
        char*         devID        ;
        char*         devType      ;
        char*         dsV          ;
        BOOL          utc          ;
        BOOL          loSupport    ;
        BOOL          nocSupport   ;
        LogLevel      logLevel     ;
        unsigned int  maxObjSize   ;
        char*         devInfHash   ;


        /**
         * Sets the given buffer with the given value, dealing correctly with
         * NULL values. If a NULL value is passed, the empty string is used.
         *
         * @param buf the destination buffer
         * @param v the new value (CAN BE NULL)
         */
        void set(char** buf, const char*  v) EXTRA_SECTION_02;


    public:

        DeviceConfig() EXTRA_SECTION_02;
        DeviceConfig(DeviceConfig& s) EXTRA_SECTION_02;
        ~DeviceConfig() EXTRA_SECTION_02;


        /**
         * Methods to get/set data values.
         * -----------------------------------------------------
         * get: return the internal value.
         *      The caller MUST NOT release the memory itself.
         *
         * set: set the internal value.
         *      The given data are copied in an internal
         *      buffer so that the caller is assured that the 
         *      given address can be released after the call.
         */
        const char*  getVerDTD() const          EXTRA_SECTION_02;
        void setVerDTD(const char*  v)          EXTRA_SECTION_02;

        const char*  getMan() const             EXTRA_SECTION_02;
        void setMan(const char*  v)             EXTRA_SECTION_02;

        const char*  getMod() const             EXTRA_SECTION_02;
        void setMod(const char*  v)             EXTRA_SECTION_02;

        const char*  getOem() const             EXTRA_SECTION_02;
        void setOem(const char*  v)             EXTRA_SECTION_02;

        const char*  getFwv() const             EXTRA_SECTION_02;
        void setFwv(const char*  v)             EXTRA_SECTION_02;

        const char*  getSwv() const             EXTRA_SECTION_02;
        void setSwv(const char*  v)             EXTRA_SECTION_02;

        const char*  getHwv() const             EXTRA_SECTION_02;
        void setHwv(const char*  v)             EXTRA_SECTION_02;

        const char*  getDevID() const           EXTRA_SECTION_02;
        void setDevID(const char*  v)           EXTRA_SECTION_02;

        const char*  getDevType() const         EXTRA_SECTION_02;
        void setDevType(const char*  v)         EXTRA_SECTION_02;

        const char*  getDsV() const             EXTRA_SECTION_02;
        void setDsV(const char*  v)             EXTRA_SECTION_02;

        BOOL getUtc() const                     EXTRA_SECTION_02;
        void setUtc(BOOL v)                     EXTRA_SECTION_02;

        BOOL getLoSupport() const               EXTRA_SECTION_02;
        void setLoSupport(BOOL v)               EXTRA_SECTION_02;

        BOOL getNocSupport() const              EXTRA_SECTION_02;
        void setNocSupport(BOOL v)              EXTRA_SECTION_02;

        LogLevel getLogLevel() const            EXTRA_SECTION_02;
        void setLogLevel(LogLevel v)            EXTRA_SECTION_02;

        unsigned int getMaxObjSize() const      EXTRA_SECTION_02;
        void setMaxObjSize(unsigned int v)      EXTRA_SECTION_02;

        const char*  getDevInfHash() const      EXTRA_SECTION_02;
        void setDevInfHash(const char *v)      EXTRA_SECTION_02;

        /**
         * Sets the values of this object with with the values from the given
         * DeviceConfig source object.
         *
         * @param s: the deviceConfig reference.
         */
        void assign(const DeviceConfig& s) EXTRA_SECTION_02;

        /*
         * Assign operator
         */
        DeviceConfig& operator = (const DeviceConfig& dc) {
            assign(dc);
            return *this;
        }
};

#endif
