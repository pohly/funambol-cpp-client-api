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
#include "spds/FileData.h"


//#define EMAIL_ITEM  TEXT("emailitem")

#define FILE_ITEM       TEXT("File")
#define FILE_HIDDEN     TEXT("h")
#define FILE_SYSTEM     TEXT("s") 
#define FILE_ARCHIVED   TEXT("a")
#define FILE_DELETE     TEXT("d")
#define FILE_WRITABLE   TEXT("w")
#define FILE_READABLE   TEXT("r")
#define FILE_EXECUTABLE TEXT("e")
#define FILE_ACCESSED   TEXT("accessed")
#define FILE_ATTRIBUTES TEXT("attributes")
#define FILE_BODY       TEXT("body")
#define FILE_CTTYTPE    TEXT("cttype")
#define FILE_ENC        TEXT("enc")
#define FILE_MODIFIED   TEXT("modified")
#define FILE_NAME       TEXT("name")
#define FILE_SIZE       TEXT("size")
#define FILE_CREATED    TEXT("created")


FileData::FileData()
{
    size = 0;
	hidden = false;
	system = false;
	archived = false;
	deleted = false;
	writable = false;
	readable = false;
	executable = false;
    isHiddenPresent = false;
    isSystemPresent = false;
    isArchivedPresent = false;
    isDeletedPresent = false;
    isWritablePresent = false;
    isReadablePresent = false;
    isExecutablePresent = false;

}

FileData::~FileData()
{
    accessed.reset();
    attributes.reset();
    
    enc.reset();
    file.reset();
    modified.reset();
    name.reset();
    created.reset();
    body.reset();    
    cttype.reset();
            
}
int FileData::parse(const wchar_t *syncmlData, size_t len)
{
    int ret = 0;
    size_t start, end;        
    StringBuffer* s = new StringBuffer(syncmlData, len);
    StringBuffer bodyattr;

    // FIXME: remove these replace once the server has fixed the message.
    s->replaceAll(TEXT("&lt;"), TEXT("<"));
    s->replaceAll(TEXT("&gt;"), TEXT(">"));
    s->replaceAll(TEXT("&amp;"), TEXT("&"));
    
    // Get the CDATA content
    if(XMLProcessor::getElementContent(s->c_str(), TEXT("CDATA"), NULL, &start, &end) == 0) {
        LOG.error(TEXT("FileData: can't find outer CDATA section."));
        return -1;
    }
    StringBuffer msg = s->substr(start, end-start);
    
    delete s;

    // Get attributes
    if( XMLProcessor::getElementContent (msg, FILE_HIDDEN, NULL, &start, &end) ) {
        hidden = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
        isHiddenPresent = true;
    }
    else hidden = false;

    if( XMLProcessor::getElementContent (msg, FILE_SYSTEM, NULL, &start, &end) ) {
        system = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
        isSystemPresent = true;
    }
    else system = false;

    if( XMLProcessor::getElementContent (msg, FILE_ARCHIVED, NULL, &start, &end) ) {
        archived = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
        isArchivedPresent = true;
    }
    else archived = false;
    
    if( XMLProcessor::getElementContent (msg, FILE_DELETE, NULL, &start, &end) ) {
        deleted = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
        isDeletedPresent = true;
    }
    else deleted = false;

    if( XMLProcessor::getElementContent (msg, FILE_WRITABLE, NULL, &start, &end) ) {
        writable = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
        isWritablePresent = true;
    }
    else writable = false;

    if( XMLProcessor::getElementContent (msg, FILE_READABLE, NULL, &start, &end) ) {
        readable = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
        isReadablePresent = true;
    }
    else readable = false;

    if( XMLProcessor::getElementContent (msg, FILE_EXECUTABLE, NULL, &start, &end) ) {
        executable = ( wcsncmp(msg.c_str()+start, TEXT("true"), end-start) == 0 ) ;
        isExecutablePresent = true;
    }
    else executable = false;
    
    if( XMLProcessor::getElementContent (msg, FILE_ACCESSED, NULL, &start, &end) ) {
        accessed = msg.substr(start, end-start);
    }
    else accessed = TEXT("");

    if( XMLProcessor::getElementContent (msg, FILE_MODIFIED, NULL, &start, &end) ) {
        modified = msg.substr(start, end-start);
    }
    else modified = TEXT("");

    if( XMLProcessor::getElementContent (msg, FILE_CREATED, NULL, &start, &end) ) {
        created = msg.substr(start, end-start);
    }
    else created = TEXT("");

    if( XMLProcessor::getElementContent (msg, FILE_BODY, NULL, &start, &end) ) {
        body = msg.substr(start, end-start);
    }
    else body = TEXT("");
    if ( XMLProcessor::getElementAttributes (msg, FILE_BODY, &start, &end) ) {
        bodyattr = msg.substr(start, end-start);
        size_t attrstart = bodyattr.ifind(TEXT("enc"));
        if (attrstart!= StringBuffer::npos) {
            enc = bodyattr.substr(attrstart + 4);
            if ((enc != TEXT("\"base64\"")) &&
                (enc != TEXT("\"qouted-printable\"")))
            {
                enc = TEXT("");
            }
            else
            {
                int repNo = enc.replaceAll(TEXT("\""),TEXT(""));                
            }
        }
        else
            enc = TEXT("");
    }
    else
        enc = TEXT("");

    if( XMLProcessor::getElementContent (msg, FILE_NAME, NULL, &start, &end) ) {
        name = msg.substr(start, end-start);
    }
    else name = TEXT("");

    if ( XMLProcessor::getElementContent (msg, FILE_SIZE, NULL, &start, &end) ) {
        size = _wtoi(msg.substr(start, end-start));
    }
    
    return ret;
}

