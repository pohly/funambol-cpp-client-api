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

/**
 * This class is a collection of utility methods used to manage Clause
 * and related classes.
 */

#include "base/util/StringBuffer.h"
#include "filter/ClauseUtil.h"
#include "filter/AllClause.h"
#include "filter/FieldClause.h"
#include "filter/LogicalClause.h"
#include "filter/WhereClause.h"
#include "syncml/core/Property.h"
#include "syncml/core/PropParam.h"


/**
 * Converts a WhereClause to a CGI query string. The returned
 * string is allocated with the new [] operator and must be
 * deleted by the caller with the delete [] operator.
 *
 * @param clause the clause to convert
 *
 * @return the corresponding CGI query string
 */
const wchar_t* ClauseUtil::toCGIQuery(Clause& clause) {
    StringBuffer query;

    ArrayList* operands = NULL;
    WhereClause* where = NULL;

    if (clause.type == WHERE_CLAUSE) {
        operands = new ArrayList();
        operands->add(clause);
    } else {
        operands = ((LogicalClause*)&clause)->getOperands();
    }

    for (int i=0; i<operands->size(); ++i) {
        where = (WhereClause*)operands->get(i);
        if (i) {
            switch (((LogicalClause*)&clause)->getOperator()) {
                case AND:
                    query.append(TEXT("&amp;AND;"));
                    break;
                case OR:
                    query.append(TEXT("&amp;OR;"));
                    break;
            }
        }
        query.append(where->getProperty());
        switch (where->getOperator()) {
            case EQ:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;EQ;"));
                } else {
                    query.append(TEXT("&amp;iEQ;"));
                }
                break;
            case NE:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;NE;"));
                } else {
                    query.append(TEXT("&amp;iNE;"));
                }
                break;
            case LT:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;LT;"));
                } else {
                    query.append(TEXT("&amp;iLT;"));
                }
                break;
            case GT:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;GT;"));
                } else {
                    query.append(TEXT("&amp;iGT;"));
                }
                break;
            case LE:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;LE;"));
                } else {
                    query.append(TEXT("&amp;iLE;"));
                }
                break;
            case GE:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;GE;"));
                } else {
                    query.append(TEXT("&amp;iGE;"));
                }
                break;
            case CONTAIN:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;CON;"));
                } else {
                    query.append(TEXT("&amp;iCON;"));
                }
                break;
            case NCONTAIN:
                if (where->isCaseSensitive()) {
                    query.append(TEXT("&amp;NCON;"));
                } else {
                    query.append(TEXT("&amp;iNCON;"));
                }
                break;
        }
        query.append(where->getValue());
    }

    delete operands; operands = NULL;

    return stringdup(query.c_str());
}

/**
 * Converts a Clause to a Filter object. The Filter is allocated
 * with the new operator and must be deleted by the caller with 
 * the delete operator.
 *
 * A Filter is created by an AND LogicalClause where the first operand
 * is the field filter and the second operand is the record filter. The
 * first operand must be of type FieldClause; the latter of type 
 * WhereClause.
 *
 * @param clause the clause to convert
 *
 * @return the corresponding filter; NULL is returned in case it was
 *         not possible to create the filter
 */
