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
#define NL          T("\n")
#define FROM        T("From: ")
#define TO          T("To: ")
#define CC          T("CC: ")
#define BCC         T("BCC: ")
#define DATE        T("Date: ")
#define RECEIVED    T("Received:")
#define SUBJECT     T("Subject: ")
#define MIMETYPE    T("Content-Type: ")
#define MIMEVERS    T("Mime-Version: ")
#define MESSAGEID   T("Message-ID: ")
#define DISPOSITION T("Content-Disposition:")
#define ENCODING    T("Content-Transfer-Encoding: ")

#define MULTIPART   T("multipart/")
#define CHARSET     T("charset=")

// Header names' length
static const unsigned char FROM_LEN = bstrlen(FROM);
static const unsigned char TO_LEN   = bstrlen(TO);
static const unsigned char CC_LEN   = bstrlen(CC);
static const unsigned char BCC_LEN  = bstrlen(BCC);
static const unsigned char DATE_LEN  = bstrlen(DATE);
static const unsigned char SUBJECT_LEN = bstrlen(SUBJECT);
static const unsigned char MIMETYPE_LEN = bstrlen(MIMETYPE);
static const unsigned char MIMEVERS_LEN = bstrlen(MIMEVERS);
static const unsigned char MESSAGEID_LEN = bstrlen(MESSAGEID);
static const unsigned char DISPOSITION_LEN = bstrlen(DISPOSITION);
static const unsigned char ENCODING_LEN = bstrlen(ENCODING);

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

const BCHAR *MailMessage::getTo() const { return to.c_str(); } 
void MailMessage::setTo(const BCHAR *to) { this->to = to; } 

const BCHAR *MailMessage::getFrom() const { return from.c_str(); } 
void MailMessage::setFrom(const BCHAR *from) { this->from = from; } 

const BCHAR *MailMessage::getCc() const { return cc.c_str(); } 
void MailMessage::setCc(const BCHAR *cc) { this->cc = cc; } 

const BCHAR *MailMessage::getBcc() const { return bcc.c_str(); } 
void MailMessage::setBcc(const BCHAR *bcc) { this->bcc = bcc; } 

//        int addHeader(const BCHAR *name, const BCHAR *content);
const BCHAR *MailMessage::getSubject() const { return subject.c_str(); }
void MailMessage::setSubject(const BCHAR *subj) { subject = subj; }

const BasicTime& MailMessage::getDate() const { return date; }
void MailMessage::setDate(const BasicTime& v) { date = v; }

const BasicTime& MailMessage::getReceived() const { return received; }

const BCHAR * MailMessage::getContentType() const { return contentType; }
void MailMessage::setContentType(const BCHAR *val) { contentType = val; }

const BCHAR * MailMessage::getBoundary() const { return boundary; }
void MailMessage::setBoundary(const BCHAR *val) { boundary = val; }

const BCHAR * MailMessage::getMimeVersion() const { return mimeVersion; }
void MailMessage::setMimeVersion(const BCHAR *val) { mimeVersion = val; }

const BCHAR * MailMessage::getMessageId() const { return messageId; }
void MailMessage::setMessageId(const BCHAR *val) { messageId = val; }
        
const BCHAR* MailMessage::getEntryID() { return entryId.c_str(); }
void MailMessage::setEntryID(const BCHAR* id) { entryId = id; }

//BCHAR* setSubjectPrefix
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

    LOG.debug(T("FormatBodyPart START"));

    ret = MIMETYPE; 
    ret += part.getMimeType(); ret += T(";\n");
    if( part.getFilename() ) {
        ret += T("        name=\""); ret += part.getFilename(); ret += T("\"\n");
    }
    if( part.getEncoding() ) {
        ret += ENCODING; ret += part.getEncoding(); ret += NL;
    }
    if( part.getFilename() ) {
        if( part.getDisposition() ) {
            ret += DISPOSITION; ret += part.getDisposition(); ret += T(";\n");
        }
        else {
            ret += DISPOSITION; ret += T("attachment;\n");
        }
        
        ret += T("      filename=\""); ret += part.getFilename();
        ret += T("\"\n");
    }
    // End of part headers
    ret += NL;
    // Content
    if( part.getFilename() ) {
        BCHAR *content = loadAndConvert(part.getContent(), part.getEncoding());
        ret += content;
        delete [] content;
    }
    else
        ret += part.getContent();

    LOG.debug(T("FormatBodyPart END"));
    return ret;
}

