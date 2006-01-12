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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

//
// @author Stefano Fornari @ Funambol
// @version $Id: PersonalDetail.h,v 1.1 2006-01-12 11:39:35 nichele Exp $
//


#ifndef INCL_PIM_PERSONAL_DETAIL
#define INCL_PIM_PERSONAL_DETAIL

#include "Address.h"
#include "ContactDetail.h"
#include "vCardProperty.h"

/**
 * An object containing the personal details of a contact
 *
 */
class PersonalDetail {

    // ------------------------------------------------------------ Private data

    private:
        Address*       address      ;
        Address*       otherAddress ;
        ContactDetail* contactDetail;
        vCardProperty* photo        ;
        wchar_t*       spouse       ;
        wchar_t*       children     ;
        wchar_t*       anniversary  ;
        wchar_t*       birthday     ;
        wchar_t*       gender       ;

        /**
         * Sets internal members releasing the currently allocated memory (if
         * any was allocate). The passed value is doplicated so that the caller
         * can independently release it.
         *
         * @param property the address of the pointer to set to the new
         *                 allocated memory
         * @param v the value to set into the property
         */
        void set(wchar_t** p, wchar_t* v);

    // -------------------------------------------- Constructors and Destructors
    public:
        /**
         * Creates an empty list of personal details
         */
        PersonalDetail();
        ~PersonalDetail();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the contact details for this Personal Detail
         *
         * @return the contact details for this Personal Detail
         */
        ContactDetail* getContactDetail();

        /**
         * Sets the contact detail. The given ContactDetail is cloned, so that the
         * caller can independently release it as needed.
         */
        void setContactDetail(ContactDetail& d);

        /**
         * Returns the address for this Personal Detail
         *
         * @return the address for this Personal Detail
         */
        Address* getAddress();

        /**
         * Sets the address. The given Address is cloned, so that the
         * caller can independently release it as needed.
         */
        void setAddress(Address& a);

        /**
         * Returns the other address for this Personal Detail
         *
         * @return the other address for this Personal Detail
         */
        Address* getOtherAddress();

        /**
         * Sets the other address. The given Address is cloned, so that the
         * caller can independently release it as needed.
         */
        void setOtherAddress(Address& p);

        /**
         * Returns the spouse for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the spouse for this Personal Detail
         */
        wchar_t* getSpouse(wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the children for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the children for this Personal Detail
         */
        wchar_t* getChildren(wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the anniversary for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the anniversary for this Personal Detail
         */
        wchar_t* getAnniversary(wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the birthday for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the birthday for this Personal Detail
         */
        wchar_t* getBirthday(wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the gender for this Personal Detail
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the gender for this Personal Detail
         */
        wchar_t* getGender(wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the photo for this Personal Detail
         *
         * @return the photo for this Personal Detail
         */
        vCardProperty* getPhoto();

        /**
         * Sets the photo. The given Property is cloned, so that the
         * caller can independently release it as needed.
         */
        void setPhoto(vCardProperty& p);

        /**
         * Sets the spouse for this Personal Detail
         *
         * @param spouse the spouse to set
         */
        void setSpouse (wchar_t* spouse);

        /**
         * Sets the children for this Personal Detail
         *
         * @param children the children to set
         */
        void setChildren (wchar_t* children);

        /**
         * Sets the anniversary for this Personal Detail
         *
         * @param anniversary the anniversary to set
         */
        void setAnniversary (wchar_t* anniversary);

        /**
         * Sets the birthday for this Personal Detail
         *
         * @param birthday the spouse to set
         */
        void setBirthday (wchar_t* birthday);

        /**
         * Sets the gender for this Personal Detail
         *
         * @param gender the gender to set
         */
        void setGender (wchar_t* gender);

        /**
         * Creates and returns a new PeronalDetail object. The object is created with
         * the C++ new operator and must be deallocated with the delete C++
         * operator
         */
        PersonalDetail* clone();
};

#endif
