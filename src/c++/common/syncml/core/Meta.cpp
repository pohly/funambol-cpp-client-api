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
 
 
#include "syncml/core/Meta.h"
 
Meta::Meta() {
        this->metInf = NULL;
		set(
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        );
}

Meta::~Meta() {

	if (metInf) {
		delete metInf; metInf = NULL;
	}
}


void Meta::set(BCHAR*    format    ,
               BCHAR*    type      ,
               BCHAR*    mark      ,
               long        size      ,
               Anchor*     anchor    ,
               BCHAR*    version   ,
               NextNonce*  nonce     ,
               long        maxMsgSize,
               long        maxObjSize,
               ArrayList*  emi       ,
               Mem*        mem       ) {

        getMetInf(); // if still null, a new instance will be created

        metInf->setFormat     (format    );
        metInf->setType       (type      );
        metInf->setMark       (mark      );
        metInf->setAnchor     (anchor    );
        metInf->setSize       (size      );
        metInf->setVersion    (version   );
        metInf->setNextNonce  (nonce     );
        metInf->setMaxMsgSize (maxMsgSize);
        metInf->setMaxObjSize (maxObjSize);
        metInf->setMem        (mem       );
        metInf->setEMI        (emi       );
    }


MetInf* Meta::getMetInf() {
    if (metInf == NULL) {
        return (metInf = new MetInf());
    }

    return metInf;
}


void Meta::setMetInf(MetInf* metInf) {
	if (this->metInf) {
		delete this->metInf; this->metInf = NULL;
	}
	if (metInf) {
		this->metInf = metInf->clone();
	}
}

MetInf* Meta::getNullMetInf() {
    return NULL;
}

/**
 * Returns dateSize (in bytes)
 *
 * @return size
 */
long Meta::getSize() {
    return getMetInf()->getSize();
}

/**
 * Sets size
 *
 * @param size the new size value
 */
void Meta::setSize(long size) {
    getMetInf()->setSize(size);
}

/**
 * Returns format
 *
 * @return format
 */
BCHAR* Meta::getFormat(BCHAR* retFormat) {
	if (retFormat == NULL) {
        return getMetInf()->getFormat(NULL);
    }
    return bstrcpy(retFormat, getMetInf()->getFormat(NULL));
    
}

/**
 * Sets format
 *
 * @param format the new format value
 */
void Meta::setFormat(BCHAR* format) {
    getMetInf()->setFormat(format);
}

/**
 * Returns type
 *
 * @return type
 */
BCHAR* Meta::getType(BCHAR* retType) {
	if (retType == NULL) {
        return getMetInf()->getType(NULL);
    }
    return bstrcpy(retType, getMetInf()->getType(NULL));   
}

/**
 * Sets type
 *
 * @param type the new type value
 */
void Meta::setType(BCHAR* type) {
    getMetInf()->setType(type);
}

/**
 * Returns mark
 *
 * @return mark
 */
BCHAR* Meta::getMark(BCHAR* retMark){
	if (retMark == NULL) {
        return getMetInf()->getMark(NULL);
    }
    return bstrcpy(retMark, getMetInf()->getMark(NULL));  
    
}

/**
 * Sets mark
 *
 * @param mark the new mark value
 */
void Meta::setMark(BCHAR* mark) {
    getMetInf()->setMark(mark);
}


/**
 * Returns version
 *
 * @return version
 */
BCHAR* Meta::getVersion(BCHAR* retVersion) {
    if (retVersion == NULL) {
        return getMetInf()->getVersion(NULL);
    }
    return bstrcpy(retVersion, getMetInf()->getVersion(NULL));  
}

/**
 * Sets version
 *
 * @param version the new version value
 */
void Meta::setVersion(BCHAR* version) {
    getMetInf()->setVersion(version);
}

/**
 * Returns anchor
 *
 * @return anchor
 */
Anchor* Meta::getAnchor() {
    return getMetInf()->getAnchor();
}

/**
 * Sets anchor
 *
 * @param anchor the new anchor value
 */
void Meta::setAnchor(Anchor* anchor) {
    getMetInf()->setAnchor(anchor);
}

 /**
 * Returns nextNonce
 *
 * @return nextNonce
 */
NextNonce* Meta::getNextNonce() {
    return getMetInf()->getNextNonce();
}

/**
 * Sets nextNonce
 *
 * @param nextNonce the new nextNonce value
 */
void Meta::setNextNonce(NextNonce* nextNonce) {
    getMetInf()->setNextNonce(nextNonce);
}

/**
 * Returns maxMsgSize
 *
 * @return maxMsgSize
 */
long Meta::getMaxMsgSize() {
    return getMetInf()->getMaxMsgSize();
}

/**
 * Sets maxMsgSize
 *
 * @param maxMsgSize the new maxMsgSize value
 */
void Meta::setMaxMsgSize(long maxMsgSize) {
    getMetInf()->setMaxMsgSize(maxMsgSize);
}

/**
 * Returns maxObjSize
 *
 * @return maxObjSize
 */
long Meta::getMaxObjSize() {
    return getMetInf()->getMaxObjSize();
}

/**
 * Sets maObjSize
 *
 * @param maxObjSize the new maxObjSize value
 */
void Meta::setMaxObjSize(long maxObjSize) {
    getMetInf()->setMaxObjSize(maxObjSize);
}

/**
 * Returns mem
 *
 * @return mem
 */
Mem* Meta::getMem() {
    return getMetInf()->getMem();
}

/**
 * Sets mem
 *
 * @param mem the new mem value
 */
void Meta::setMem(Mem* mem) {
    getMetInf()->setMem(mem);
}

/**
 * Returns emi
 *
 * @return emi
 */
ArrayList* Meta::getEMI() {
    return getMetInf()->getEMI();
}

/**
 *
 * This property is binding with set-method and there is a
 * bug into JiBx: it uses the first method with the specified
 * name without checking the parameter type.
 * This method must be written before all other setEMI() methods
 * to have a right marshalling.
 *
 * @param emi ArrayList of EMI object
 */
void Meta::setEMI(ArrayList* emi) {    
    getMetInf()->setEMI(emi);   
}

Meta* Meta::clone() {
    Meta* ret = new Meta();

    MetInf* retMetInf = new MetInf(getFormat(NULL), getType(NULL), getMark(NULL), getSize(), getAnchor(), getVersion(NULL), 
                   getNextNonce(), getMaxMsgSize(), getMaxObjSize(), getEMI(), getMem());
    ret->setMetInf(retMetInf);
    
    if (retMetInf) {
        delete retMetInf; retMetInf = NULL;
    }

    return ret;

}
