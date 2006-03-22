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
#include "filter/WhereClause.h"


WhereClause::WhereClause() : property(NULL), value(NULL), op(UNKNOWN), caseSensitive(TRUE) {
    type = WHERE_CLAUSE;
}

WhereClause::WhereClause(const BCHAR* p, const BCHAR* v, WhereClauseOperator o, BOOL s) {
    type = WHERE_CLAUSE;
    property = NULL; if (p) property = stringdup(p);
    value = NULL; if (v) value = stringdup(v);
    op = o;
    caseSensitive = s;
}

WhereClause::~WhereClause() {
    if (property) {
        delete [] property;
    }
    if (value) {
        delete [] value;
    }
}

/*
 * Gets property
 *
 * @return  the current property's value
 *
 */
BCHAR* WhereClause::getProperty(BCHAR* property) {
    if (property == NULL) {
        return this->property;
    }

    return bstrcpy(property, this->property);
}

/*
 * Sets property
 *
 * @param property the new value
 *
 */
void WhereClause::setProperty(BCHAR* property) {
    if (this->property) {
        delete this->property; this->property = NULL;
    }

    if (property) {
        this->property = stringdup(property);
    }
}

/*
 * Gets value
 *
 * @return  the current value's value
 *
 */
BCHAR* WhereClause::getValue(BCHAR* value) {
    if (value == NULL) {
        return this->value;
    }

    return bstrcpy(value, this->value);
}

/*
 * Sets value
 *
 * @param value the new value
 *
 */
void WhereClause::setValue(BCHAR* value) {
    if (this->value) {
        delete this->value; this->value = NULL;
    }

    if (value) {
        this->value = stringdup(value);
    }
}

/*
 * Gets operator
 *
 * @return  the current operator's value
 *
 */
WhereClauseOperator WhereClause::getOperator() {
    return op;
}

/*
 * Sets operator
 *
 * @param operator the new value
 *
 */
void WhereClause::setOperator(WhereClauseOperator o) {
    op = o;
}

BOOL WhereClause::isCaseSensitive() {
    return (caseSensitive == TRUE);
}

void WhereClause::setCaseSensitive(BOOL s) {
    caseSensitive = s;
}

ArrayElement* WhereClause::clone() {
    return (ArrayElement*)new WhereClause(property, value, op, caseSensitive);
}
