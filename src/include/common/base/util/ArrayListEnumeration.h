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

#ifndef INCL_ARRAY_LIST_ENUMERATION
#define INCL_ARRAY_LIST_ENUMERATION

#include "base/util/ArrayList.h"
#include "base/util/Enumeration.h"

/**
 * An implementation of the ArrayEnumaration based on 
 * the ArrayList class
 */
class ArrayListEnumeration : public Enumeration {

    private:
        
        /**
        * This is the list on which to iterate. It is an own copy
        * of the passed one.
        */
        ArrayList list;
        
        /**
        * to store the size of the array list make faster
        * the check on the size of the list               
        */ 
        int arraySize;
        
        /**
        * To trace the index of the enumeration
        */
        int counter; 

    public: 
        
        ArrayListEnumeration() {             
            counter   = 0; 
            arraySize = list.size();
        }
    
        ArrayListEnumeration(ArrayList l) { 
            list      = l; 
            counter   = 0; 
            arraySize = list.size();
        }
    
        virtual ~ArrayListEnumeration() {}
        /**
         * Return true if there are more elements in the enumeration.
         */
	
	    bool hasMoreElement() {
	        return (counter == arraySize) ? false : true;	       
	    }

        /**
         * Return the next element
         */
	    ArrayElement* getNextElement() { 
	        if (counter == 0) { counter++;  return list.front(); }
	        else              { counter++;  return list.next();  }
	    }

        
};

#endif