inline static size_t findNewLine(StringBuffer &str, size_t offset) {
    size_t nl = str.find(T("\n"), offset)+1;
    if(nl == StringBuffer::npos)
        return nl;
    return (str[nl] == CHR('\r')) ? nl+1 : nl ;
}

static size_t getHeadersLen(StringBuffer &s, StringBuffer &newline)
{
    // detect the newline used in headers
    size_t pos1 = s.find("\n");
    if(pos1 == StringBuffer::npos){
        LOG.error("MailMessage: no newlines in message?");
        return pos1;
    }
    size_t pos2 = pos1 + 1 ;

    while (s[pos1-1] == '\r'){
        pos1--;
    }
    newline = s.substr(pos1, pos2-pos1);
    
    StringBuffer emptyline = newline + newline ;

    // Split headers and body
    size_t hdrlen = s.find(emptyline);
    if(hdrlen == StringBuffer::npos) {
        // Empty body, get the message anyway.
        hdrlen = s.length();
    }
    return hdrlen;
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
    LOG.debug(T("getBodyPart START"));

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
    if(part.ifind(T("Content-Type: multipart/alternative")) != StringBuffer::npos) {
        size_t b_pos = part.ifind(T("boundary="));
        if( b_pos != StringBuffer::npos ) {
			size_t begin = part.find(T("=\""), b_pos) + 2 ;
			size_t end = part.find(T("\""), begin) ;

			StringBuffer inner_boundary(T("\n--"));
            inner_boundary += part.substr( begin, end-begin );

            begin = part.find(inner_boundary, end);
            begin += inner_boundary.length();
            end = part.find(inner_boundary, begin);
            if (begin != StringBuffer::npos && end != StringBuffer::npos) {
                part = part.substr(begin, end-begin);
                LOG.debug(T("Bodypart is multipart/alternative: getting first alternative only."));
                LOG.debug(part);
            }
		}
    }

    StringBuffer newline;

    // Split headers and body
    size_t hdrlen = getHeadersLen(part, newline);

    // Get headers
    StringBuffer headers = part.substr(0, hdrlen);
    ArrayList lines;
    const StringBuffer *line;
    
    // parse the bodypart headers
    headers.split(lines, newline);

    for ( line=(StringBuffer *)lines.front();
		  line;
		  line=(StringBuffer *)lines.next() ) {
        if( *line == T("\r") )
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

        // These ones are parameters, and can appear on the same line.
        if( line->ifind(T("filename=")) != StringBuffer::npos ) {
            size_t begin = line->find("filename=") + bstrlen("filename=");
            size_t end = begin;
            size_t quote = line->find(T("\""), begin);
            if (quote != StringBuffer::npos){
                begin = quote + 1;
                end = line->find(T("\""), begin) ;
            }
            else {
                end = line->find(T(";"), begin) ;
                if (end == StringBuffer::npos) {
                    end = line->find(T(" "), begin);
                }
            }
			ret.setFilename( line->substr(begin, end-begin) );
		}
        else {
            size_t begin=line->ifind(CHARSET);
            if( begin != StringBuffer::npos ) {
                begin += bstrlen(CHARSET);
                size_t end = begin;
                size_t quote = line->find(T("\""), begin);
                if (quote != StringBuffer::npos){
                    begin = quote + 1;
                    end = line->find(T("\""), begin) ;
                }
                else {
                    end = line->find(T(";"), begin) ;
                    if (end == StringBuffer::npos) {
                        end = line->find(T(" "), begin);
                    }
                }
                ret.setCharset( line->substr( begin, end-begin ) );
            }
		}

    }
    // move to the beginning of the content
    hdrlen += bstrlen(newline);
    // get bodypart content 
    if( !ret.getFilename() ) {
		// this is not an attachment
        if( bstrcmp(ret.getEncoding(), T("quoted-printable")) == 0 ) {
            BCHAR *decoded = qp_decode( part.substr(hdrlen) );
            ret.setContent ( decoded );
            delete [] decoded;
        }
        else if ( bstrcmp(ret.getEncoding(), T("base64")) == 0 ) {
            BCHAR *decoded = "";
            size_t len = 0;
            if( uudecode( part.substr(hdrlen), &decoded, &len ) ) {
                LOG.error("Error decoding content");
            }
            ret.setContent ( decoded );
            delete [] decoded;
        }
        else
            ret.setContent ( part.substr(hdrlen) );
    }
    else {
        LOG.debug(T("Attachment"));
        ret.setContent( mkTempFileName( ret.getFilename() ) );
        LOG.debug(ret.getContent());
        StringBuffer p = part.substr(hdrlen);
        if (p.length()) {
            LOG.debug(T("Saving..."));
            if( convertAndSave(ret.getContent(), p.c_str(), ret.getEncoding()) ) {
                LOG.error(T("Error in convertAndSave"));
            }
            else {
                LOG.debug(T("convertAndSave success"));
            }    
        }
	}
    LOG.debug(T("getBodyPart END"));

    // return true if there are more parts
	return (next != StringBuffer::npos);
}

