/**
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public	 License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

//
// @author Stefano Fornari @ Funambol
// @version $Id: vCardProperty.h,v 1.1 2006-01-12 11:39:35 nichele Exp $
//
#include <string.h>

#ifndef INCL_PIM_PROPERTY
#define INCL_PIM_PROPERTY

#include "base/fscapi.h"

/**
 * This object represents a property for VCard and ICalendar object
 * (i.e. its value and its parameters)
 */
class vCardProperty {

    // ------------------------------------------------------------ Private data

    private:
        wchar_t* encoding     ;
        wchar_t* language     ;
        wchar_t* value        ;
        wchar_t* chrset       ;

        /**
         * Sets internal members releasing the currently allocated memory (if
         * any was allocate). The passed value is doplicated so that the caller
         * can independently release it.
         *
         * @param property the address of the pointer to set to the new
         *                 allocated memory
         * @param v the value to set into the property
         */
        void set(wchar_t** property, wchar_t* v);

    // -------------------------------------------- Constructors and Destructors
    public:
        /**
         * Creates property without parameters but with the specified value
         */
        vCardProperty (wchar_t* v = NULL);

        ~vCardProperty();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the encoding parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the encoding parameter of this property
         */
        wchar_t* getEncoding (wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the language parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the language parameter of this property
         */
        wchar_t* getLanguage (wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the value parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the value parameter of this property
         */
        wchar_t* getValue (wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the charset parameter of this property
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the charset parameter of this property
         */
        wchar_t* getCharset (wchar_t* buf = NULL, int size = -1);

        /**
         * Sets the encoding parameter of this property
         *
         * @param encoding the encoding to set
         */
        void setEncoding (wchar_t* encoding);

        /**
         * Sets the language parameter of this property
         *
         * @param language the language to set
         */
        void setLanguage (wchar_t* language);

        /**
         * Sets the value parameter of this property
         *
         * @param value the value to set
         */
        void setValue (wchar_t* value);

        /**
         * Sets the charset parameter of this property
         *
         * @param chrset the charset to set
         */
        void setCharset (wchar_t* chrset);


        /**
         * Creates and returns a new Property object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        vCardProperty* clone();
};

#endif
