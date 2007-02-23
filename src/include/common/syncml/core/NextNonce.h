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


#ifndef INCL_NEXT_NONCE
#define INCL_NEXT_NONCE

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/base64.h"


class NextNonce {
    
     // ------------------------------------------------------------ Private data
    private:
        char*  wvalue;
        char* value;
        long size;
        void initialize();
    // ---------------------------------------------------------- Protected data    
    public:
    
    NextNonce();
    NextNonce(void* value, unsigned long size);

    /*
    * For a NextNonce starting from a b64 string
    */
    NextNonce(char*  b64value);

    ~NextNonce();
    
    void* getValue();
    long getValueSize();

    void* setValue(void* argValue, unsigned long size);        
    
    void setWValue(const char* wnonce);

    const char* getValueAsBase64();

    NextNonce* clone();
};

#endif
