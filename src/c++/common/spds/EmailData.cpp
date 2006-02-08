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


#define EMAIL_READ  TEXT("read")
#define EMAIL_FORW  TEXT("forwarded")
#define EMAIL_REPL  TEXT("replied")
#define EMAIL_TREC  TEXT("received")
#define EMAIL_TCRE  TEXT("created")
#define EMAIL_TMOD  TEXT("modified")
#define EMAIL_DELE  TEXT("deleted")
#define EMAIL_FLAG  TEXT("flagged")
#define EMAIL_ITEM  TEXT("emailitem")

EmailData::EmailData()
{
    read = false;
    forwarded = false;
    replied = false;
    deleted = false;
    flagged = false;
}

int EmailData::parse(const wchar_t *syncmlData, size_t len)
{
    int ret = 0;
    size_t start, end;        
    StringBuffer* s = new StringBuffer(syncmlData, len);

    // FIXME: remove these replace once the server has fixed the message.
    s->replaceAll(TEXT("&lt;"), TEXT("<"));
    s->replaceAll(TEXT("&gt;"), TEXT(">"));
    s->replaceAll(TEXT("&amp;"), TEXT("&"));
    
    // Get the CDATA content
    if(XMLProcessor::getElementContent(s->c_str(), TEXT("CDATA"), NULL, &start, &end) == 0) {
        LOG.error(TEXT("EMailData: can't find outer CDATA section."));
        return -1;
    }
    StringBuffer msg = s->substr(start, end-start);
    
    delete s;

    // Get attributes
    if( XMLProcessor::getElementContent (msg, EMAIL_READ, NULL, &start, &end) ) {
        read = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
    }
    else read = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_FORW, NULL, &start, &end) ) {
        forwarded = ( msg.substr(start, end-start) == TEXT("true") ) ;
    }
    else forwarded = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_REPL, NULL, &start, &end) ) {
        replied = ( msg.substr(start, end-start) == TEXT("true") ) ;
    }
    else replied = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_DELE, NULL, &start, &end) ) {
        deleted = ( msg.substr(start, end-start) == TEXT("true") ) ;
    }
    else deleted = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_FLAG, NULL, &start, &end) ) {
        flagged = ( msg.substr(start, end-start) == TEXT("true") ) ;
    }
    else flagged = false;

    if( XMLProcessor::getElementContent (msg, EMAIL_TREC, NULL, &start, &end) ) {
        received = msg.substr(start, end-start);
    }
    else received = TEXT("");

    if( XMLProcessor::getElementContent (msg, EMAIL_TCRE, NULL, &start, &end) ) {
        created = msg.substr(start, end-start);
    }
    else created = TEXT("");

    if( XMLProcessor::getElementContent (msg, EMAIL_TMOD, NULL, &start, &end) ) {
        modified = msg.substr(start, end-start);
    }
    else modified = TEXT("");    

    // Get content
    if( XMLProcessor::getElementContent (msg, EMAIL_ITEM, NULL, &start, &end) ) {
		StringBuffer item = msg.substr(start, end-start);
        item.replace(TEXT("]]&gt;"), TEXT("]]>"));
        
        if(XMLProcessor::getElementContent(item, TEXT("CDATA"), NULL, &start, &end) == 0) {
            LOG.error(TEXT("EMailData: can't find inner CDATA section."));
            return -1;
        }
        ret=emailItem.parse( item.c_str()+start, end-start );
    }
    else {
        LOG.info(TEXT("EMailData: no <emailitem> tag."));
        // It is not an error, just log it for info.
    }
    return ret;
}

wchar_t *EmailData::format() {
    StringBuffer out;

    out.reserve(150);
    
    out = TEXT("<![CDATA[\n<Email>\n");
    out += XMLProcessor::makeElement(EMAIL_READ, read);
    out += XMLProcessor::makeElement(EMAIL_FORW, forwarded);
    out += XMLProcessor::makeElement(EMAIL_REPL, replied);
    out += XMLProcessor::makeElement(EMAIL_TREC, received);
    out += XMLProcessor::makeElement(EMAIL_TCRE, created);
    out += XMLProcessor::makeElement(EMAIL_TMOD, modified);
    out += XMLProcessor::makeElement(EMAIL_DELE, deleted);
    out += XMLProcessor::makeElement(EMAIL_FLAG, flagged);
    out += TEXT("<emailitem>\n<![CDATA[");
    wchar_t *item = emailItem.format();
    if ( item ) {
        out += item;
        delete [] item;
    }
    out += TEXT("]]&gt;\n</emailitem>\n</Email>\n]]>\n");
    return stringdup(out.c_str());
}

