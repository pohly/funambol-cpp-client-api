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


#ifndef INCL_DEVINF
#define INCL_DEVINF

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "syncml/core/VerDTD.h"
#include "syncml/core/DataStore.h"
#include "syncml/core/CTCap.h"
#include "syncml/core/Ext.h"


class DevInf {
    
     // ------------------------------------------------------------ Private data
    private:
        VerDTD* verDTD;
        wchar_t* man;
        wchar_t* mod;
        wchar_t* oem;
        wchar_t* fwV;
        wchar_t* swV;
        wchar_t* hwV;
        wchar_t* devID;
        wchar_t* devTyp;
        ArrayList* dataStores;  //DataStore[]
        ArrayList* ctCap;       // CTCap[]
        ArrayList* ext;         // Ext[]
        BOOL utc;
        BOOL supportLargeObjs;
        BOOL supportNumberOfChanges;      
        SyncCap* syncCap;

        void initialize();
    
    // ---------------------------------------------------------- Public data    
    public:
    
        DevInf();
        ~DevInf();

        /**
         * Creates a new DevInf object with the given parameter
         *
         * @param verDTD the DTD version - NOT NULL
         * @param man the device manufacturer
         * @param mod the device model name
         * @param oem the device OEM
         * @param fwV the device firmware version
         * @param swV the device software version
         * @param hwV the device hardware version
         * @param devID the device ID - NOT NULL
         * @param devTyp the device type - NOT NULL
         * @param dataStores the array of datastore - NOT NULL
         * @param ctCap the array of content type capability - NOT NULL
         * @param ext the array of extension element name - NOT NULL
         * @param utc is true if the device supports UTC based time
         * @param supportLargeObjs is true if the device supports handling of large objects
         * @param supportNumberOfChanges is true if the device supports number of changes
         *
         */
        DevInf(VerDTD* verDTD,
                    wchar_t* man,
                    wchar_t* mod,
                    wchar_t* oem,
                    wchar_t* fwV,
                    wchar_t* swV,
                    wchar_t* hwV,
                    wchar_t* devID,
                    wchar_t* devTyp,
                    ArrayList* dataStores,
                    ArrayList* ctCap,
                    ArrayList* ext,
                    BOOL utc,
                    BOOL supportLargeObjs,
                    BOOL supportNumberOfChanges,
                    SyncCap* syncCap);

        // ---------------------------------------------------------- Public methods
        /**
         * Gets the DTD version property
         *
         * @return the DTD version property
         */
        VerDTD* getVerDTD();

        /**
         * Sets the DTD version property
         *
         * @param verDTD the DTD version
         */
        void setVerDTD(VerDTD* verDTD);

        /**
         * Gets the device manufacturer property
         *
         * @return the device manufacturer property
         */
        wchar_t* getMan(wchar_t* retMan);

        /**
         * Sets the device manufacturer property
         *
         * @param man the device manufacturer property
         *
         */
        void setMan(wchar_t* man);

        /**
         * Gets the model name of device
         *
         * @return the model name of device
         */
        wchar_t* getMod(wchar_t* retMod);

        /**
         * Sets the device model property
         *
         * @param mod the device model property
         *
         */
        void setMod(wchar_t* mod);

        /**
         * Gets the Original Equipment Manufacturer of the device
         *
         * @return the OEM property
         */
        wchar_t* getOEM(wchar_t* retOem);

        /**
         * Sets the Original Equipment Manufacturer of the device
         *
         * @param oem the Original Equipment Manufacturer of the device
         *
         */
        void setOEM(wchar_t* oem);

        /**
         * Gets the firmware version property
         *
         * @return the firmware version property
         */
        wchar_t* getFwV(wchar_t* retFwV);

        /**
         * Sets the firmware version property
         *
         * @param fwV the firmware version property
         *
         */
        void setFwV(wchar_t* fwV);

        /**
         * Gets the software version property
         *
         * @return the software version property
         */
        wchar_t* getSwV(wchar_t* retSwV);

        /**
         * Sets the software version property
         *
         * @param swV the software version property
         *
         */
        void setSwV(wchar_t* swV);

        /**
         * Gets the hardware version property
         *
         * @return the hardware version property
         */
        wchar_t* getHwV(wchar_t* retHwv);

        /**
         * Sets the hardware version property
         *
         * @param hwV the hardware version property
         *
         */
        void setHwV(wchar_t* hwV);

        /**
         * Gets the device identifier
         *
         * @return the device identifier
         */
        wchar_t* getDevID(wchar_t* retDevID);

        /**
         * Sets the device identifier
         *
         * @param devID the device identifier
         *
         */
        void setDevID(wchar_t* devID);

        /**
         * Gets the device type
         *
         * @return the device type
         */
        wchar_t* getDevTyp(wchar_t* retDevTyp);

        /**
         * Sets the device type
         *
         * @param devTyp the device type
         *
         */
        void setDevTyp(wchar_t* devTyp);

        /**
         * Gets the array of datastore
         *
         * @return the array of datastore
         */
        ArrayList* getDataStore();

        /**
         * Sets an array of datastore
         *
         * @param dataStores an array of datastore
         *
         */
        void setDataStore(ArrayList* dataStores);
        /**
         * Gets the array of content type capability
         *
         * @return the array of content type capability
         */
        ArrayList* getCTCap();

        /**
         * Sets an array of content type capability
         *
         * @param ctCap an array of content type capability
         *
         */
        void setCTCap(ArrayList* ctCap);

        /**
         * Gets the array of extension
         *
         * @return the array of extension
         */
        ArrayList* getExt();

        /**
         * Sets an array of extensions
         *
         * @param ext an array of extensions
         *
         */
        void setExt(ArrayList* ext);

        /**
         * Gets true if the device supports UTC based time
         *
         * @return true if the device supports UTC based time
         */
        BOOL isUTC();

        /**
         * Sets the UTC property
         *
         * @param utc is true if the device supports UTC based time
         */
        void setUTC(BOOL utc);

        /**
         * Gets the Boolean value of utc
         *
         * @return true if the device supports UTC based time
         */
        BOOL getUTC();

        /**
         * Gets true if the device supports handling of large objects
         *
         * @return true if the device supports handling of large objects
         */
        BOOL isSupportLargeObjs();

        /**
         * Sets the supportLargeObjs property
         *
         * @param supportLargeObjs is true if the device supports handling of large objects
         *
         */
        void setSupportLargeObjs(BOOL supportLargeObjs);

        
        /**
         * Gets the Boolean value of supportLargeObjs
         *
         * @return true if the device supports handling of large objects
         */
        BOOL getSupportLargeObjs();

        /**
         * Gets true if the device supports number of changes
         *
         * @return true if the device supports number of changes
         */
        BOOL isSupportNumberOfChanges();

        /**
         * Sets the supportNumberOfChanges property
         *
         * @param supportNumberOfChanges is true if the device supports number of changes
         *
         */
        void setSupportNumberOfChanges(BOOL supportNumberOfChanges);        

        /**
         * Gets the Boolean value of supportNumberOfChanges
         *
         * @return true if the device supports number of changes
         */
        BOOL getSupportNumberOfChanges();

        void setSyncCap(SyncCap* syncCap);

        SyncCap* getSyncCap();

        DevInf* clone();
   
};

#endif
