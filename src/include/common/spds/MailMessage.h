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
#ifndef INCL_MAIL_MESSAGE
    #define INCL_MAIL_MESSAGE

#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
//#include "base/util/UTF8StringBuffer.h"
#include "base/util/BasicTime.h"
#include "spds/BodyPart.h"

/**
 * This class is represent a mail message, and can parse/format
 * it in RFC2822/MIME encoding.
 * Some of the RFC2822 headers are handles as clas fields (To,
 * From, etc.), the others are inserted literally in the headers
 * arraylist. Any string put in this list is inserted in the 
 * header section of an outgoing message.
 */

class MailMessage : public ArrayElement {

    private:
        // Used to store the newline format used in the incoming message 
        StringBuffer newline;

        //---------------------------------------- Message Headers
        StringBuffer to;
        StringBuffer from;
        StringBuffer cc;
        StringBuffer bcc;

        StringBuffer subject;

        BasicTime date;

        StringBuffer contentType;
        StringBuffer boundary;
        StringBuffer mimeVersion;
        StringBuffer messageId;

        ArrayList headers;

		// can be used by the client to store an internal msg id
		StringBuffer entryId;
        //
		//time_t lastModificationTime;

        BodyPart body;
        //BodyPart *alternate;
        ArrayList attachments;

        int parseHeaders(StringBuffer &rfcHeaders);
        int parseBodyParts(StringBuffer &rfcBody);

    public:

        //MailMessage();
        //MailMessage(MailMessage &);
        //~MailMessage();

        // Headers
        const BCHAR *getTo() const;
        void setTo(const BCHAR *to);

        const BCHAR *getFrom() const;
        void setFrom(const BCHAR *from);

        const BCHAR *getCc() const;
        void setCc(const BCHAR *cc);

        const BCHAR *getBcc() const ;
        void setBcc(const BCHAR *bcc);

        const BCHAR *getSubject() const ;
        void setSubject(const BCHAR *subj);
		
        const BasicTime& getDate() const ;
        void setDate(const BasicTime& d);
		
        const BCHAR * getContentType() const ;
        void setContentType(const BCHAR *val);

        const BCHAR * getBoundary() const ;
        void setBoundary(const BCHAR *val);

        const BCHAR * getMimeVersion() const ;
        void setMimeVersion(const BCHAR *val);
        
        const BCHAR * getMessageId() const ;
        void setMessageId(const BCHAR *val);
        
		const BCHAR* getEntryID();
		void setEntryID(const BCHAR* id);

        // Body
		BodyPart & getBody();
		void setBody(BodyPart &body);
        //BodyPart * getAlternate();
        //void setAlternate(BodyPart &alt);
		
        // Attachment
        BodyPart * getFirstAttachment() ;
        BodyPart * getNextAttachment() ;
        int addAttachment(BodyPart &att);
        int attachmentCount();

        // Conversion methods
        BCHAR *format();
        int parse(const BCHAR *rfc2822, size_t len = StringBuffer::npos);

        /*
         * Return true if the message is empty
         */
        bool empty();

        ArrayElement* clone();

};

#endif
