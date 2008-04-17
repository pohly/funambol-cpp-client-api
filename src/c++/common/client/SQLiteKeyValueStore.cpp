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

#include <sqlite3.h>

#include "client/SQLiteKeyValueStore.h"
#include "base/util/StringBuffer.h"
#include "base/util/KeyValuePair.h"

    
BEGIN_NAMESPACE
    
SQLiteKeyValueStore::SQLiteKeyValueStore(const StringBuffer & table, const StringBuffer & colKey,   const StringBuffer & colValue,
                                         const StringBuffer & path)
: SQLKeyValueStore(table,colKey,colValue), enumeration(*this)
{
    this->path = path;
    db = NULL;
    statement = NULL;
}


SQLiteKeyValueStore::~SQLiteKeyValueStore()
{
    if (statement)
    {
        sqlite3_finalize(statement);
        statement = NULL;
    }
    disconnect();
}

Enumeration& SQLiteKeyValueStore::query(const StringBuffer & sql) const
{
    if (!db)
    {
        throw new StringBuffer /*Exception*/ ("Not connected to database");
    }
    
    if (statement)
    {
        sqlite3_finalize(statement);
        statement = NULL;
    }
    
    int ret = sqlite3_prepare(db, sql.c_str(), sql.length(), &statement, NULL);
    if (ret == SQLITE_OK) {
        enumeration.lastReturn = sqlite3_step(statement);
    }
    return enumeration;
}

int SQLiteKeyValueStore::execute(const StringBuffer & sql)
{
    connect();

    sqlite3_stmt * stmt;
    
    int ret = sqlite3_prepare(db, sql.c_str(), sql.length(), &stmt, NULL);
    if (ret == SQLITE_OK) {
        ret = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    
    
    if (ret == SQLITE_DONE)
        ret = SQLITE_OK;
    return ret;
}

int SQLiteKeyValueStore::connect()
{
    if (db)
        return true;
        
    int ret = sqlite3_open(path, &db);
    if (ret == SQLITE_OK)
    {
        execute("BEGIN TRANSACTION;");
    }
    return ret;
}


int SQLiteKeyValueStore::disconnect()
{
    if (db == NULL)
        return true;
    
    execute("ROLLBACK TRANSACTION;");
    
    int ret = sqlite3_close(db);
    db = NULL;
    
    return ret;
}

/**
 * Ensure that all properties are stored persistently.
 * If setting a property led to an error earlier, this
 * call will indicate the failure.
 *
 * @return 0 - success, failure otherwise
 */
int SQLiteKeyValueStore::save()
{
    return (
        ((execute("COMMIT TRANSACTION;") == SQLITE_OK) && (execute("BEGIN TRANSACTION;") == SQLITE_OK))
        ? 1 : 0);
}

END_NAMESPACE