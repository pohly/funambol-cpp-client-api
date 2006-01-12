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

#ifndef INCL_PIM_CONTACT
#define INCL_PIM_CONTACT

#include "base/fscapi.h"
#include "base/util/ArrayList.h"
#include "vocl/VObject.h"
#include "vocl/vCard/BusinessDetail.h"
#include "vocl/vCard/Name.h"
#include "vocl/vCard/Note.h"
#include "vocl/vCard/Title.h"
#include "vocl/vCard/PersonalDetail.h"
#include "vocl/vCard/Phone.h"
#include "vocl/vCard/Email.h"
#include "vocl/vCard/WebPage.h"

#define MAX_TITLES          10
#define MAX_VPROPERTY_VALUE 200
#define BUSINESS            0
#define HOME                1
#define OTHER               2

class Contact : public VObject {

    // ------------------------------------------------------------ Private data

    private:
        Name* name;
        PersonalDetail* personalDetail;
        BusinessDetail* businessDetail;
        ArrayList* notes;

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
        vCardProperty* getPropertyFromVProperty(VProperty* vp);
        VProperty* getVPropertyFromProperty(wchar_t* name, vCardProperty* prop);
        VProperty* composeVAddress(Address* adr);
        Address* composeAddress(VProperty* vp, int type);
        Address* addLabelAddress(VProperty* vp, int type);        

    //--------------------------------------------- Constructors and Destructors

    public:
        Contact();
        ~Contact();

    // ---------------------------------------------------------- Public methods

        /**
         * Returns the UID of this contact
         *
         * @return the uid of this contact or NULL if not specified
         */
        wchar_t* getUID(wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the timezone for this contact
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the timezone for this contact or NULL if not specified
         */
        wchar_t* getTimezone (wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the note for this contact
         *
         * @return the note for this contact or NULL if not specified
         */
        ArrayList* getNotes();

        /**
         * Sets the notes. The given ArrayList is cloned, so that the
         * caller can independently release it as needed.
         */
        void setNotes(ArrayList& list);

        /**
         * Returns the revision of this contact
         *
         * @param buf if not NULL, the value is copied in this buffer
         * @param size buffer size
         *
         * @return the revision of this contact of NULL if not specified
         */
        wchar_t* getRevision(wchar_t* buf = NULL, int size = -1);

        /**
         * Returns the name of this contact
         *
         * @return the name of this contact or NULL if not specified
         */
        Name* getName();

        /**
         * Sets the contact name. The given Name is cloned, so that the
         * caller can independently release it as needed.
         */
        void setName(Name& n);

        /**
         * Returns the business details of this contact
         *
         * @return the business details of this contact of NULL if not specified
         */
        BusinessDetail* getBusinessDetail ();

        /**
         * Sets the business detail. The given BusinessDetail is cloned, so that the
         * caller can independently release it as needed.
         */
        void setBusinessDetail(BusinessDetail& d);

        /**
         * Returns the personal details of this contact
         *
         * @return the personaldetails of this contact or NULL if not specified
         */
        PersonalDetail* getPersonalDetail();

        /**
         * Sets the personal detail. The given PersonalDetail is cloned, so that the
         * caller can independently release it as needed.
         */
        void setPersonalDetail(PersonalDetail& d);

        /**
         * Sets the UID of this contact
         *
         * @param uid the UID to set
         */
        void setUID (wchar_t* uid);

        /**
         * Sets the timezone for this contact
         *
         * @param timezone the timezone to set
         */
        void setTimezone (wchar_t* tz);

        /**
         * Sets the revision of this contact
         *
         * @param revision the revision to set
         */
        void setRevision (wchar_t* revision);
        
        Contact* clone();
        wchar_t* toString();
};

#endif