static void generateBoundary(StringBuffer& boundary)
{
    BCHAR buf[40];
	int i;

    *buf = '=';
    memset(buf+1, '-', 9*sizeof(BCHAR));
    for(i=10; i<36; i++) {
        buf[i] = '0' + rand() % 10;
    }
    buf[i]=0;
    boundary = buf;
}

static bool isAscii(const char *str){
    if(!str)
        return true;

    for(size_t i = 0; i < bstrlen(str); i++) {
        if ( ! isprint(str[i]) ){
			return false;
        }
    }
	return true;
}


//----------------------------------------------------------- Public Methods

/**
 * Format a mailmessage in a RFC2822 string
 */
BCHAR * MailMessage::format() {

    StringBuffer ret;

    LOG.debug(T("MailMessage::format START"));

    // If the message is empty, return null
    if ( empty() ) {
        return 0;
    }

    if ( contentType.empty() ) {
        if ( attachments.size() ) {
            contentType = T("multipart/mixed");
        }
        else {
            contentType = body.getMimeType();

            if (headers.size() > 0) {
                StringBuffer *line; int j = 0;
                for (line=(StringBuffer *)headers.front(); line; line=(StringBuffer *)headers.next() ) {                                                  
                    if (bstrstr(line->c_str(), T("format=")) != 0 
                        || bstrstr(line->c_str(),T("reply-type=")) != 0 ) {
                            contentType.append(T("; "));
                            line->replaceAll(T(";"), T(" "));
                            contentType.append(line->c_str());                     
                            headers.removeElementAt(j);
                            j--;
                         }
                     j++;
                }
            }

        }
    }
    if ( mimeVersion.empty() ) {
        mimeVersion = T("1.0");
    }

    // Add generics headers
    ret.join((ArrayList &)headers, NL);
    // Add parsed headers
    ret += MIMEVERS; ret += mimeVersion; ret += NL;
    ret += MESSAGEID; ret += messageId; ret += NL;
    LOG.debug("MailMessage: From: %s\n", from.c_str());
    ret += FROM; ret += from; ret += NL;
    ret += TO; ret += to; ret += NL;
    if (cc.length() ) {
        ret += CC; ret += cc; ret += NL;
    }
    if (bcc.length() ) {
        ret += BCC; ret += bcc; ret += NL;
    }
    ret += DATE; ret += date.formatRfc822(); ret += NL;
    ret += SUBJECT; 
    if (!isAscii(subject))
    {
        // FIXME
        // If the length of the encoded subject is bigger then 75 chars,
        // additional encoded words are needed, as required by rfc2047        
        BCHAR* qp = 0;
        qp = qp_encode(subject.c_str());
        ret += "=?utf-8?Q?";
        ret += qp;
        ret += "?=";
        delete [] qp;
    }
    else {
        ret += subject; 
    }
    ret += NL;
    ret += MIMETYPE; ret += contentType; ret+= T("; ");
    if (contentType.ifind(MULTIPART) != StringBuffer::npos ){
        if ( boundary.empty() ) {
            generateBoundary(boundary);
        }
        ret += T("\n boundary=\""); ret += boundary; 
        ret += T("\"\n\nThis is a multi-part message in MIME format.\n");
        // Prepare a string with the boundary on a line alone
        StringBuffer bound = T("\n--"); bound += boundary;
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
        ret += T("--\n");
    }
    else {
        // Body
        if(body.getCharset())
            ret += CHARSET; ret += body.getCharset(); ret += NL;
        if( body.getEncoding() )
            ret += ENCODING; ret += body.getEncoding();
        // end of headers
        ret += NL;  
        ret += NL;
        ret += body.getContent(); ret += NL;
    }
    LOG.debug(T("MailMessage::format END"));
	return stringdup(ret.c_str());
}


