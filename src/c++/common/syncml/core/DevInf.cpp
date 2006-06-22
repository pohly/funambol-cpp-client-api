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
 
#include "base/util/utils.h"
#include "syncml/core/DevInf.h"
 
DevInf::DevInf() {
    
    initialize();

}
DevInf::~DevInf() {
    
    if(verDTD) { delete verDTD ; verDTD  = NULL; }                
    
    if(man   )  { delete [] man   ;  man    = NULL; } 
    if(mod   )  { delete [] mod   ;  mod    = NULL; } 
    if(oem   )  { delete [] oem   ;  oem    = NULL; } 
    if(fwV   )  { delete [] fwV   ;  fwV    = NULL; } 
    if(swV   )  { delete [] swV   ;  swV    = NULL; } 
    if(hwV   )  { delete [] hwV   ;  hwV    = NULL; } 
    if(devID )  { delete [] devID ;  devID  = NULL; } 
    if(devTyp)  { delete [] devTyp;  devTyp = NULL; } 
    
    if(dataStores) { dataStores->clear() ; } //delete dataStores; dataStores = NULL;}     //DataStore[]  
    if(ctCap     ) { ctCap->clear()      ; } //delete ctCap;      ctCap = NULL;     }     // CTCap[]
    if(ext       ) { ext->clear()        ; } //delete ext;        ext = NULL;       }     // Ext[]

    utc                    = FALSE;                  
    supportLargeObjs       = FALSE;     
    supportNumberOfChanges = FALSE;

}

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
DevInf::DevInf(VerDTD* verDTD,
        BCHAR* man,
        BCHAR* mod,
        BCHAR* oem,
        BCHAR* fwV,
        BCHAR* swV,
        BCHAR* hwV,
        BCHAR* devID,
        BCHAR* devTyp,
        ArrayList* dataStores,
        ArrayList* ctCap,
        ArrayList* ext,
        BOOL utc,
        BOOL supportLargeObjs,
        BOOL supportNumberOfChanges,
        SyncCap* syncCap) {
    
    initialize();

    setVerDTD(verDTD);
    setDevID (devID);
    setDevTyp(devTyp);
    setDataStore(dataStores);
    setCTCap (ctCap);
    setExt   (ext);
    setSyncCap(syncCap);
    setMan(man);
    setMod(mod);
    setOEM(oem);
    setFwV(fwV);
    setSwV(swV);
    setHwV(hwV);

    setUTC(utc);
    setSupportLargeObjs(supportLargeObjs);
    setSupportNumberOfChanges(supportNumberOfChanges);

}

void DevInf::initialize() {
    verDTD = NULL;  
    man    = NULL; 
    mod    = NULL; 
    oem    = NULL; 
    fwV    = NULL; 
    swV    = NULL; 
    hwV    = NULL; 
    devID  = NULL; 
    devTyp = NULL; 
    
    syncCap = NULL;
    dataStores = new ArrayList();  //DataStore[]   
    ctCap      = new ArrayList();  // CTCap[] 
    ext        = new ArrayList();  // Ext[] 
    
    utc                    = FALSE;                  
    supportLargeObjs       = FALSE;     
    supportNumberOfChanges = FALSE;
}


// ---------------------------------------------------------- Public methods
/**
* Gets the DTD version property
*
* @return the DTD version property
*/
VerDTD* DevInf::getVerDTD() {
    return verDTD;
}

/**
* Sets the DTD version property
*
* @param verDTD the DTD version
*/
void DevInf::setVerDTD(VerDTD* verDTD) {
    if (verDTD == NULL) {
        // TBD
    } else {
        if (this->verDTD) {
            delete [] this->verDTD; this->verDTD = NULL;
        }
        this->verDTD = verDTD->clone();
    }    
}

/**
* Gets the device manufacturer property
*
* @return the device manufacturer property
*/
BCHAR* DevInf::getMan(BCHAR* retMan) {
    if (retMan == NULL) {
        return man;
    }
    return bstrcpy(retMan, man);

}

/**
* Sets the device manufacturer property
*
* @param man the device manufacturer property
*
*/
void DevInf::setMan(BCHAR* man) {
    if (this->man) {
        delete [] this->man; this->man = NULL;
    }
    this->man = stringdup(man);
}

