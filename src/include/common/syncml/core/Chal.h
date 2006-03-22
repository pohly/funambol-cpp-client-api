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


#ifndef INCL_CHAL
#define INCL_CHAL

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "syncml/core/Constants.h"
#include "syncml/core/Meta.h"



class Chal {
    
     // ------------------------------------------------------------ Private data
    private:
       Meta* meta;
       void initialize();
    // ---------------------------------------------------------- Protected data    
    public:
    
    Chal();
    ~Chal();
    /**
     * Constructs a new Chal object.
     *
     * @param meta The meta object - NOT NULL
     *
     */
    Chal (Meta* meta);

    // ---------------------------------------------------------- Public methods
    /**
     * Gets the Meta property
     * 
     * @return meta the Meta property
     */
    Meta* getMeta();
    
    /**
     * Sets the Meta property
     *
     * @param meta the Meta property
     *
     */
    void setMeta(Meta* meta);

    /**
     * Returns the nextNonce property or null
     *
     *  @return the nextNonce property or null
     */
    NextNonce* getNextNonce();

    void setNextNonce(NextNonce* nextNonce);
    /**
     * Returns the authentication type
     *
     * @return authentication type.
     */
    BCHAR* getType(BCHAR* retType);

    /**
     * Returns the authentication format
     *
     * @return format the authentication format
     */
    BCHAR* getFormat(BCHAR* retFormat);

    /**
     * Creates a basic authentication challange.
     * This will have type = Cred.AUTH_TYPE_BASIC and
     * format = Constants.FORMAT_B64
     *
     * @return the newly created AuthenticationChallange
     */
    static Chal* getBasicChal();

    /**
     * Creates a MD5 authentication challange.
     * This will have type = Cred.AUTH_TYPE_MD5 and
     * format = Constants.FORMAT_B64
     *
     * @return the newly created AuthenticationChallange
     */
    static Chal* getMD5Chal();

    Chal* clone();
   
};

#endif
