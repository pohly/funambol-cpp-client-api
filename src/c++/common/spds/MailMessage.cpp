/*
 * Copyright (C) 2003-2006 Funambol
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

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "base/quoted-printable.h"
#include "base/Log.h"
#include "spds/spdsutils.h"
#include "spds/MailMessage.h"

//------------------------------------------------------------------ Defines

// Headers names
#define NL          TEXT("\n")
#define FROM        TEXT("From: ")
#define TO          TEXT("To: ")
#define CC          TEXT("CC: ")
#define BCC         TEXT("BCC: ")
#define DATE        TEXT("Date: ")
#define SUBJECT     TEXT("Subject: ")
#define MIMETYPE    TEXT("Content-Type: ")
#define MIMEVERS    TEXT("Mime-Version: ")
#define MESSAGEID   TEXT("Message-ID: ")
#define DISPOSITION TEXT("Content-Disposition:")
#define ENCODING    TEXT("Content-Transfer-Encoding: ")

#define MULTIPART   TEXT("multipart/")
#define CHARSET     TEXT("charset=")

// Header names' length
static const unsigned char FROM_LEN = wcslen(FROM);
static const unsigned char TO_LEN   = wcslen(TO);
static const unsigned char CC_LEN   = wcslen(CC);
static const unsigned char BCC_LEN  = wcslen(BCC);
static const unsigned char DATE_LEN  = wcslen(DATE);
static const unsigned char SUBJECT_LEN = wcslen(SUBJECT);
static const unsigned char MIMETYPE_LEN = wcslen(MIMETYPE);
static const unsigned char MIMEVERS_LEN = wcslen(MIMEVERS);
static const unsigned char MESSAGEID_LEN = wcslen(MESSAGEID);
static const unsigned char DISPOSITION_LEN = wcslen(DISPOSITION);
static const unsigned char ENCODING_LEN = wcslen(ENCODING);

#if 0
//-------------------------------------------------------------- Contructors
MailMessage::MailMessage()
  : to(), from(), cc(), bcc(), subject(),
    contentType(), boundary(), mimeVersion(),
    headers(), entryId(), lastModificationTime(0), body(), alt(0),
    attachments() {
}

MailMessage::MailMessage(MailMessage &m)
  : to(m.to), from(m.from), cc(m.cc), bcc(m.bcc), subject(m.subject),
    contentType(m.contentType), boundary(m.boundary), mimeVersion(m.mimeVersion),
    headers(m.headers), entryId(m.entryId),
    lastModificationTime(m.lastModificationTime), body(m.body),
    attachments(m.attachments) {
    alt = new BodyPart(m.alt);
}

MailMessage::~MailMessage() {
    if (alt)
        delete alt;
}
#endif

//---------------------------------------------------------------- Accessors

const wchar_t *MailMessage::getTo() const { return to.c_str(); } 
void MailMessage::setTo(const wchar_t *to) { this->to = to; } 

const wchar_t *MailMessage::getFrom() const { return from.c_str(); } 
void MailMessage::setFrom(const wchar_t *from) { this->from = from; } 

const wchar_t *MailMessage::getCc() const { return cc.c_str(); } 
void MailMessage::setCc(const wchar_t *cc) { this->cc = cc; } 

const wchar_t *MailMessage::getBcc() const { return bcc.c_str(); } 
void MailMessage::setBcc(const wchar_t *bcc) { this->bcc = bcc; } 

//        int addHeader(const wchar_t *name, const wchar_t *content);
const wchar_t *MailMessage::getSubject() const { return subject.c_str(); }
void MailMessage::setSubject(const wchar_t *subj) { subject = subj; }

const BasicTime& MailMessage::getDate() const { return date; }
void MailMessage::setDate(const BasicTime& v) { date = v; }

const wchar_t * MailMessage::getContentType() const { return contentType; }
void MailMessage::setContentType(const wchar_t *val) { contentType = val; }

const wchar_t * MailMessage::getBoundary() const { return boundary; }
void MailMessage::setBoundary(const wchar_t *val) { boundary = val; }

const wchar_t * MailMessage::getMimeVersion() const { return mimeVersion; }
void MailMessage::setMimeVersion(const wchar_t *val) { mimeVersion = val; }

const wchar_t * MailMessage::getMessageId() const { return messageId; }
void MailMessage::setMessageId(const wchar_t *val) { messageId = val; }
        
const wchar_t* MailMessage::getEntryID() { return entryId.c_str(); }
void MailMessage::setEntryID(const wchar_t* id) { entryId = id; }

//wchar_t* setSubjectPrefix
//void setSenderName
//long setMessageSize
/*
time_t MailMessage::getLastModificationTime () {
	return lastModificationTime;
}

void MailMessage::setLastModificationTime (time_t time) {
	lastModificationTime = time;
} 
  
*/