/**
* Gets the model name of device
*
* @return the model name of device
*/
BCHAR* DevInf::getMod(BCHAR* retMod) {
    if (retMod == NULL) {
        return mod;
    }
    return bstrcpy(retMod, mod);
}

/**
* Sets the device model property
*
* @param mod the device model property
*
*/
void DevInf::setMod(BCHAR* mod) {
    if (this->mod) {
        delete [] this->mod; this->mod = NULL;
    }
    this->mod = stringdup(mod);
}

/**
* Gets the Original Equipment Manufacturer of the device
*
* @return the OEM property
*/
BCHAR* DevInf::getOEM(BCHAR* retOem) {
    if (retOem == NULL) {
        return oem;
    }
    return bstrcpy(retOem, oem);
}

/**
* Sets the Original Equipment Manufacturer of the device
*
* @param oem the Original Equipment Manufacturer of the device
*
*/
void DevInf::setOEM(BCHAR* oem) {
    if (this->oem) {
        delete [] this->oem; this->oem = NULL;
    }
    this->oem = stringdup(oem);
}

/**
* Gets the firmware version property
*
* @return the firmware version property
*/
BCHAR* DevInf::getFwV(BCHAR* retFwV) {
    if (retFwV == NULL) {
        return fwV;
    }
    return bstrcpy(retFwV, fwV);
}

/**
* Sets the firmware version property
*
* @param fwV the firmware version property
*
*/
void DevInf::setFwV(BCHAR* fwV) {
    if (this->fwV) {
        delete [] this->fwV; this->fwV = NULL;
    }
    this->fwV = stringdup(fwV);
}

/**
* Gets the software version property
*
* @return the software version property
*/
BCHAR* DevInf::getSwV(BCHAR* retSwV) {
    if (retSwV == NULL) {
        return swV;
    }
    return bstrcpy(retSwV, swV);
}

/**
* Sets the software version property
*
* @param swV the software version property
*
*/
void DevInf::setSwV(BCHAR* swV) {
    if (this->swV) {
        delete [] this->swV; this->swV = NULL;
    }
    this->swV = stringdup(swV);
}

/**
* Gets the hardware version property
*
* @return the hardware version property
*/
BCHAR* DevInf::getHwV(BCHAR* retHwv) {
    if (retHwv == NULL) {
        return hwV;
    }
    return bstrcpy(retHwv, hwV);
}

/**
* Sets the hardware version property
*
* @param hwV the hardware version property
*
*/
void DevInf::setHwV(BCHAR* hwV) {
    if (this->hwV) {
        delete [] this->hwV; this->hwV = NULL;
    }
    this->hwV = stringdup(hwV);
}

/**
* Gets the device identifier
*
* @return the device identifier
*/
BCHAR* DevInf::getDevID(BCHAR* retDevID) {
    if (retDevID == NULL) {
        return devID;
    }
    return bstrcpy(retDevID, devID);
}

/**
* Sets the device identifier
*
* @param devID the device identifier
*
*/
void DevInf::setDevID(BCHAR* devID) {
    if (devID == NULL) {
            // TBD
    } else {
        if (this->devID) {
            delete [] this->devID; this->devID = NULL;
        }
        this->devID = stringdup(devID);
    }
}

/**
* Gets the device type
*
* @return the device type
*/
BCHAR* DevInf::getDevTyp(BCHAR* retDevTyp) {
    if (retDevTyp == NULL) {
        return devTyp;
    }
    return bstrcpy(retDevTyp, devTyp);
}

/**
* Sets the device type
*
* @param devTyp the device type
*
*/
void DevInf::setDevTyp(BCHAR* devTyp) {
    if (devTyp == NULL) {
            // TBD
    } else {
        if (this->devTyp) {
            delete [] this->devTyp; this->devTyp = NULL;
        }
        this->devTyp = stringdup(devTyp);
    }
}

/**
* Gets the array of datastore
*
* @return the array of datastore
*/
ArrayList* DevInf::getDataStore() {
    return dataStores;
}

