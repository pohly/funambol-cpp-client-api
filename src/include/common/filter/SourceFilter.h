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

#ifndef INCL_FILTERCLAUSE
#define INCL_FILTERCLAUSE

#include "filter/LogicalClause.h"

class SourceFilter {

    // ------------------------------------------------------- Private interface
private:

    LogicalClause* clause;
    BOOL inclusive;
    wchar_t* type;

    // ----------------------------------------------------- Protected interface
protected:

    // -------------------------------------------------------- Public interface
public:


    /*
     * FilterClause constructor
     * 
     */
    SourceFilter();


    /*
     * FilterClause destructor
     * 
     */
    ~SourceFilter();

    /*
     * setInclusive
     * 
     * @param i
     */
    void setInclusive(BOOL i);


    /*
     * isInclusive
     * 
     */
    BOOL isInclusive();


    /*
     * isExclusive
     * 
     */
    BOOL isExclusive();


    /*
     * setClause
     * 
     * @param c
     */
    void setClause(LogicalClause* c);

    /*
     * setClause
     * 
     * @param c
     */
    void setClause(LogicalClause& c);


    /*
     * getClause
     * 
     */
    LogicalClause* getClause();

    /**
     * Returns type
     *
     * @return type
     */
    wchar_t* getType(wchar_t* buf = NULL);

    /**
     * Sets type
     *
     * @param type the new type value
     */
    void setType(wchar_t* type);

};


#endif
