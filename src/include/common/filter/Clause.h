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

#ifndef INCL_CLAUSE
#define INCL_CLAUSE

#include "base/util/ArrayElement.h"

typedef enum {
    CLAUSE         = 0,
    LOGICAL_CLAUSE = 1,
    WHERE_CLAUSE   = 2,
    ALL_CLAUSE     = 3,
    FIELD_CLAUSE   = 4,
    FILTER_CLAUSE  = 5
} ClauseType;

class Clause : public ArrayElement {

    // ------------------------------------------------------- Private interface
private:

    // ----------------------------------------------------- Protected interface
protected:    

    // -------------------------------------------------------- Public interface
public:
    ClauseType type;

    Clause();
    virtual ~Clause();
    virtual ArrayElement* clone() = 0;

};


#endif
