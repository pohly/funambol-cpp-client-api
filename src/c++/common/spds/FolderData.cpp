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
#include "spds/FolderData.h"
#include "base/quoted-printable.h"


#define FOLDER_ITEM       TEXT("Folder")
#define FOLDER_HIDDEN     T("h")
#define FOLDER_SYSTEM     T("s") 
#define FOLDER_ARCHIVED   T("a")
#define FOLDER_DELETE     T("d")
#define FOLDER_WRITABLE   T("w")
#define FOLDER_READABLE   T("r")
#define FOLDER_EXECUTABLE T("e")
#define FOLDER_ACCESSED   T("accessed")
#define FOLDER_ATTRIBUTES T("attributes")
#define FOLDER_ROLE       T("role")
#define FOLDER_MODIFIED   T("modified")
#define FOLDER_NAME       T("name")
#define FOLDER_CREATED    T("created")
#define FOLDER_EXT    T("ext")
#define FOLDER_XNAM    T("XNam")
#define FOLDER_XVAL    T("XVal")


FolderData::FolderData()
{
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

FolderData::~FolderData()
{
    accessed.reset();
    attributes.reset();
    //folder.reset();
    modified.reset();
    name.reset();
    created.reset();
    role.reset();
            
}
int FolderData::parse(const char *syncmlData, size_t len)
{
    int ret = 0;
    unsigned int start, end;        
    StringBuffer* s = new StringBuffer(syncmlData, len);
    StringBuffer bodyattr;

    // FIXME: remove these replace once the server has fixed the message.
    s->replaceAll(T("&lt;"), T("<"));
    s->replaceAll(T("&gt;"), T(">"));
    s->replaceAll(T("&amp;"), T("&"));
    
    // Get the CDATA content
    if(XMLProcessor::getElementContent(s->c_str(), T("CDATA"), NULL, &start, &end) == 0) {
        LOG.info(T("FileData: can't find outer CDATA section."));
        start = 0;
        end = s->length();        
    }
    StringBuffer msg = s->substr(start, end-start);
    
    delete s;

    // Get attributes
    if( XMLProcessor::getElementContent (msg, FOLDER_HIDDEN, NULL, &start, &end) ) {
        hidden = ( strncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
        isHiddenPresent = true;
    }
    else hidden = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_SYSTEM, NULL, &start, &end) ) {
        system = ( strncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
        isSystemPresent = true;
    }
    else system = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_ARCHIVED, NULL, &start, &end) ) {
        archived = ( strncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
        isArchivedPresent = true;
    }
    else archived = false;
    
    if( XMLProcessor::getElementContent (msg, FOLDER_DELETE, NULL, &start, &end) ) {
        deleted = ( strncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
        isDeletedPresent = true;
    }
    else deleted = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_WRITABLE, NULL, &start, &end) ) {
        writable = ( strncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
        isWritablePresent = true;
    }
    else writable = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_READABLE, NULL, &start, &end) ) {
        readable = ( strncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
        isReadablePresent = true;
    }
    else readable = false;

    if( XMLProcessor::getElementContent (msg, FOLDER_EXECUTABLE, NULL, &start, &end) ) {
        executable = ( strncmp(msg.c_str()+start, T("true"), end-start) == 0 ) ;
        isExecutablePresent = true;
    }
    else executable = false;
    
    if( XMLProcessor::getElementContent (msg, FOLDER_ACCESSED, NULL, &start, &end) ) {
        accessed = msg.substr(start, end-start);
    }
    else accessed = TEXT("");

    if( XMLProcessor::getElementContent (msg, FOLDER_MODIFIED, NULL, &start, &end) ) {
        modified = msg.substr(start, end-start);
    }
    else modified = TEXT("");

    if( XMLProcessor::getElementContent (msg, FOLDER_CREATED, NULL, &start, &end) ) {
        created = msg.substr(start, end-start);
    }
    else created = TEXT("");

    if( XMLProcessor::getElementContent (msg, FOLDER_ROLE, NULL, &start, &end) ) {
        role = msg.substr(start, end-start);
    }
    else role = TEXT("");


    if( XMLProcessor::getElementContent (msg, FOLDER_NAME, NULL, &start, &end) ) {
        name = msg.substr(start, end-start);
    }
    else{ 
        name = TEXT("");
        ret = -1;
    }

    if( XMLProcessor::getElementContent (msg, FOLDER_EXT, NULL, &start, &end) ) {
        ;
        // In this version the extention fields are not suported
    }
            
    return ret;
}


char* FolderData::format() {
    
    StringBuffer out;

    out.reserve(150);
    
    out = T("<![CDATA[\n<Folder>\n");
    if (name.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_NAME, _wcc(name));
    if (created.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_CREATED, _wcc(created));
    if (modified.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_MODIFIED, _wcc(modified));
    if (accessed.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_ACCESSED, _wcc(accessed));
    
    StringBuffer attributes;

    if (isHiddenPresent)
        attributes += XMLProcessor::makeElement(FOLDER_HIDDEN, hidden);
    if (isSystemPresent)
        attributes += XMLProcessor::makeElement(FOLDER_SYSTEM, system);
    if (isArchivedPresent)
        attributes += XMLProcessor::makeElement(FOLDER_ARCHIVED, archived);
    if (isDeletedPresent)
        attributes += XMLProcessor::makeElement(FOLDER_DELETE, deleted);
    if (isWritablePresent)
        attributes += XMLProcessor::makeElement(FOLDER_WRITABLE, writable);
    if (isReadablePresent)
        attributes += XMLProcessor::makeElement(FOLDER_READABLE, readable);
    if (isExecutablePresent)
        attributes += XMLProcessor::makeElement(FOLDER_EXECUTABLE, executable);

    if (!attributes.empty())
        out += XMLProcessor::makeElement(FOLDER_ATTRIBUTES, attributes);
    
    if (role.length() > 0)
        out += XMLProcessor::makeElement(FOLDER_ROLE, _wcc(role));
    
    out += T("</Folder>\n]]>\n");
    return stringdup(out.c_str());
}

int FolderData::lengthForB64(int len) {
    
    int modules = 0;
    int ret     = 0;

    modules = len % 3;
    if (modules == 0) {
        ret = 4 * (len / 3);

    } else {
        ret = 4 * ((len/3) + 1);

    }
    return ret;


}