Filter* ClauseUtil::toFilter(SourceFilter& sourceFilter) {
    LogicalClause* clause = sourceFilter.getClause();

    //
    // A filter is created by an AND LogicaFilter...
    //
    if (clause->getOperator() != AND) {
        return NULL;
    }

    //
    // A AND Clause must have at least two operands
    //
    ArrayList* operands = clause->getOperands();
    if (operands->size() < 2) {
        return NULL;
    }

    //
    // The first operand must be a FieldClause or AllClause
    //
    Clause* c = (Clause*)operands->get(0);

    if ((c->type != FIELD_CLAUSE) && (c->type != ALL_CLAUSE)) {
        return NULL;
    }

    FieldClause* fieldClause = (c->type == ALL_CLAUSE) 
                             ? NULL
                             : (FieldClause*)c;

    //
    // The second operand mustbe a WhereClause
    //
    c = (Clause*)operands->get(1);

    if (  (c->type != LOGICAL_CLAUSE) 
       && (c->type != ALL_CLAUSE)
       && (c->type != WHERE_CLAUSE)) {
        return NULL;
    }

    Clause* recordClause = (c->type == ALL_CLAUSE) 
                         ? NULL
                         : c;

    //
    // Now we can start to create the filter...
    //
    Meta meta;
    meta.setType(sourceFilter.getType());

    Filter* filter = new Filter();
    filter->setMeta(&meta);

    if (fieldClause) {

        Item field;
        Meta fieldMeta;
        ComplexData fieldData;
   
        fieldMeta.setType(TEXT("application/vnd.syncml-devinf+xml"));
        fieldData.setProperties(fieldClause->getProperties());

        field.setMeta(&fieldMeta);
        field.setData(&fieldData);

        filter->setField(&field);
    }

    if (recordClause) {
        Item record;
        Meta recordMeta;
        ComplexData recordData;

        recordMeta.setType(TEXT("syncml:filtertype-cgi"));
        wchar_t* query = (wchar_t*)toCGIQuery(*recordClause);
        recordData.setData(query);
        safeDelete(&query);

        record.setMeta(&recordMeta);
        record.setData(&recordData);

        filter->setRecord(&record);
    }

    if (sourceFilter.isInclusive()) {
        filter->setFilterType(TEXT("INCLUSIVE"));
    }

    return filter;
}

 /**
  * Creates the filter clause given download age, body size and attach size.
  * since, bodySize and attachSize have the following meaning:
  *
  * since
  * -----
  * Since when we want download new emails?
  *
  * NULL: since ever
  * x : emails earlier than the date x
  *
  * bodySize
  * --------
  * How much body shall be downloaded?
  *
  * 0 : only headers (e.g. subject)
  * -1: no limits
  * x : up to x Kb
  *
  * attachSize
  * ----------
  * If there are attachments, which ones shall be downloaded?
  *
  * 0 : no attachments
  * -1: all attachments
  * x : attachments up to x kb
  *
  *
  * @param downloadAge download age
  * @param bodySize body size
  * @param attachSize attachment size
  */
  SourceFilter* ClauseUtil::createSourceFilter(const wchar_t* since, int bodySize, int attachSize) {
    if ((since == NULL) && (bodySize == -1) && (attachSize == -1)) {
        //
        // No filter is needed
        //
        return NULL;
    }

    ArrayList operands;

    //
    // FIELD CLAUSE(S)
    // ---------------
    //

    ArrayList properties;
    if (bodySize >= 0) {
        Property p;

        p.setPropName(TEXT("emailitem"));
        if (bodySize > 0) {
            ArrayList params;
            PropParam textParam;
            textParam.setParamName(TEXT("texttype"));

            params.add(textParam);

            p.setPropParams(params);
            p.setMaxSize(bodySize*1024);
        }
        properties.add(p);
    }

    if (attachSize >= 0) {
        ArrayList params;
        PropParam attachParam;
        attachParam.setParamName(TEXT("attachtype"));
        
        params.add(attachParam);

        Property p;

        p.setPropName(TEXT("emailitem"));
        p.setPropParams(params);
        p.setMaxSize(attachSize*1024);

        properties.add(p);
    }

    if (properties.size() == 0) {
        AllClause all;
        operands.add(all);
    } else {
        FieldClause fieldClause(properties);
        operands.add(fieldClause);
    }

    //
    // RECORD CLAUSE
    // -------------
    // 
    //
    // If since is not NULL we need to add a Record clause
    //
    if (since) {
        WhereClause recordClause(TEXT("modified"), since, GE, FALSE);
        operands.add(recordClause);
    } else {
        AllClause all;
        operands.add(all);
    }

    LogicalClause clause(AND, operands);

    SourceFilter* filter = new SourceFilter();
    filter->setClause(clause);
    

    return filter;
}
