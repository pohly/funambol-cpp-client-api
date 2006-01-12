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


#ifndef INCL_CRED
#define INCL_CRED

#include "base/fscapi.h"
#include "syncml/core/Authentication.h"
#include "syncml/core/Constants.h"

class Cred {
    

     // ------------------------------------------------------------ Private data
    private:

        Authentication* authentication;        
    
    // ---------------------------------------------------------- Protected data    
    public:
        
        Cred();
        ~Cred();
        
        Cred(Authentication* authentication);

        /**
         * Gets type property
         *
         * @return type property
         */
        wchar_t* getType(wchar_t* retType);

        /**
         * Gets format property
         *
         * @return format property
         */
        wchar_t* getFormat(wchar_t* retFormat);

        /**
         * Gets data property
         *
         * @return data property
         */
        wchar_t* getData(wchar_t* retData);

        /**
         * Gets the username stored in this credential
         *
         * @return the username stored in this credential
         */
        wchar_t* getUsername(wchar_t* retUsername);


        /**
         * Create and return the Authentication object corresponding to the given
         * type and data.
         *
         * @param type the type of the required Authentication object
         * @param data the data to be interpreted based on the type
         *
         * @return the corresponding Authentication object.
         */
        Authentication* createAuthentication(wchar_t* data, wchar_t* type);

        /**
         * Gets the Authentication object.
         *
         * @return authentication the authentication objects
         */
        Authentication* getAuthentication();

        /**
         * Sets the Authentication object.
         *
         * @param auth the new Authentication object
         *
         */
        void setAuthentication(Authentication* auth);

        Cred* clone();
};

#endif