BodyPart & MailMessage::getBody() { return body; }
void MailMessage::setBody(BodyPart &body) { this->body = body; }

// Alternate representation
//BodyPart * MailMessage::getAlternate() { return alternate; };
//void MailMessage::setAlternate(BodyPart &alt) { alternate = new BodyPart(alt); };

BodyPart * MailMessage::getFirstAttachment() {
    return (BodyPart *)attachments.front();
}

BodyPart * MailMessage::getNextAttachment() {
    return (BodyPart *)attachments.next();
}

int MailMessage::addAttachment(BodyPart &body) {
    return attachments.add(body);
}

int MailMessage::attachmentCount() {
    return attachments.size();
}

//----------------------------------------------------------- Static Functions

static StringBuffer formatBodyPart(const BodyPart &part) 
{
    StringBuffer ret;

    LOG.debug(TEXT("FormatBodyPart START"));

    ret = MIMETYPE; 
    ret += part.getMimeType(); ret += TEXT(";\n");
    if( part.getFilename() ) {
        ret += TEXT("        name=\""); ret += part.getFilename(); ret += TEXT("\"\n");
    }
    if( part.getEncoding() ) {
        ret += ENCODING; ret += part.getEncoding(); ret += NL;
    }
    if( part.getFilename() ) {
        if( part.getDisposition() ) {
            ret += DISPOSITION; ret += part.getDisposition(); ret += TEXT(";\n");
        }
        else {
            ret += DISPOSITION; ret += TEXT("attachment;\n");
        }
        
        ret += TEXT("      filename=\""); ret += part.getFilename();
        ret += TEXT("\"\n");
    }
    // End of part headers
    ret += NL;
    // Content
    if( part.getFilename() ) {
        wchar_t *content = loadAndConvert(part.getContent(), part.getEncoding());
        ret += content;
        delete [] content;
    }
    else
        ret += part.getContent();

    LOG.debug(TEXT("FormatBodyPart END"));
    return ret;
}

inline static size_t findNewLine(StringBuffer &str, size_t offset) {
    size_t nl = str.find(TEXT("\n"), offset)+1;
    if(nl == StringBuffer::npos)
        return nl;
    return (str[nl] == CHR('\r')) ? nl+1 : nl ;
}

/**
 * Get the next bodypart from the message body string.
 *
 * @param rfcBody  (in)  - message content
 * @param boundary (in)  - mime boundary string
 * @param ret      (out) - parsed BodyPart
 * @param next     (i/o) - offset of the new boundary
 */