int MailMessage::parse(const BCHAR *rfc2822, size_t len) {
    StringBuffer s(rfc2822, len);
    int rc;
    
    LOG.debug(T("MailMessage::parse START"));

    size_t hdrlen = getHeadersLen(s, newline);

    StringBuffer headers = s.substr(0, hdrlen);
    StringBuffer rfcbody = s.substr(hdrlen);
    
    rc = parseHeaders(headers);
    if(rc)
        return rc;

    if(contentType.ifind(MULTIPART) != StringBuffer::npos) {
        // Multipart message
        rc= parseBodyParts(rfcbody);
    }
    else {
        body.setMimeType(contentType);
        // FIXME: handle all encodings, not only quoted-printable
        if( bstrcmp(body.getEncoding(), T("quoted-printable")) == 0 ) {
            BCHAR *decoded = qp_decode( rfcbody );
            body.setContent ( decoded );
            delete [] decoded;
        }
        else if ( bstrcmp(body.getEncoding(), T("base64")) == 0 ) {
            BCHAR *decoded = NULL;
            size_t len = 0;
            rc = uudecode( rfcbody, &decoded, &len ) ;
            if( !rc ) {
                body.setContent ( decoded );
                delete [] decoded;
            }
        }
        else body.setContent(rfcbody);
    }

    LOG.debug(T("MailMessage::parse END"));
    return rc;
}

BOOL hasLineEncodedWords(StringBuffer line, size_t* startPos, size_t* endPos)
{
    BOOL has = FALSE;
    if (!line.empty()) {
        *startPos = line.find("=?");
        size_t firstMark = line.substr(*startPos + 2).find("?") + *startPos + 2;
        if (StringBuffer::npos != firstMark) {
            size_t secondMark = line.substr(firstMark + 1).find("?") + firstMark + 1;
            if (StringBuffer::npos != secondMark) {
                *endPos = line.substr(secondMark + 1).find("?=") + secondMark + 1;
                if ((StringBuffer::npos != *endPos) && 
                    (StringBuffer::npos != *startPos) && 
                    (*endPos > *startPos)) /*&& 
                    (endPos - startPos) <= 73)*/ // uncomment when the 75 maximum length 
                                                 // for an encoded word fix will be applied
                                                 // in format() method       
                {
                    has = TRUE; LOG.debug("TRUE");
                }        
            }
        }
    }
    return has;
}

