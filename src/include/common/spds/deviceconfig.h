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
#include "base/log.h"

class DeviceConfig {
    
    private:

        BCHAR*        verDTD       ;
        BCHAR*        man          ;
        BCHAR*        mod          ;
        BCHAR*        oem          ;
        BCHAR*        fwv          ;
        BCHAR*        swv          ;
        BCHAR*        hwv          ;
        BCHAR*        devID        ;
        BCHAR*        devType      ;
        BCHAR*        dsV          ;
        BOOL          utc          ;
        BOOL          loSupport    ;
        BOOL          nocSupport   ;
        LogLevel      logLevel     ;
        unsigned int  maxObjSize   ;
        BCHAR*        devInfHash   ;


        /**
         * Sets the given buffer with the given value, dealing correctly with
         * NULL values. If a NULL value is passed, the empty string is used.
         *
         * @param buf the destination buffer
         * @param v the new value (CAN BE NULL)
         */
        void set(BCHAR** buf, const BCHAR* v) EXTRA_SECTION_02;


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
        const BCHAR* getVerDTD() const          EXTRA_SECTION_02;
        void setVerDTD(const BCHAR* v)          EXTRA_SECTION_02;

        const BCHAR* getMan() const             EXTRA_SECTION_02;
        void setMan(const BCHAR* v)             EXTRA_SECTION_02;

        const BCHAR* getMod() const             EXTRA_SECTION_02;
        void setMod(const BCHAR* v)             EXTRA_SECTION_02;

        const BCHAR* getOem() const             EXTRA_SECTION_02;
        void setOem(const BCHAR* v)             EXTRA_SECTION_02;

        const BCHAR* getFwv() const             EXTRA_SECTION_02;
        void setFwv(const BCHAR* v)             EXTRA_SECTION_02;

        const BCHAR* getSwv() const             EXTRA_SECTION_02;
        void setSwv(const BCHAR* v)             EXTRA_SECTION_02;

        const BCHAR* getHwv() const             EXTRA_SECTION_02;
        void setHwv(const BCHAR* v)             EXTRA_SECTION_02;

        const BCHAR* getDevID() const           EXTRA_SECTION_02;
        void setDevID(const BCHAR* v)           EXTRA_SECTION_02;

        const BCHAR* getDevType() const         EXTRA_SECTION_02;
        void setDevType(const BCHAR* v)         EXTRA_SECTION_02;

        const BCHAR* getDsV() const             EXTRA_SECTION_02;
        void setDsV(const BCHAR* v)             EXTRA_SECTION_02;

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

        const BCHAR* getDevInfHash() const      EXTRA_SECTION_02;
        void setDevInfHash(const BCHAR *v)      EXTRA_SECTION_02;

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