static bool getBodyPart(StringBuffer &rfcBody, StringBuffer &boundary,
                       BodyPart &ret, size_t &next)
{   
    LOG.debug(TEXT("getBodyPart START"));

    // FIXME: check empty value 
    const wchar_t *newline = TEXT("\n");

    // The part starts on the next line
    size_t begin = findNewLine(rfcBody, next);
    if (begin == StringBuffer::npos)
       return false;
    // find the end of the part
    next = rfcBody.find(boundary, begin);
    if (next == StringBuffer::npos)
       return false;
    // get the part
    StringBuffer part = rfcBody.substr(begin, next-begin);
    // If it is a multipart alternative part, get the text part only.
    if(part.ifind(TEXT("Content-Type: multipart/alternative")) != StringBuffer::npos) {
        size_t b_pos = part.ifind(TEXT("boundary="));
        if( b_pos != StringBuffer::npos ) {
			size_t begin = part.find(TEXT("=\""), b_pos) + 2 ;
			size_t end = part.find(TEXT("\""), begin) ;

			StringBuffer inner_boundary(TEXT("\n--"));
            inner_boundary += part.substr( begin, end-begin );

            begin = part.find(inner_boundary, end);
            begin += inner_boundary.length();
            end = part.find(inner_boundary, begin);
            if (begin != StringBuffer::npos && end != StringBuffer::npos) {
                part = part.substr(begin, end-begin);
                LOG.debug(TEXT("Bodypart is multipart/alternative: getting first alternative only."));
                LOG.debug(part);
            }                
		}
    }
    // Split headers and body
    size_t hdrlen = part.find(TEXT("\n\n"));
    if(hdrlen == StringBuffer::npos) {
        // No double newline, try CRLF
        hdrlen = part.find(TEXT("\r\n\r\n"));
        if(hdrlen == StringBuffer::npos) {
            // FIXME: handle abnormal CR CR LF case from server
            hdrlen = part.find(TEXT("\r\r\n\r\r\n"));
            if(hdrlen == StringBuffer::npos) {
                return false;      // No part body ?
            }
            else {
                newline = TEXT("\r\r\n");
            }
        }
        else {
            newline = TEXT("\r\n");
        }
    }
    // Get headers
    StringBuffer headers = part.substr(0, hdrlen);
    ArrayList lines;
    const StringBuffer *line;
    
    // parse the bodypart headers
    headers.split(lines, newline);

    for ( line=(StringBuffer *)lines.front();
		  line;
		  line=(StringBuffer *)lines.next() ) {
        if( *line == TEXT("\r") )
            continue;
        // The first empty line marks the end of the header section
        //if( line->empty() ){
        //    break;
        //}
        // Process the headers
        if( line->ifind(MIMETYPE) == 0 )
           ret.setMimeType(line->substr(MIMETYPE_LEN));
        else if( line->ifind(DISPOSITION) == 0 )
           ret.setDisposition(line->substr(DISPOSITION_LEN));
        else if( line->ifind(ENCODING) == 0 )
           ret.setEncoding(line->substr(ENCODING_LEN));
        else if( line->ifind(TEXT("filename=")) != StringBuffer::npos ) {
			size_t begin = line->find(TEXT("=\"")) + 2 ;
			size_t end = line->find(TEXT("\""), begin) ;
			ret.setFilename( line->substr(begin, end-begin) );
		}
        else {
            size_t begin=line->ifind(CHARSET);
            if( begin != StringBuffer::npos ) {
			    begin += wcslen(CHARSET) ;
		        size_t end = line->ifind(TEXT(" "), begin) ;
                ret.setCharset( line->substr( begin, end-begin ) );
            }
		}
    }
    // move to the beginning of the content
    hdrlen += wcslen(newline);
    // get bodypart content 
    if( !ret.getFilename() ) {
		// this is not an attachment
        // FIXME: handle all encodings, not only quoted-printable
        if( wcscmp(ret.getEncoding(), TEXT("quoted-printable")) == 0 ) {
            wchar_t *decoded = qp_decode( part.substr(hdrlen) );
            ret.setContent ( decoded );
            delete [] decoded;
        }
        else
            ret.setContent ( part.substr(hdrlen) );
    }
    else {
        LOG.debug(TEXT("Attachment"));
        ret.setContent( mkTempFileName( ret.getFilename() ) );
        LOG.debug(ret.getContent());
        StringBuffer p = part.substr(hdrlen);
        if (p.length()) {
            LOG.debug(TEXT("Saving..."));
            if( convertAndSave(ret.getContent(), p.c_str(), ret.getEncoding()) ) {
                LOG.error(TEXT("Error in convertAndSave"));
            }
            else {
                LOG.debug(TEXT("convertAndSave success"));
            }    
        }
	}
    LOG.debug(TEXT("getBodyPart END"));

    // return true if there are more parts
	return (next != StringBuffer::npos);
}




//----------------------------------------------------------- Public Methods

