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

#include "base/fscapi.h"
#include "base/Log.h"
#include "base/util/XMLProcessor.h"
#include "syncml/formatter/Formatter.h"
#include "spds/EmailData.h"


#define EMAIL_READ  T("read")
#define EMAIL_FORW  T("forwarded")
#define EMAIL_REPL  T("replied")
#define EMAIL_TREC  T("received")
#define EMAIL_TCRE  T("created")
#define EMAIL_TMOD  T("modified")
#define EMAIL_DELE  T("deleted")
#define EMAIL_FLAG  T("flagged")
#define EMAIL_ITEM  T("emailitem")

EmailData::EmailData()
{
    read = false;
    forwarded = false;
    replied = false;
    deleted = false;
    flagged = false;
}

int EmailData::parse(const BCHAR *syncmlData, size_t len)
{
    int ret = 0;
    unsigned int start, end, size;        
    StringBuffer* s = new StringBuffer(syncmlData, len);
    size = s->length();
    // FIXME: remove these replace once the server has fixed the message.
    if ( strstr(s->c_str(), T("&lt;")) != NULL) {
        s->replaceAll(T("&lt;"), T("<"));
        s->replaceAll(T("&gt;"), T(">"));
        s->replaceAll(T("&amp;"), T("&"));
    }
    
    // Get the CDATA content
    if(XMLProcessor::getElementContent(s->c_str(), T("CDATA"), NULL, &start, &end) == 0) {
        LOG.error(T("EMailData: can't find outer CDATA section."));
        return -1;
    }
    /*
    // workaround: if the item was crypted the mail item arrives with <Email> and not in &lt;Email>
    // So the substitution doesn't works fine
    
    if ((size-(end-start)) > 10) {
        wchar_t* ptr = wcsstr(&s->c_str()[end + 1], TEXT("]]>"));
        if (ptr) {
            int l = ptr - &s->c_str()[end];
            end = end + l;
        }

    }
    */
    StringBuffer msg = s->substr(start, end-start);
    
    delete s;

    // Get attributes
    if( XMLProcessor::getElementContent (msg, EMAIL_READ, NULL, &start, &end) ) {
        read = ( bstrncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
    }
    else read = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_FORW, NULL, &start, &end) ) {
        forwarded = ( msg.substr(start, end-start) == T("true") ) ;
    }
    else forwarded = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_REPL, NULL, &start, &end) ) {
        replied = ( msg.substr(start, end-start) == T("true") ) ;
    }
    else replied = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_DELE, NULL, &start, &end) ) {
        deleted = ( msg.substr(start, end-start) == T("true") ) ;
    }
    else deleted = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_FLAG, NULL, &start, &end) ) {
        flagged = ( msg.substr(start, end-start) == T("true") ) ;
    }
    else flagged = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_TREC, NULL, &start, &end) ) {
        received = msg.substr(start, end-start);
    }
    else received = T("");

    if( XMLProcessor::getElementContent (msg, EMAIL_TCRE, NULL, &start, &end) ) {
        created = msg.substr(start, end-start);
    }
    else created = T("");

    if( XMLProcessor::getElementContent (msg, EMAIL_TMOD, NULL, &start, &end) ) {
        modified = msg.substr(start, end-start);
    }
    else modified = T("");    

    // Get content
    if( XMLProcessor::getElementContent (msg, EMAIL_ITEM, NULL, &start, &end) ) {
		StringBuffer item = msg.substr(start, end-start);
        item.replace(T("]]&gt;"), T("]]>"));
        
        if(XMLProcessor::getElementContent(item, T("CDATA"), NULL, &start, &end) == 0) {
            LOG.error(T("EMailData: can't find inner CDATA section."));
            return -1;
        }
        ret=emailItem.parse( item.c_str()+start, end-start );
    }
    else {
        LOG.info(T("EMailData: no <emailitem> tag."));
        // It is not an error, just log it for info.
    }
    return ret;
}

BCHAR *EmailData::format() {
    StringBuffer out;

    out.reserve(150);
    
    out = T("<![CDATA[\n<Email>\n");
    out += XMLProcessor::makeElement(EMAIL_READ, read);
    out += XMLProcessor::makeElement(EMAIL_FORW, forwarded);
    out += XMLProcessor::makeElement(EMAIL_REPL, replied);
    out += XMLProcessor::makeElement(EMAIL_TREC, received);
    out += XMLProcessor::makeElement(EMAIL_TCRE, created);
    out += XMLProcessor::makeElement(EMAIL_TMOD, modified);
    out += XMLProcessor::makeElement(EMAIL_DELE, deleted);
    out += XMLProcessor::makeElement(EMAIL_FLAG, flagged);
    out += T("<emailitem>\n<![CDATA[");
    BCHAR *item = emailItem.format();
    if ( item ) {
        out += item;
        delete [] item;
    }
    out += T("]]&gt;\n</emailitem>\n</Email>\n]]>\n");
    return stringdup(out.c_str());
}

