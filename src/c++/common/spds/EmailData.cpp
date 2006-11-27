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
    unsigned int start = 0, end = 0;
    bool ret = false;

    if( XMLProcessor::getElementContent(xml, field, NULL, &start, &end) ) {

        ret = ( strncmp(xml+start, T("true"), end-start) == 0 ) ;
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

/*
* The parse method returns:
* -1: it means the <emailitem> doesn't containt CDATA section. This could be right
*     if the <email> is an update from server of flags only, as <read>, <forwarded>...
      The rightness must be choosen by the caller. It could be or not an error
*
* -2: it means there is an error into the format of the <emailitem>. It must be treated as an error
*       
*/

int EmailData::parse(const char *msg, size_t len)
{
    int ret = 0;
    unsigned int start, end;
    
    // Get attributes
    read      = checkFlag(msg, EMAIL_READ);
    forwarded = checkFlag(msg, EMAIL_FORW);
    replied   = checkFlag(msg, EMAIL_REPL);
    deleted   = checkFlag(msg, EMAIL_DELE);
    flagged   = checkFlag(msg, EMAIL_FLAG);
     
    if( XMLProcessor::getElementContent (msg, EMAIL_TREC, NULL, &start, &end) ) {
        received = StringBuffer(msg+start, end-start);
    }
    else received = T("");

    if( XMLProcessor::getElementContent (msg, EMAIL_TCRE, NULL, &start, &end) ) {
        created = StringBuffer(msg+start, end-start);
    }
    else created = T("");

    if( XMLProcessor::getElementContent (msg, EMAIL_TMOD, NULL, &start, &end) ) {
        modified = StringBuffer(msg+start, end-start);
    }
    else modified = T("");    

    // Get content
    StringBuffer itemtmp(msg);
    start = itemtmp.find("<emailitem");
    end = itemtmp.rfind("</emailitem>");
    if (start != StringBuffer::npos && end != StringBuffer::npos) { 
        itemtmp = NULL;
    //if( XMLProcessor::getElementContent(msg, EMAIL_ITEM, NULL, &start, &end) ) {
		StringBuffer item(msg+start, end-start);        
        unsigned int startAttr=0, endAttr=0;
        size_t itemlen = end-start;

        if(XMLProcessor::getElementAttributes(msg, EMAIL_ITEM, &startAttr, &endAttr, false)){ //currently emailitem is not escaped so false!!
            StringBuffer attrlist(msg+startAttr, endAttr-startAttr);
            if(attrlist.ifind("quoted-printable") != StringBuffer::npos) {
                char *decoded = qp_decode(item);
                item = decoded;
                delete [] decoded;
            }
        }
        // item must start with CDATA
        size_t item_start = item.find("![CDATA");
        if(item_start > 50){ // it could be <emailitem ENC="QUOTED-PRINTABLE"><![CDATA[
            LOG.error(T("EMailData: can't find inner CDATA section."));
            return -1;
        }
        size_t item_end = item.rfind("]]>");
        
        // In emailitem the last &gt; close the CDATA of emailitem tag and is escaped, so it is needed
        // to be found the follow. Usually the first is skipped
        //
        if(item.length() - item_end > 10){
            item_end = item.rfind("]]&gt;");
            if(item.length() - item_end > 10){
                LOG.error(T("EMailData: can't find CDATA end tag."));
                return -2;
            }
        }
        // okay, move the start pointer to the end of
        item_start += strlen("![CDATA[");
        
        ret=emailItem.parse( item.c_str()+item_start, item_end - item_start );

    }
    else {
        LOG.info(T("EMailData: no <emailitem> tag."));
        // It is not an error, just log it for info.
    }
    return ret;
}

char *EmailData::format() {
    StringBuffer out;

    out.reserve(150);
        
    out = T("<Email>\n");
    out += XMLProcessor::makeElement(EMAIL_READ, read);
    out += XMLProcessor::makeElement(EMAIL_FORW, forwarded);
    out += XMLProcessor::makeElement(EMAIL_REPL, replied);
    out += XMLProcessor::makeElement(EMAIL_TREC, received);
    out += XMLProcessor::makeElement(EMAIL_TCRE, created);
    out += XMLProcessor::makeElement(EMAIL_TMOD, modified);
    out += XMLProcessor::makeElement(EMAIL_DELE, deleted);
    out += XMLProcessor::makeElement(EMAIL_FLAG, flagged);
    char *item = emailItem.format();
    if ( item ) {
    out += T("<emailitem>\n<![CDATA[");
        out += item;
        delete [] item;
        out += T("]]&gt;\n</emailitem>\n");
    }
    out += T("</Email>\n");
    return stringdup(out.c_str());
}

