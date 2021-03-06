/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"
#include "examples/TestSyncSource.h"
#include "examples/TestSyncSource2.h"
#include "filter/AllClause.h"
#include "filter/ClauseUtil.h"
#include "filter/LogicalClause.h"
#include "filter/FieldClause.h"
#include "filter/sourceFilter.h"
#include "filter/WhereClause.h"
#include "syncml/core/core.h"
#include "syncml/formatter/Formatter.h"
#include "base/globalsdef.h"

USE_NAMESPACE


void testClause() {
    SourceFilter filter;

    WhereClause where1("CompanyName", "funambol", EQ, false);
    WhereClause where2("FirstName", "ste", CONTAIN, true);

    ArrayList operands;

    operands.add(where1);
    operands.add(where2);

    LogicalClause record(AND, operands);
    AllClause all;

    //
    // Record only filter
    //
    operands.clear();
    operands.add(all);
    operands.add(record);

    LogicalClause recordOnly(AND, operands);

    filter.setClause(recordOnly);
    filter.setInclusive(false);
    filter.setType("text/x-s4j-sifc");

    Filter* f = ClauseUtil::toFilter(filter);

    StringBuffer* sb = Formatter::getFilter(f);
    LOG.info("Record only filter");
    LOG.info("%s", sb->c_str());

    delete f; delete sb;

    //
    // Field only filter
    //
    Property fieldProperty;
    ArrayList fieldProperties, fieldParameters;
    PropParam param;

    param.setParamName("texttype");
    fieldParameters.add(param);
    param.setParamName("attachtype");
    fieldParameters.add(param);

    fieldProperty.setPropName("emailitem");
    fieldProperty.setMaxSize(20000);
    fieldProperty.setPropParams(&fieldParameters);
    fieldProperties.add(fieldProperty);

    FieldClause field(&fieldProperties);

    operands.clear();
    operands.add(field);
    operands.add(all);

    LogicalClause fieldOnly(AND, operands);

    filter.setClause(fieldOnly);
    filter.setInclusive(true);

    f = ClauseUtil::toFilter(filter);
    sb = Formatter::getFilter(f);

    LOG.info("Field only filter");
    LOG.info("%s", sb->c_str());

    delete f; delete sb;

    //
    // Both record and field filters
    //
    operands.clear();
    operands.add(field);
    operands.add(record);

    LogicalClause fieldAndRecord(AND, operands);

    filter.setClause(fieldAndRecord);

    f = ClauseUtil::toFilter(filter);
    sb = Formatter::getFilter(f);

    LOG.info("Field and record filter");
    LOG.info("%s", sb->c_str());

    delete f; delete sb;
}
