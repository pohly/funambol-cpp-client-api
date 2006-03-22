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

#include "base/util/utils.h"
#include "filter/SourceFilter.h"


SourceFilter::SourceFilter() : clause(NULL), type(NULL), inclusive(FALSE) {
}

SourceFilter::~SourceFilter() {
    if (clause) delete clause;
}

void SourceFilter::setInclusive(BOOL i) {
    inclusive = i;
}

BOOL SourceFilter::isInclusive() {
    return (inclusive == TRUE);
}

BOOL SourceFilter::isExclusive() {
    return (inclusive == FALSE);
}

/*
 * Gets clause
 *
 * @return  the current clause's value
 *
 */
LogicalClause* SourceFilter::getClause() {
    return clause;
}

/*
 * Sets clause
 *
 * @param clause the new value
 *
 */
void SourceFilter::setClause(LogicalClause* clause) {
    if (this->clause) {
        delete this->clause; this->clause = NULL;
    }

    if (clause) {
        this->clause = (LogicalClause*)clause->clone();
    }
}

/*
 * Sets clause
 *
 * @param clause the new value
 *
 */
void SourceFilter::setClause(LogicalClause& clause) {
    if (this->clause) {
        delete this->clause;
    }

    this->clause = (LogicalClause*)clause.clone();
}

/**
 * Returns type
 *
 * @return type
 */
BCHAR* SourceFilter::getType(BCHAR* buf) {
	if (buf == NULL) {
        return type;
    }
    return bstrcpy(buf, type);   
}

/**
 * Sets type
 *
 * @param type the new type value
 */
void SourceFilter::setType(BCHAR* type) {
    if (this->type) {
        delete [] this->type; this->type = NULL;
    }

    if (type) {
        this->type = stringdup(type);
    }
}