StringBuffer decodeWordFromHeader(StringBuffer encodedWord, StringBuffer& charset)
{
    StringBuffer decodedWord;
    unsigned int charsetStart = 2;
    unsigned int encodingStart = encodedWord.substr(charsetStart).find("?") + charsetStart + 1;
    unsigned int encTextStart = encodedWord.substr(encodingStart).find("?") + encodingStart + 1;

    charset = encodedWord.substr(charsetStart, encodingStart - charsetStart - 1);
    StringBuffer encoding = encodedWord.substr(encodingStart, encTextStart - encodingStart - 1);
    StringBuffer encText = encodedWord.substr(encTextStart);
    LOG.debug("1");
    if (_stricmp(encoding.c_str(), "Q") == 0) {
        LOG.debug("2");
        // quoted-printable
        BCHAR* dec = qp_decode(encText.c_str());
        decodedWord.append(dec);
        decodedWord.replaceAll("_", " ");         
    }
    else if (_stricmp(encoding.c_str(), "B") == 0){
        LOG.debug("3");
        // base64
        int len = b64_decode((void *)encText.c_str(), encText.c_str());
        decodedWord.append(encText.substr(0, len));
    }
    else { LOG.debug("4"); }
    return decodedWord;
}


//---------------------------------------------------------- Private Methods

int MailMessage::parseHeaders(StringBuffer &rfcHeaders) {

    ArrayList lines;
    const StringBuffer *line;
    StringBuffer strReceived;
    BOOL firstReceivedMatched = FALSE;
    BOOL receivedExtracted = FALSE;
    BOOL subjectParsing = FALSE;
    LOG.debug(T("parseHeaders START"));

    rfcHeaders.split(lines, newline);

    for ( line=(StringBuffer *)lines.front();
		  line;
		  line=(StringBuffer *)lines.next() ) {
        if( *line == T("\r") )
            break;
        // The first empty line marks the end of the header section
        if( line->empty() ){
            break;
        }
        // Process the headers
        bool unknown=false;
        
        if( line->ifind(TO) == 0 ){
            to = line->substr(TO_LEN);
            subjectParsing = FALSE;
        }
        else if( line->ifind(FROM) == 0 ) {
            from = line->substr(FROM_LEN);
            subjectParsing = FALSE;
        }
        else if( line->ifind(CC) == 0 ) {
            cc = line->substr(CC_LEN);
            subjectParsing = FALSE;
        }
        else if( line->ifind(BCC) == 0 ) {
            bcc = line->substr(BCC_LEN);
            subjectParsing = FALSE;
        }
        else if ( line->ifind(DATE) == 0 ) {
            subjectParsing = FALSE;        
            if( date.parseRfc822(line->substr(DATE_LEN)) ) {
                LOG.error(T("Error parsing date"));
                return 500;
            }
        }
        else if( line->ifind(SUBJECT) == 0 ) {
            subject = line->substr(SUBJECT_LEN); LOG.debug("SUBJECT:"); LOG.debug(subject.c_str());
            subjectParsing = TRUE;        
            size_t startPos, endPos;
            if (!subject.empty()&& (TRUE == hasLineEncodedWords(subject, &startPos, &endPos))) {
                StringBuffer charset;
                StringBuffer decoded = decodeWordFromHeader(subject.substr(startPos, endPos - startPos), charset);
                subject.replace(subject.substr(startPos, endPos + 2), decoded); 
                wchar_t* wsubject = toWideChar(subject.c_str(), charset); 
                subject = toMultibyte(wsubject);                
            }
        }
        else if( line->ifind(ENCODING) == 0 ) {  // it is here for single part only
            body.setEncoding(line->substr(ENCODING_LEN));
            subjectParsing = FALSE;
        }
        else if(line->ifind(MIMEVERS) == 0 ) {
            mimeVersion = line->substr(MIMEVERS_LEN);
            subjectParsing = FALSE;
        }
        else if(line->ifind(MESSAGEID) == 0 ) {
            messageId = line->substr(MESSAGEID_LEN);
            subjectParsing = FALSE;
        }
        else if( line->ifind(MIMETYPE) == 0 ) {
            size_t len = line->find(T(";")) - MIMETYPE_LEN ;
            contentType = line->substr(MIMETYPE_LEN, len);
            subjectParsing = FALSE;
        }            
        else if(line->ifind(RECEIVED) == 0) {
            if (!receivedExtracted) {
                strReceived = line->substr(line->ifind(";") );
                firstReceivedMatched = TRUE;
                unknown = true;
                if (!strReceived.empty())                    
                {
                    received.parseRfc822(strReceived.substr(2));
                    firstReceivedMatched = FALSE;
                    receivedExtracted = TRUE;
                }
            }
        }
        else {
            if (firstReceivedMatched) {
                strReceived = line->substr(line->ifind("; "));
                if (!strReceived.empty())
                {
                    received.parseRfc822(strReceived.substr(2));
                    firstReceivedMatched = FALSE;
                    receivedExtracted = TRUE;
                    unknown = true;
                }
                unknown = true;
            }
            else if (subjectParsing) {
                size_t startPos, endPos;
                StringBuffer tmp = line->c_str();
                if (TRUE == hasLineEncodedWords(tmp, &startPos, &endPos)) {
                    StringBuffer charset;
                    StringBuffer decoded = decodeWordFromHeader(tmp.substr(startPos, endPos - startPos), charset);
                    tmp.replace(tmp.substr(startPos, endPos + 2), decoded); 
                    wchar_t* wline = toWideChar(tmp.c_str(), charset); 
                    tmp = toMultibyte(wline);                
                    subject.append(tmp.substr(1));
                }
            }
            else {
                unknown = true;
            }            
        }            

        // These ones are parameters, and can appear on the same line.
        // FIXME: Should be a sub-parsing of content-type.
        if( line->ifind(T("boundary=")) != StringBuffer::npos ) {
			size_t begin = line->find(T("=\"")) + 2 ;
			size_t end = line->find(T("\""), begin) ;
			boundary = line->substr( begin, end-begin );
		}
        else {
            size_t begin=line->ifind(CHARSET);
            if( begin != StringBuffer::npos ) {
                begin += bstrlen(CHARSET);
                size_t end = begin;
                size_t quote = line->find(T("\""), begin);
                if (quote != StringBuffer::npos){
                    begin = quote + 1;
                    end = line->find(T("\""), begin) ;
                }
                else {
                    end = line->find(T(";"), begin) ;
                    if (end == StringBuffer::npos) {
                        end = line->find(T(" "), begin);
                    }
                }
                body.setCharset( line->substr( begin, end-begin ) );
            }
            else if(unknown) {
                StringBuffer s(line->c_str());
                headers.add(s);
            }
	    }
        
    }
    // If received was not found, copy send date
    if( received == BasicTime() ){
        received = date;
    }
    LOG.debug(T("parseHeaders END"));

	// FIXME: should check for mandatory headers before return 0
	return 0;
}


