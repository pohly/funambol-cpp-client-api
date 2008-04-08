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

#ifndef INCL_KEY_VALUE_STORE
#define INCL_KEY_VALUE_STORE
/** @cond API */
/** @addtogroup Client */
/** @{ */

#include "base/fscapi.h"
#include "base/ErrorHandler.h"
#include "base/util/ArrayElement.h"
#include "base/util/Enumeration.h"
#include "spds/constants.h"
#include "spds/SyncItem.h"
#include "spds/SyncStatus.h"
#include "spds/SyncSourceReport.h"


/**
 * This is the interface for the handling of the key/value that
 * has to be written in the storage. It provides methods that has to be 
 * specialized by implementation on filesystem, registry, db...
 */
class KeyValueStore {

private:
                     
protected:  

    /**
    * The completed node where to write the property key/vlue
    */
    StringBuffer node;
    
public:
    
    /**      
     * The name of the general node 
     */
    KeyValueStore(const char* node) { 
        this->node = node;         
    }

    // Destructor
    virtual ~KeyValueStore() {}
      
    /*
     * Returns the value of the given property
     *
     *@param prop - the property name
     *
     *@return   empty value means the property is not found. 
     */
    virtual StringBuffer readPropertyValue(const char *prop) = 0;

    /*
     * Sets a property value 
     *
     * @param prop      - the property name
     * @param value    - the property value (zero terminated string)
     * @param add      - if the property doesn't exist it add. If false doesn't add
     */
    virtual void setPropertyValue(const char *prop, const char *value, bool add = true) = 0;
    
     /**
     * Remove a certain property 
     *
     * @param prop    the name of the property which is to be removed
     */
    virtual void removeProperty(const char *prop) = 0;
     
    /**
     * Get all the properties that are already read from the store.
     */
    virtual Enumeration* getProperties() = 0;

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
