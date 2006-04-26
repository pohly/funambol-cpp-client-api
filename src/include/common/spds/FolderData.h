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


#ifndef INCL_FOLDERDATA
#define INCL_FOLDERDATA

#include "base/util/ArrayElement.h"
#include "base/util/WString.h"
#include "base/util/StringBuffer.h"

class FolderData : public ArrayElement {
    
    // ------------------------------------------------------- Private data
    private:
        //WString folder;
		WString name;
		WString created;
		WString modified;
		WString accessed;
		WString attributes;
		bool hidden;
		bool system;
		bool archived;
		bool deleted;
		bool writable;
		bool readable;
		bool executable;
		WString role;
        		
        // represents the presence of their equivalent tag
        bool isHiddenPresent;
        bool isSystemPresent;
        bool isArchivedPresent;
        bool isDeletedPresent;
        bool isWritablePresent;
        bool isReadablePresent;
        bool isExecutablePresent;

        /*
        * return the length for the base64 array starting from length of the original array
        */
        int lengthForB64(int len);

    public:
    // ------------------------------------------------------- Constructors
        FolderData();
        ~FolderData();

    // ---------------------------------------------------------- Accessors  
		/*const wchar_t* getFolder() { return folder; }
		void setFolder(const wchar_t* v) { folder = v; } */

		const wchar_t* getName() { return name; }
		void setName(const wchar_t* v) { name = v; }

		const wchar_t* getCreated() { return created; }
		void setCreated(const wchar_t* v) { created = v; }

		const wchar_t* getModified() { return modified; }
		void setModified(const wchar_t* v) { modified = v; }

		const wchar_t* getAccessed() { return accessed; }
		void setAccessed(const wchar_t* v) { accessed = v; }

		const wchar_t* getAttributes() { return attributes; }
		void setAttributes(const wchar_t* v) { attributes = v; }
		
		bool getHidded() { return hidden; }
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

		const wchar_t* getRole() { return role; }
		void setRole(const wchar_t* v) { role = v; } 
        
        
    // ----------------------------------------------------- Public Methods
        int parse(const char *syncmlData, size_t len = WString::npos) ;
        char *format() ;

        ArrayElement* clone() { return new FolderData(*this); }
   
};

#endif