void generateBoundary(StringBuffer& boundary)
{
    wchar_t buf[40];
	int i;

    *buf = '=';
    wmemset(buf+1, '-', 9);
    for(i=10; i<36; i++) {
        buf[i] = '0' + rand() % 10;
    }
    buf[i]=0;
    boundary = buf;
}

/**
 * Format a mailmessage in a RFC2822 string
 */
wchar_t * MailMessage::format() {

    StringBuffer ret;

    LOG.debug(TEXT("MailMessage::format START"));

    // If the message is empty, return null
    if ( empty() ) {
        return 0;
    }

    if ( contentType.empty() ) {
        if ( attachments.size() ) {
            contentType = TEXT("multipart/mixed");
        }
        else {
            contentType = body.getMimeType();
        }
    }
    if ( mimeVersion.empty() ) {
        mimeVersion = TEXT("1.0");
    }

    // Add generics headers
    ret.join((ArrayList &)headers, NL);
    // Add parsed headers
    ret += MIMEVERS; ret += mimeVersion; ret += NL;
    ret += MESSAGEID; ret += messageId; ret += NL;
    ret += FROM; ret += from; ret += NL;
    ret += TO; ret += to; ret += NL;
    if (cc.length() ) {
        ret += CC; ret += cc; ret += NL;
    }
    if (bcc.length() ) {
        ret += BCC; ret += bcc; ret += NL;
    }
    ret += DATE; ret += date.formatRfc822(); ret += NL;
    ret += SUBJECT; ret += subject; ret += NL;
    ret += MIMETYPE; ret += contentType; ret+= TEXT("; ");
    if (contentType.ifind(MULTIPART) != StringBuffer::npos ){
        if ( boundary.empty() ) {
            generateBoundary(boundary);
        }
        ret += TEXT("\n boundary=\""); ret += boundary; 
        ret += TEXT("\"\n\nThis is a multi-part message in MIME format.\n");
        // Prepare a string with the boundary on a line alone
        StringBuffer bound = TEXT("\n--"); bound += boundary;
        // Body
        ret += bound; ret += NL;
        ret += formatBodyPart(body);
        ret += bound;
        // Attachments
        const BodyPart *part;
        for ( part=(const BodyPart *)attachments.front();
              part;
              part=(BodyPart *)attachments.next() ) {
            ret += NL;
            ret += formatBodyPart(*part);
            ret += bound; 
        }
        ret += TEXT("--\n");
    }
    else {
        // Body
        if(body.getCharset())
            ret += TEXT(" charset="); ret += body.getCharset(); ret += NL;
        if( body.getEncoding() )
            ret += ENCODING; ret += body.getEncoding();
        // end of headers
        ret += NL;  
        ret += NL;
        ret += body.getContent(); ret += NL;
    }
    LOG.debug(TEXT("MailMessage::format END"));
	return stringdup(ret.c_str());
}


int MailMessage::parse(const wchar_t *rfc2822, size_t len) {
    StringBuffer s(rfc2822, len);
    int rc;
    
    LOG.debug(TEXT("MailMessage::parse START"));

    // set default newline sequence
    newline = TEXT("\n");

    // Split headers and body
    size_t hdrlen = s.find(TEXT("\n\n"));
    if(hdrlen == StringBuffer::npos) {
        // No double newline, try CRLF
        hdrlen = s.find(TEXT("\r\n\r\n"));
        if(hdrlen == StringBuffer::npos) {
            // FIXME: Try broken CR CR NL sequence
            hdrlen = s.find(TEXT("\r\r\n\r\r\n"));
            if(hdrlen == StringBuffer::npos) 
                return -1;      // No body ?
            else
                newline = TEXT("\r\r\n");
        }
        else newline = TEXT("\r\n"); // Adjust newline   
    }

    StringBuffer headers = s.substr(0, hdrlen);
    StringBuffer rfcbody = s.substr(hdrlen);
    
    rc = parseHeaders(headers);
    if(rc)
        return rc;

    if(contentType.ifind(MULTIPART) != StringBuffer::npos) {
        // Multipart message
        if (!rc) {
            rc= parseBodyParts(rfcbody);
        }
    }
    else {
        body.setMimeType(contentType);
        // FIXME: handle all encodings, not only quoted-printable
        if( wcscmp(body.getEncoding(), TEXT("quoted-printable")) == 0 ) {
            wchar_t *decoded = qp_decode( rfcbody );
            body.setContent ( decoded );
            delete [] decoded;
        }
        else body.setContent(rfcbody);
    }

    LOG.debug(TEXT("MailMessage::parse END"));
    return rc;
}

