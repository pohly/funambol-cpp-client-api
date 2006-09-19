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


#ifndef INCL_VER_PROTO
#define INCL_VER_PROTO

#include "base/fscapi.h"


class VerProto {
    
     // ------------------------------------------------------------ Private data
    private:
        char*  version;       
    
    // ---------------------------------------------------------- Protected data    
    public:
    
        VerProto();
        ~VerProto();
    
        /**
         * Creates a new VerProto object from its version.
         *
         * @param version the protocol version - NOT NULL
         *
         */
        VerProto(char*  version);            
    
        /**
         * Returns the protocol version.
         * 
         * @return the protocol version - NOT NULL
         *
         */
        char*  getVersion(char*  retVersion);
    
        /**
         * Sets the protol version.
         *
         * @param version the protocol version - NOT NULL
         *
         */
        void setVersion(char*  version);        
        
        VerProto* clone();
   
};

#endif
