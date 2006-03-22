/*
 * Copyright (C) 2005-2006 Funambol
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


#ifndef INCL_MAIL
#define INCL_MAIL

#include "base/fscapi.h"
#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"
#include "spds/MailMessage.h"

class Email : public ArrayElement {
    
    // ------------------------------------------------------------ Private data
    private:
        bool read;
        bool forwarded;
        bool replied;
        StringBuffer received;
        StringBuffer created;
        StringBuffer modified;
        bool deleted;
        bool flagged;

        MailMessage emailItem;

    public:
        
    // --------------------------------------------------------------- Accessors  
        bool getRead() { return read; }
        void setRead(bool v) { read=v; }

        bool getForwarded() { return forwarded; }
        void setForwarded(bool v) { forwarded=v; }

        bool getReplied() { return replied; }
        void setReplied(bool r) { replied=r; }
        
        const BCHAR * getReceived() { return received; }
        void setReceived(const BCHAR * v) { received=v; }

        const BCHAR * getCreated() { return created; }
        void setCreated(const BCHAR * v) { created=v; }

        const BCHAR * getModified() { return modified; }
        void setModified(const BCHAR * v) { modified=v; }

        bool getDeleted() { return deleted; }
        void setDeleted(bool v) { deleted=v; }

        bool getFlagged() { return flagged; }
        void setFlagged(bool v) { flagged=v; }

        MailMessage& getMailMessage() { return emailItem; }
        void setMailMessage(const MailMessage& v) { emailItem = v; }

    // ---------------------------------------------------------- Public Methods
        int parse(const BCHAR *syncmlData) { return 0; }          // TODO
        BCHAR *format()                     { return wcsdup(T("")); } // TODO

        ArrayElement* clone() { return new Email(*this); }
   
};

#endif

