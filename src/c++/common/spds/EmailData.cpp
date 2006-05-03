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
#include "base/quoted-printable.h"
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

static inline bool checkFlag(const char *xml, const char *field)
{
    size_t start = 0, end = 0;
    bool ret = false;

    if( XMLProcessor::getEscapedElementContent(xml, field, NULL, &start, &end) ) {
        ret = ( bstrncmp(xml+start, T("true"), end-start) == 0 ) ;
    }
    return ret;
}

EmailData::EmailData()
{
    read = false;
    forwarded = false;
    replied = false;
    deleted = false;
    flagged = false;
}

int EmailData::parse(const BCHAR *msg, size_t len)
{
    int ret = 0;
    size_t start, end;
    
    // Get attributes
    read      = checkFlag(msg, EMAIL_READ);
    forwarded = checkFlag(msg, EMAIL_FORW);
    replied   = checkFlag(msg, EMAIL_REPL);
    deleted   = checkFlag(msg, EMAIL_DELE);
    flagged   = checkFlag(msg, EMAIL_FLAG);
     
    if( XMLProcessor::getEscapedElementContent (msg, EMAIL_TREC, NULL, &start, &end) ) {
        received = StringBuffer(msg+start, end-start);
    }
    else received = T("");

    if( XMLProcessor::getEscapedElementContent (msg, EMAIL_TCRE, NULL, &start, &end) ) {
        created = StringBuffer(msg+start, end-start);
    }
    else created = T("");

    if( XMLProcessor::getEscapedElementContent (msg, EMAIL_TMOD, NULL, &start, &end) ) {
        modified = StringBuffer(msg+start, end-start);
    }
    else modified = T("");    

    // Get content
    if( XMLProcessor::getEscapedElementContent(msg, EMAIL_ITEM, NULL, &start, &end) ) {
		StringBuffer item(msg+start, end-start);
        item.replaceAll("&lt;", "<");
        item.replaceAll("&gt;", ">");

        unsigned int startAttr=0, endAttr=0;
        size_t itemlen = end-start;

        if(XMLProcessor::getElementAttributes(msg, EMAIL_ITEM, &startAttr, &endAttr, true)){
            StringBuffer attrlist(msg+startAttr, endAttr-startAttr);
            if(attrlist.ifind("quoted-printable") != StringBuffer::npos) {
                char *decoded = qp_decode(item);
                item = decoded;
                delete [] decoded;
            }
        }
        // item must start with CDATA
        size_t item_start = item.find("![CDATA");
        if(item_start > 10){
            LOG.error(T("EMailData: can't find inner CDATA section."));
            return -1;
        }
        size_t item_end = item.rfind("]]>");
        // If not found, try also the old case
        if(item.length() - item_end > 10){
            item_end = item.rfind("]]&amp;gt;");
            if(item.length() - item_end > 10){
                LOG.error(T("EMailData: can't find CDATA end tag."));
                return -1;
            }
        }
        // okay, move the start pointer to the end of
        item_start += bstrlen("![CDATA[");
        
        ret=emailItem.parse( item.c_str()+item_start, item_end - item_start );

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

