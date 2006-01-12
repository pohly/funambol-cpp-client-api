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

#ifndef INCL_TARGET
#define INCL_TARGET

#include "base/fscapi.h"


class Filter;  // forward declaration

class Target {
    
    // ------------------------------------------------------------ Private data
    private:
        wchar_t* locURI;
        wchar_t* locName;
        Filter*  filter;
        
        void set(const wchar_t* locURI, const wchar_t* locName, const Filter* filter);

    // ---------------------------------------------------------- Protected data    
    public:


     /**
     * Creates a new Target object with the given locURI and locName
     *
     * @param locURI the locURI - NOT NULL
     * @param locName the locName - NULL
     * @param filter a filter to be applied for this target; it defaults to NULL
     *
     */
    Target(const wchar_t* locURI, const wchar_t* locName, const Filter* filter = NULL);

    /**
     * Creates a new Target object with the given locURI
     *
     * @param locURI the locURI - NOT NULL
     *
     */
    Target(const wchar_t* locURI);
    
    Target();
    ~Target();
    
    
    // ---------------------------------------------------------- Public methods
    
    /** Gets locURI properties
     * @return locURI properties
     */
    const wchar_t* getLocURI();

    /**
     * Sets locURI property
     * @param locURI the locURI
     */
    void setLocURI(const wchar_t* locURI);

    /**
     * Gets locName properties
     * @return locName properties
     */
    const wchar_t* getLocName();

    /**
     * Sets locName property
     * @param locName the locURI
     */
    void setLocName(const wchar_t* locName);

    /**
     * Gets filter
     *
     * @return  the current filter's value
     *
     */
    Filter* getFilter();
    
    /**
     * Sets filter
     *
     * @param filter the new value
     *
     */
    void setFilter(Filter* filter);


    Target* clone();



};

#endif
