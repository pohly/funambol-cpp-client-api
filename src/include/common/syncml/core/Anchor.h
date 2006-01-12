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

#ifndef INCL_ANCHOR
#define INCL_ANCHOR

#include "base/fscapi.h"

class Anchor {
    
    // ------------------------------------------------------------ Private data
    private:
        wchar_t* last;
        wchar_t* next;
    // ---------------------------------------------------------- Protected data    
    public:
    
        Anchor(const wchar_t* last, const wchar_t* next);
        ~Anchor();

        /**
     * Gets the last property
     *
     * @return the last property
     */
     const wchar_t* getLast();

    /**
     * Sets the last property
     *
     * @param last the last property
     *
     */
    void setLast(const wchar_t* last);
    
    /**
     * Gets the next property
     *
     * @return the next property
     */
    const wchar_t* getNext();
    
    /**
     * Sets the next property
     *
     * @param next the next property
     *
     */
    void setNext(const wchar_t* next);


    Anchor* clone();

};

#endif
