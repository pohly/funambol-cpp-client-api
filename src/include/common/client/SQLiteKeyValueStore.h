/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#ifndef INCL_SQLLITE_KEY_VALUE_STORE
#define INCL_SQLLITE_KEY_VALUE_STORE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include <sqlite3.h>

#include "base/util/KeyValueStore.h"
#include "base/util/KeyValuePair.h"
#include "client/SQLKeyValueStore.h"

BEGIN_NAMESPACE

class SQLiteKeyValueStore : public SQLKeyValueStore {
private:
    
    StringBuffer    path;
    
    sqlite3         * db;
    mutable sqlite3_stmt    * statement;
    
protected:

    
    /*
     * Execute a query to get a value, given the key.   If a connection to
     * the database is not open, open it. 
     *
     * @param sql   - The sql command to execute.
     *
     * @return      - The result of the query - an Enumeration of KeyValuePair s
     */
    virtual Enumeration & query(const StringBuffer & sql) const;
    
    /*
     * Execute a non-select query.  If a connection to the database is not open,
     * open it.
     *
     * @param sql   - The sql command to execute.
     *
     * @return      - Success or Failure
     */
    virtual int execute(const StringBuffer & sql);
    
public:
    
    mutable
    class SQLiteKeyValueStoreEnumeration : public Enumeration
    {
    protected:
        friend class SQLiteKeyValueStore;
    
        int lastReturn;
        int totalRows;
        int nextRow;
        KeyValuePair kvp;
        SQLiteKeyValueStore & skvs;
        
        void reinit(int lastRet)
        {
            lastReturn = lastRet;
            totalRows  = 1;
            nextRow    = 0;
        }
        
    public:
        
        SQLiteKeyValueStoreEnumeration(SQLiteKeyValueStore & instance)
        : skvs(instance) {}
        
       /**
        * Return true if there are more elements in the enumeration.
        */
        virtual bool hasMoreElement() const
        {
            return (skvs.statement && lastReturn == SQLITE_ROW && nextRow < totalRows);
        }

        /**
         * Return the next element or NULL if there is none.
         */
        virtual ArrayElement* getNextElement()
        {
            StringBuffer sb;
            sb = (const char *) sqlite3_column_text(skvs.statement, 0);
            kvp.setKey   (sb);
            sb = (const char *) sqlite3_column_text(skvs.statement, 1);
            kvp.setValue (sb);

            lastReturn = sqlite3_step(skvs.statement);
            nextRow++;

            return &kvp;
        }

    } enumeration;
    
    
    /*
     * Constructor
     *
     * @param uri       - The location of the server
     * @param colKey    - The name of the column for the key
     * @param colValue  - The name of the column of the value
     * @param path      - The name of the full path to the database to use
     *
     */
    SQLiteKeyValueStore(const StringBuffer & table, const StringBuffer & colKey,   const StringBuffer & colValue,
                        const StringBuffer & path);
    
    /*
     * Destructor
     *
     * Subclasses MUST disconnect in their own destructors
     */
    virtual ~SQLiteKeyValueStore();
    
    /**
     * Get all the properties that are currently defined.     
     */
    virtual Enumeration& getProperties() const;
    
    /*
     * Connect to the database server.  The connection should be stored
     * within the subclass and destroyed in disconnect();  If connect is called
     * while a connection exists, nothing should happen.
     *
     * @return      - Success or Failure
     */
    int connect();
    
    /*
     * Disconnect from the database server.  If the connection is not open,
     * do nothing.
     *
     * @return      - Success or Failure
     */
    int disconnect();
    
    /**
     * Ensure that all properties are stored persistently.
     * If setting a property led to an error earlier, this
     * call will indicate the failure.
     *
     * @return 0 - success, failure otherwise
     */
    virtual int save();
};


END_NAMESPACE

/** @} */
/** @endcond */
#endif
