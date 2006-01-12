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


#ifndef INCL_SESSION_ID
#define INCL_SESSION_ID

#include "base/fscapi.h"


class SessionID {
    
     // ------------------------------------------------------------ Private data
    private:
         wchar_t* sessionID;        
    
    public:       

    // ------------------------------------------------------------ Constructors
   
    SessionID();
    ~SessionID();
    
    /**
     * Create a new SessionID object with the parameters
     *
     * @param sessionID the identifier of session - NOT NULL
     *
     */
    SessionID(wchar_t* sessionID);

    // ---------------------------------------------------------- Public methods

    /**
     * Gets the session identifier
     *
     * @return sessionID the session identifier
     */
    wchar_t* getSessionID(wchar_t* retSessionID);

    /**
     * Sets the session identifier
     *
     * @param sessionID the session identifier
     */
    void setSessionID(wchar_t* sessionID);

    SessionID* clone();
       
};

#endif