//---------------------------------------------------------- Private Methods
int MailMessage::parseHeaders(StringBuffer &rfcHeaders) {
    ArrayList lines;
    const StringBuffer *line;
    
    LOG.debug(TEXT("parseHeaders START"));

    rfcHeaders.split(lines, newline);

    for ( line=(StringBuffer *)lines.front();
		  line;
		  line=(StringBuffer *)lines.next() ) {
        if( *line == TEXT("\r") )
            break;
        // The first empty line marks the end of the header section
        if( line->empty() ){
            break;
        }
        // Process the headers
        bool unknown=false;

        if( line->ifind(TO) == 0 )
            to = line->substr(TO_LEN);
        else if( line->ifind(FROM) == 0 )
            from = line->substr(FROM_LEN);
        else if( line->ifind(CC) == 0 )
            cc = line->substr(CC_LEN);
        else if( line->ifind(BCC) == 0 )
            bcc = line->substr(BCC_LEN);
        else if ( line->ifind(DATE) == 0 )
            date.parseRfc822( line->substr(DATE_LEN) );
        else if( line->ifind(SUBJECT) == 0 )
            subject = line->substr(SUBJECT_LEN);
        else if( line->ifind(ENCODING) == 0 )   // it is here for single part only
            body.setEncoding(line->substr(ENCODING_LEN));
        else if(line->ifind(MIMEVERS) == 0 )
            mimeVersion = line->substr(MIMEVERS_LEN);
        else if(line->ifind(MESSAGEID) == 0 )
            mimeVersion = line->substr(MESSAGEID_LEN);
        else
            if( line->ifind(MIMETYPE) == 0 ) {
                size_t len = line->find(TEXT(";")) - MIMETYPE_LEN ;
                contentType = line->substr(MIMETYPE_LEN, len);
            }
        else
            unknown=true;

        // These one are parameters, and can appear on the same line.
        // FIXME: Should be a sub-parsing of charset.
        if( line->ifind(TEXT("boundary=")) != StringBuffer::npos ) {
			size_t begin = line->find(TEXT("=\"")) + 2 ;
			size_t end = line->find(TEXT("\""), begin) ;
			boundary = line->substr( begin, end-begin );
		}
        else {
            size_t begin=line->ifind(CHARSET);
            if( begin != StringBuffer::npos ) {
                begin += wcslen(TEXT("charset="));
		        size_t end = line->find(TEXT(" "), begin) ;
                body.setCharset( line->substr( begin, end-begin ) );
            }
            else if(unknown) {
                StringBuffer s(line->c_str());
                headers.add(s);
            }
	    }
        
    }
    LOG.debug(TEXT("parseHeaders END"));

	// FIXME: should check for mandatory headers before return 0
	return 0;
}


int MailMessage::parseBodyParts(StringBuffer &rfcBody) {

    BodyPart part;
    // The boundary is the one defined in the headers preceded by
    // a newline and two hypens
    StringBuffer bound(TEXT("\n--"));
    bound += boundary;

    LOG.debug(TEXT("parseBodyParts START"));

    size_t nextBoundary = rfcBody.find(bound);
    getBodyPart(rfcBody, bound, body, nextBoundary);

    while( getBodyPart(rfcBody, bound, part, nextBoundary) ) {
        // some problem in the attachment?
        if( part.getContent() )
            attachments.add(part);
    }
    LOG.debug(TEXT("parseBodyParts END"));
    return 0;
}

// Return true if the instance is empty
bool MailMessage::empty() {
    MailMessage emptymsg;
    return ( memcmp(this, &emptymsg, sizeof(MailMessage)) == 0);
}

ArrayElement* MailMessage::clone() {
    return new MailMessage(*this);
}