wchar_t* FileData::format() {
    StringBuffer out;

    out.reserve(150);
    
    out = TEXT("<![CDATA[\n<File>\n");
    if (name.length() > 0)
        out += XMLProcessor::makeElement(FILE_NAME, name);
    if (created.length() > 0)
        out += XMLProcessor::makeElement(FILE_CREATED, created);
    if (modified.length() > 0)
        out += XMLProcessor::makeElement(FILE_MODIFIED, modified);
    if (accessed.length() > 0)
        out += XMLProcessor::makeElement(FILE_ACCESSED, accessed);
    StringBuffer attributes = TEXT("");
    if (isHiddenPresent)
        attributes += XMLProcessor::makeElement(FILE_HIDDEN, hidden);
    if (isArchivedPresent)
        attributes += XMLProcessor::makeElement(FILE_ARCHIVED, archived);
    if (isDeletedPresent)
        attributes += XMLProcessor::makeElement(FILE_DELETE, deleted);
    if (isExecutablePresent)
        attributes += XMLProcessor::makeElement(FILE_EXECUTABLE, executable);
    if (isReadablePresent)
        attributes += XMLProcessor::makeElement(FILE_READABLE, readable);
    if (isSystemPresent)
        attributes += XMLProcessor::makeElement(FILE_SYSTEM, system);
    if (!attributes.empty())
        out += XMLProcessor::makeElement(FILE_ATTRIBUTES, attributes);
    if (size > 0)
        out += XMLProcessor::makeElement(FILE_SIZE, size);
    if (enc.empty()){
        out += XMLProcessor::makeElement(FILE_BODY, body);
    }
    else
    {   
        ArrayList* attrList = new ArrayList();
        KeyValuePair* attr = new KeyValuePair(TEXT("enc"), (wchar_t*)enc.c_str());
        attrList->add(*attr);
        out += XMLProcessor::makeElement(FILE_BODY, body, attrList);
    }
    
    
    out += TEXT("</File>\n]]>\n");
    return stringdup(out.c_str());
}
