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


#ifndef INCL_FILE
#define INCL_FILE

#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"
#include "spds/MailMessage.h"

class FileData : public ArrayElement {
    
    // ------------------------------------------------------- Private data
    private:
        StringBuffer file;
		StringBuffer name;
		StringBuffer created;
		StringBuffer modified;
		StringBuffer accessed;
		StringBuffer attributes;
		bool hidden;
		bool system;
		bool archived;
		bool deleted;
		bool writable;
		bool readable;
		bool executable;
		StringBuffer cttype;
		StringBuffer body;
		StringBuffer enc;
		int size;

        // represents the presence of their equivalent tag
        bool isHiddenPresent;
        bool isSystemPresent;
        bool isArchivedPresent;
        bool isDeletedPresent;
        bool isWritablePresent;
        bool isReadablePresent;
        bool isExecutablePresent;
    public:
    // ------------------------------------------------------- Constructors
        FileData();
        ~FileData();

    // ---------------------------------------------------------- Accessors  
		const wchar_t* getFile() { return file; }
		void setFile(const wchar_t* v) { file = v; } 

		const wchar_t* getName() { return name; }
		void setName(const wchar_t* v) { name = v; }

		const wchar_t* getCreated() { return created; }
		void setCreated(const wchar_t* v) { created = v; }

		const wchar_t* getModified() { return modified; }
		void setModified(const wchar_t* v) { modified = v; }

		const wchar_t* getAccessed() { return accessed; }
		void setAccessed(const wchar_t* v) { accessed = v; }

		const wchar_t* getAttributes() { return file; }
		void setAttributes(const wchar_t* v) { attributes = v; }
		
		bool getHiddied() { return hidden; }
		void setHidden(bool v) { hidden = v; }

		bool getSystem() { return system; }
		void setSystem(bool v) { system = v; }

		bool getArchived() { return archived; }
		void setArchived(bool v) { archived = v; }

		bool getDeleted() { return deleted; }
		void setDeleted(bool v) { deleted = v; }

		bool getWritable() { return writable; }
		void setWritable(bool v) { writable = v; }
		
		bool getReadable() { return readable; }
		void setReadable(bool v) { readable = v; }

		bool getExecutable() { return executable; }
		void setExecutable(bool v) { executable = v; }

		const wchar_t* getCttype() { return cttype; }
		void setCttype(const wchar_t* v) { cttype = v; } 

        const wchar_t* getBody() { return body; }
		void setBody(const wchar_t* v) { body = v; } 

		const wchar_t* getEnc() { return enc; }
		void setEnc(const wchar_t* v) { enc = v; } 
		
		int getSize() { return size; }
		void setSize(int v) { size = v; }
        
    // ----------------------------------------------------- Public Methods
        int parse(const wchar_t *syncmlData, size_t len = StringBuffer::npos) ;
        wchar_t *format() ;

        ArrayElement* clone() { return new FileData(*this); }
   
};

#endif

