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



#ifndef INCL_META
#define INCL_META

#include "base/fscapi.h"
#include "syncml/core/MetInf.h"
#include "syncml/core/Anchor.h"
#include "syncml/core/NextNonce.h"
#include "syncml/core/Mem.h"
#include "syncml/core/EMI.h"

class Meta {
    
     // ------------------------------------------------------------ Private data
    private:
       MetInf* metInf;
       
       void set(BCHAR*    format    ,
                BCHAR*    type      ,
                BCHAR*    mark      ,
                long        size      ,
                Anchor*     anchor    ,
                BCHAR*    version   ,
                NextNonce*  nonce     ,
                long        maxMsgSize,
                long        maxObjSize,
                ArrayList*  emi       ,
                Mem*        mem      ); 
                
    
    // ---------------------------------------------------------- Public data    
    public:

        Meta();

        ~Meta();
    
        MetInf* getMetInf();
		
		void setMetInf(MetInf* metInf);

		 /**
		 * This get method always returns null. This is a used in the JiBX mapping
		 * in order to do not output the MetInf element.
		 *
		 * @return always null
		 */
		MetInf* getNullMetInf();

		/**
		 * Returns dateSize (in bytes)
		 *
		 * @return size
		 */
		long getSize();

		/**
		 * Sets size
		 *
		 * @param size the new size value
		 */
		void setSize(long size);
   
		/**
		 * Returns format
		 *
		 * @return format
		 */
		BCHAR* Meta::getFormat(BCHAR* retFormat=0);

		/**
		 * Sets format
		 *
		 * @param format the new format value
		 */
		void setFormat(BCHAR* format);

		/**
		 * Returns type
		 *
		 * @return type
		 */
		BCHAR* getType(BCHAR* retType=0);

		/**
		 * Sets type
		 *
		 * @param type the new type value
		 */
		void setType(BCHAR* type);

		/**
		 * Returns mark
		 *
		 * @return mark
		 */
		BCHAR* getMark(BCHAR* retMark=0);

		/**
		 * Sets mark
		 *
		 * @param mark the new mark value
		 */
		void setMark(BCHAR* mark);

		/**
		 * Returns version
		 *
		 * @return version
		 */
		BCHAR* getVersion(BCHAR* retVersion=0);

		/**
		 * Sets version
		 *
		 * @param version the new version value
		 */
		void setVersion(BCHAR* version);

		
		/**
		 * Returns anchor
		 *
		 * @return anchor
		 */
		Anchor* getAnchor();

		/**
		 * Sets anchor
		 *
		 * @param anchor the new anchor value
		 */
		void setAnchor(Anchor* anchor);

		 /**
		 * Returns nextNonce
		 *
		 * @return nextNonce
		 */
		NextNonce* getNextNonce();

		/**
		 * Sets nextNonce
		 *
		 * @param nextNonce the new nextNonce value
		 */
		void setNextNonce(NextNonce* nextNonce);

		/**
		 * Returns maxMsgSize
		 *
		 * @return maxMsgSize
		 */
		long getMaxMsgSize();

		/**
		 * Sets maxMsgSize
		 *
		 * @param maxMsgSize the new maxMsgSize value
		 */
		void setMaxMsgSize(long maxMsgSize);

		/**
		 * Returns maxObjSize
		 *
		 * @return maxObjSize
		 */
		long getMaxObjSize();

		/**
		 * Sets maObjSize
		 *
		 * @param maxObjSize the new maxObjSize value
		 */
		void setMaxObjSize(long maxObjSize);

		/**
		 * Returns mem
		 *
		 * @return mem
		 */
		Mem* getMem();

		/**
		 * Sets mem
		 *
		 * @param mem the new mem value
		 */
		void setMem(Mem* mem);
        
        /**
		 * Sets emi
		 *
		 * @param emi the new emi value
		 */
		void setEMI(ArrayList* emi);
		
        /**
		 * Gets emi
		 *
		 * @return emi 
		 */
		ArrayList* getEMI();
        
        /**
		 * clone meta
		 *
		 * @return meta
		 */
        Meta* clone();
};

#endif
