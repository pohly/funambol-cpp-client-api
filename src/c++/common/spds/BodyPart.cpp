/*
 * Copyright (C) 2003-2006 Funambol
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

#include "base/fscapi.h"
#include "spds/BodyPart.h"

BodyPart::BodyPart() {
    mimeType = TEXT("text/plain");
    charset  = TEXT("UTF-8");
    encoding = TEXT("8bit");
}

const wchar_t * BodyPart::getMimeType() const { return (mimeType.c_str()); }
void BodyPart::setMimeType(const wchar_t *type) { mimeType = type; }

const wchar_t * BodyPart::getEncoding() const { return (encoding.c_str()); }
void BodyPart::setEncoding(const wchar_t *enc) { encoding = enc; }

const wchar_t * BodyPart::getCharset() const { return (charset.c_str()); }
void BodyPart::setCharset(const wchar_t *cs) { charset = cs; }

const wchar_t * BodyPart::getContent() const { return content.c_str(); }
void BodyPart::setContent(const wchar_t *cont) { content = cont; }

const wchar_t *BodyPart::getDisposition() const { return disposition.c_str(); }
void BodyPart::setDisposition(const wchar_t *disp) { disposition = disp; }

const wchar_t *BodyPart::getFilename() const { return filename.c_str(); }
void BodyPart::setFilename(const wchar_t *name) { filename = name; }

ArrayElement* BodyPart::clone() {
    BodyPart *p = new BodyPart();
    p->setMimeType(mimeType);
    p->setEncoding(encoding);
    p->setContent(content);
    p->setDisposition(disposition);
    p->setFilename(filename);

    return p;
}