int MailMessage::parseBodyParts(StringBuffer &rfcBody) {

    BodyPart part;
    // The boundary is the one defined in the headers preceded by
    // a newline and two hypens
    StringBuffer bound(T("\n--"));
    bound += boundary;

    LOG.debug(T("parseBodyParts START"));

    size_t nextBoundary = rfcBody.find(bound);
    getBodyPart(rfcBody, bound, body, nextBoundary);

    if (contentType.ifind("multipart/alternative") == StringBuffer::npos) {        
        // If it's not multipart/alternative, get the other parts
        while( getBodyPart(rfcBody, bound, part, nextBoundary) ) {
            // some problem in the attachment?
            if( part.getContent() ) {
                attachments.add(part);
            }
            else LOG.error("Empty content in attachment.");
        }
    }

    LOG.debug(T("parseBodyParts END"));
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


void MailMessage::setHeaders(const BCHAR* chExtraHeaders) 
{
    if(chExtraHeaders){
        StringBuffer extraHeaders(chExtraHeaders);
        ArrayList lines;
        extraHeaders.split(headers, "\n");
    }
}

/**
 * The result must be deleted by caller
 */
BCHAR* MailMessage::getHeaders()
{
    if( headers.size() ) {
        StringBuffer buff;    
        buff.join(headers, "\n");            
        BCHAR* strHeaders = stringdup(buff.c_str(), buff.length() -1);
        return strHeaders;
    }
    else return 0;
}
