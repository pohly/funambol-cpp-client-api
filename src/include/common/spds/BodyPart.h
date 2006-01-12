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
#ifndef INCL_BODY_PART
    #define INCL_BODY_PART

#include "base/util/ArrayElement.h"
#include "base/util/StringBuffer.h"

class BodyPart : public ArrayElement {
    private:
        StringBuffer mimeType;
        StringBuffer encoding;
        StringBuffer charset;
        StringBuffer content;
        StringBuffer disposition;
        StringBuffer filename;

    public:

        BodyPart();

        // The mime type as specified by MIME standard
        const wchar_t *getMimeType() const ;
        void setMimeType(const wchar_t *type) ;
        
        // The character set: UTF-8
        const wchar_t *getCharset() const ;
        void setCharset(const wchar_t *cs) ;
        
        // The content encoding: 7bit, 8bit, base64, quoted-printable
        const wchar_t *getEncoding() const ;
        void setEncoding(const wchar_t *type) ;

        // The content is the real content for the body
        // or a path name to a temp file for the attachment
        const wchar_t *getContent() const ;
        void setContent(const wchar_t *cont) ;

        // For multipart message.
        // Values: inline, attachment
        const wchar_t *getDisposition() const ;
        void setDisposition(const wchar_t *type) ;

        // For multipart message.
        // It is the name of the file attached (without path)
        const wchar_t *getFilename() const ;
        void setFilename(const wchar_t *type) ;

		ArrayElement* clone() ;
};

#endif

