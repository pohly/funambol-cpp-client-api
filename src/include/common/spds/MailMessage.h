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
#include "base/util/UTF8StringBuffer.h"
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

        ArrayList headers;

		// can be used by the client to store an internal msg id
		StringBuffer entryId;
        
		time_t lastModificationTime;

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
        const wchar_t *getTo() const;
        void setTo(const wchar_t *to);

        const wchar_t *getFrom() const;
        void setFrom(const wchar_t *from);

        const wchar_t *getCc() const;
        void setCc(const wchar_t *cc);

        const wchar_t *getBcc() const ;
        void setBcc(const wchar_t *bcc);

        const wchar_t *getSubject() const ;
        void setSubject(const wchar_t *subj);
		
        const BasicTime& getDate() const ;
        void setDate(const BasicTime& d);
		
        const wchar_t * getContentType() const ;
        void setContentType(const wchar_t *val);

        const wchar_t * getBoundary() const ;
        void setBoundary(const wchar_t *val);

        const wchar_t * getMimeVersion() const ;
        void setMimeVersion(const wchar_t *val);
        
		const wchar_t* getEntryID();
		void setEntryID(const wchar_t* id);

        //wchar_t* setSubjectPrefix
		//void setSenderName
		//long setMessageSize

		time_t getLastModificationTime ();
	    void setLastModificationTime (time_t time);
//        int addHeader(const wchar_t *name, const wchar_t *content);
        
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
        wchar_t *format();
        int parse(const wchar_t *rfc2822, size_t len = StringBuffer::npos);

        /*
         * Return true if the message is empty
         */
        bool empty();

        ArrayElement* clone();

};

#endif
