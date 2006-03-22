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
        const BCHAR *getMimeType() const ;
        void setMimeType(const BCHAR *type) ;
        
        // The character set: UTF-8
        const BCHAR *getCharset() const ;
        void setCharset(const BCHAR *cs) ;
        
        // The content encoding: 7bit, 8bit, base64, quoted-printable
        const BCHAR *getEncoding() const ;
        void setEncoding(const BCHAR *type) ;

        // The content is the real content for the body
        // or a path name to a temp file for the attachment
        const BCHAR *getContent() const ;
        void setContent(const BCHAR *cont) ;

        // For multipart message.
        // Values: inline, attachment
        const BCHAR *getDisposition() const ;
        void setDisposition(const BCHAR *type) ;

        // For multipart message.
        // It is the name of the file attached (without path)
        const BCHAR *getFilename() const ;
        void setFilename(const BCHAR *type) ;

		ArrayElement* clone() ;
};

#endif

