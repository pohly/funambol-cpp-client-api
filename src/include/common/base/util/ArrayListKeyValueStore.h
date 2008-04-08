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

#ifndef INCL_ARRAYLIST_KEY_VALUE_STORE
#define INCL_ARRAYLIST_KEY_VALUE_STORE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/ErrorHandler.h"
#include "base/util/ArrayElement.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSourceReport.h"
#include "base/util/KeyValuePair.h"
#include "base/util/KeyValueStore.h"
#include "base/util/ArrayListEnumeration.h"

/**
 * This is the interface for the handling of the key/value that
 * has to be written in the storage. It provides methods that has to be 
 * specialized by implementation on filesystem, registry, db...
 */
class ArrayListKeyValueStore : public KeyValueStore {

private:
                  
    /**
    * the internal counter on which do the iteration
    */
    int counter;
                  
protected:

    /**
    * The list contains the pair key/value that are what is in the file that is 
    * in the form key:value
    */
    ArrayList data; 

public:
    
    /**      
     * The name of the general node 
     */
    ArrayListKeyValueStore(const char* node) : KeyValueStore(node) {         
        counter = 0; 
    }

    // Destructor
    virtual ~ArrayListKeyValueStore() {}      
    
    /**
    * return the reference to the inside arraylist.
    * It is used because it is useful to have a copy 
    * of the cache to calculate the diff    
    */
    //virtual ArrayList& getData() { return data; }

    /*
     * Returns the value of the given property from the data ArrayList
     *
     *@param prop - the property name
     *
     *@return   empty value means the property is not found. This is the copy of the
     *          value in the arraylist
     */
    virtual StringBuffer readPropertyValue(const char *prop);

    /*
     * Sets a property value from the data ArrayList
     *
     * @param prop      - the property name
     * @param value    - the property value (zero terminated string)
     * @param add      - if the property doesn't exist it add. If false doesn't add
     */
    virtual void setPropertyValue(const char *prop, const char *value, bool add = true);
    
     /**
     * Remove a certain property from the data ArrayList
     *
     * @param prop    the name of the property which is to be removed
     */
    virtual void removeProperty(const char *prop);
    
    /**
     * Read all the properties that are in the store. This is
     * an enumeration of KeyValuePairs
     * 
     */
    virtual Enumeration* getProperties() {        
        return new ArrayListEnumeration(data); 
    }

    /**
    * used to iterate on the element of the data. 
    * if NULL there are no more element
    */
    virtual KeyValuePair* getNextElement() {
        if (counter == 0) { counter++; return (KeyValuePair*)data.front(); }
        else              { counter++; return (KeyValuePair*)data.next();  }
    }
    
    /**
    * reset the counter that is used to get the element
    * in the arraylist. To be used before starting to use the getNextElement
    * method.
    * NOTE. the setProperty and removeProperty uses the getNextElement on the
    * internal data arrayList. So be careful about the usage getNextElement.
    * don't use it when when using the setProperty or readProperty
    */
    virtual void reset() { counter = 0; }


     /**
     * Extract all currently properties in the node
     * It populates the data ArrayList to hold the 
     * key/values.
     * @return 0 - success, failure otherwise
     */
    virtual int read() = 0;

    /**
     * Save the current properties that are
     * in the data arraylist 
     * @return 0 - success, failure otherwise
     */
    virtual int save() = 0;
    
};

/** @} */
/** @endcond */
#endif
