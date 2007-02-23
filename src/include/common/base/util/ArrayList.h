/*
 * Copyright (C) 2003-2007 Funambol
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


#ifndef INCL_BASE_UTIL_ARRAY_LIST
#define INCL_BASE_UTIL_ARRAY_LIST

#include "base/fscapi.h"

#include "base/util/ArrayElement.h"

/**
 * This class implements a simple linked list that can be accessed by index too.
 * This class does not make use of C++ templates by choice, since it must be 
 * as much easier and portable as possible.
 * 
 * Each list element must be an instance of ArrayElement, which must be 
 * considered an abstract class. Implementing classes must define the destructor
 * and the method clone(), which is used to replicate a given object. This is
 * used by insertion methods: they always clone the item and store the cloned
 * element so that the version inside the list and the caller can independently
 * release their memory. Note that clone methods MUST use the C++ operator to
 * allocate new object, since ArrayList will delete them calling the C++ delete
 * operator.
 */

struct Element {
	ArrayElement* e; // the element value
	Element* n;      // the next element (NULL for the latest)
};

class ArrayList {
	private:
	    Element* head;
	    
        Element* iterator;

        ArrayList& set (const ArrayList & other);
        
	public:
	    ArrayList() EXTRA_SECTION_00;
        ArrayList(const ArrayList &other) EXTRA_SECTION_00;
	    ~ArrayList() EXTRA_SECTION_00;
	    
	    /**
	     * Is this list empty?
	     */
	    bool isEmpty() EXTRA_SECTION_00;
	    
		/**
		 * Adds a new element at the specified index. If index is greater than
		 * the list size the element is appended.
		 * The element is dinamically duplicated so that the caller can release
		 * the element at any time. This method returns the position (0 based) at which
		 * the element has been inserted. It can be different by index if index is out
		 * of the array bounds, in that case element is appended as last element.
		 * It returns -1 in case of errors.
		 * 
		 * @param index the insertion position
		 * @param element the element to insert
		 */
	    int add(int index, ArrayElement& element) EXTRA_SECTION_00;
	    
	    /**
	     * Same as add(index, element, size), but append at the end of the array.
	     * 
	     * @param element the element to insert
	     */
	    int add(ArrayElement& element) EXTRA_SECTION_00;
	    
        /**
        * Add all the ArrayElement of the given ArrayList to the current
        * array list
        */
        int add(ArrayList* list) EXTRA_SECTION_00;

	    /**
	     * Frees the list. All elements are freed as well.
	     */
	    void clear() EXTRA_SECTION_00;
	    
	    /**
	     * Frees the list and all its elements, regardless the value of
	     * autoDeleteElements. 
	     */
	    void clearAll() EXTRA_SECTION_00;
	    
        int removeElementAt(int index);

	    /**
	     * Returns the index-th element of the array or NULL if index is out of
	     * the array bounds. Note that the retuned element will be released at
	     * list destruction. Clone it if it must have a different life cycle.
	     * 
	     * @param index the element position 
	     */
	    ArrayElement* get(int index) EXTRA_SECTION_00;
	    
        /**
         * Returns the first element of the array and set here the internal iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the first element of the array, or NULL if empty.
         */
        ArrayElement* front() EXTRA_SECTION_00;

        /**
         * Returns the next element of the array and increment the internal iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the first element of the array, or NULL if beyond the last.
         */
        ArrayElement* next() EXTRA_SECTION_00;

        /**
         * Returns the previous element of the array and decrement the internal iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the first element of the array, or NULL if before the first.
         */
        ArrayElement* prev() EXTRA_SECTION_00;

        /**
         * Returns the last element of the array and set here the internal iterator.
         *
         * Note that the retuned element will be released at list destruction.
         * Clone it if it must have a different life cycle.
         *
         * @return - the first element of the array, or NULL if empty.
         */
        ArrayElement* back() EXTRA_SECTION_00;

	    /**
	     * Returns the array size.
	     */
	    int size() EXTRA_SECTION_00;
	    
	    /**
	     * Same as get(index)
	     */
	    ArrayElement* operator[] (int index) EXTRA_SECTION_00;

        /**
         * Copy the ArrayList
         */
        ArrayList& operator= (const ArrayList &v) EXTRA_SECTION_00;

		bool remove(int index) EXTRA_SECTION_00;
        /**
        * Clones the arrayList a return a pointer to a new one.
        */
        ArrayList* clone();
	    
};
#endif