/**
* Sets an array of datastore
*
* @param dataStores an array of datastore
*
*/
void DevInf::setDataStore(ArrayList* dataStores) {
    if (this->dataStores) {
		this->dataStores->clear();
    } 
    if (dataStores) {
	    this->dataStores = dataStores->clone();
    }

}
/**
* Gets the array of content type capability
*
* @return the array of content type capability
*/
ArrayList* DevInf::getCTCap() {
    return ctCap;
}

/**
* Sets an array of content type capability
*
* @param ctCap an array of content type capability
*
*/
void DevInf::setCTCap(ArrayList* ctCap) {
    if (this->ctCap) {
		this->ctCap->clear(); 
    } 
    if (ctCap) {
	    this->ctCap = ctCap->clone();
    }
}

/**
* Gets the array of extension
*
* @return the array of extension
*/
ArrayList* DevInf::getExt() {
    return ext;
}

/**
* Sets an array of extensions
*
* @param ext an array of extensions
*
*/
void DevInf::setExt(ArrayList* ext) {
    if (this->ext) {
		this->ext->clear();
    } 
    if (ext) {
	    this->ext = ext->clone();
    }
}

/**
* Gets true if the device supports UTC based time
*
* @return true if the device supports UTC based time
*/
BOOL DevInf::isUTC() {
    return (utc != NULL);
}

/**
* Sets the UTC property
*
* @param utc is true if the device supports UTC based time
*/
void DevInf::setUTC(BOOL utc) {
    if ((utc == NULL) || (utc != TRUE && utc != FALSE)) {
        this->utc = NULL;
    } else {
        this->utc = utc;
    }  
}


/**
* Gets the Boolean value of utc
*
* @return true if the device supports UTC based time
*/
BOOL DevInf::getUTC() {
    return utc;
}

/**
* Gets true if the device supports handling of large objects
*
* @return true if the device supports handling of large objects
*/
BOOL DevInf::isSupportLargeObjs() {
    return (supportLargeObjs != NULL);
}

/**
* Sets the supportLargeObjs property
*
* @param supportLargeObjs is true if the device supports handling of large objects
*
*/
void DevInf::setSupportLargeObjs(BOOL supportLargeObjs) {
    if ((supportLargeObjs == NULL) || (supportLargeObjs != TRUE && supportLargeObjs != FALSE)) {
        this->supportLargeObjs = NULL;
    } else {
        this->supportLargeObjs = supportLargeObjs;
    }  
}

/**
* Gets the Boolean value of supportLargeObjs
*
* @return true if the device supports handling of large objects
*/
BOOL DevInf::getSupportLargeObjs() {
    return supportLargeObjs;
}

/**
* Gets true if the device supports number of changes
*
* @return true if the device supports number of changes
*/
BOOL DevInf::isSupportNumberOfChanges() {
    return (supportNumberOfChanges != NULL);
}

/**
* Sets the supportNumberOfChanges property
*
* @param supportNumberOfChanges is true if the device supports number of changes
*
*/
void DevInf::setSupportNumberOfChanges(BOOL supportNumberOfChanges) {
    if ((supportNumberOfChanges == NULL) || (supportNumberOfChanges != TRUE && supportNumberOfChanges != FALSE)) {
        this->supportNumberOfChanges = NULL;
    } else {
        this->supportNumberOfChanges = supportNumberOfChanges;
    }  
}

/**
* Gets the Boolean value of supportNumberOfChanges
*
* @return true if the device supports number of changes
*/
BOOL DevInf::getSupportNumberOfChanges() {
    return supportNumberOfChanges;
}

void DevInf::setSyncCap(SyncCap* syncCap) {
    if (this->syncCap) {
        delete this->syncCap; this->syncCap = NULL;
    }
    if (syncCap) {
        this->syncCap = syncCap->clone();
    }
}

SyncCap* DevInf::getSyncCap() {
    return syncCap;
}


DevInf* DevInf::clone() {
    DevInf* ret = new DevInf(verDTD, man, mod, oem, fwV, swV, hwV, devID, 
                             devTyp, dataStores, ctCap, ext, 
                             utc, supportLargeObjs, supportNumberOfChanges, syncCap);
    return ret;
}

