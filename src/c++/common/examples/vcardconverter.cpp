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

/**
 * This program reads one vcard from the file
 * given as first parameter and applies certain
 * conversions to it.
 *
 * The content of the file has to be ASCII or
 * UTF-8 encoded.
 */

#include <stdio.h>
#include <memory>

#include "vocl/VConverter.h"
#include "vocl/vCard/vCardConverter.h"
#include "base/util/utils.h"

// very simply auto_ptr for arrays
template <class T> class auto_array {
    T *m_array;
  public:
    auto_array(T *array = 0) : m_array(array) {}
    ~auto_array() { if (m_array) delete [] m_array; }

    void operator = (T *array) {
        if (m_array) delete [] m_array;
        m_array = array;
    }
    operator T * () { return m_array; }
    T *get() { return m_array; }
    T &operator [] (int index) { return m_array[index]; }
};

int main( int argc, char **argv )
{
    char *sep = TEXT("--------------- %s -----------------------\n");
    char *sep2 = TEXT("-----------------------------------------------------------\n");

    if (argc != 2) {
        fprintf(stdout, "usage: %s <vcard file>\n", argv[0]);
        return 1;
    }

    // read as char *
    char *buffer;
    size_t len;
    if (!readFile(argv[1], &buffer, &len, true)) {
        fprintf(stdout, "%s: reading failed", argv[1]);
    }
    auto_array<char> vcard(buffer);

    // convert to wchar_t
    auto_array<wchar_t> wvcard(toWideChar(vcard));
    fwprintf(stdout, sep, TEXT("original vcard"));
    fwprintf(stdout, TEXT("%s\n"), wvcard.get());
    fwprintf(stdout, sep2);
    fwprintf(stdout, TEXT("\n"));

    // parse it
    std::auto_ptr<VObject> vobj(VConverter::parse(wvcard));
    if (vobj.get() == 0) {
        fprintf(stdout, "VConverter::parse()failed\n");
        return 1;
    }
    vobj->toNativeEncoding();
    
    VProperty *fileas = vobj->getProperty(TEXT("X-EVOLUTION-FILE-AS"));
    VProperty *n = vobj->getProperty(TEXT("FN"));
    fwprintf(stdout,
             TEXT("version: %s\nprodid: %s\nfull name: %s\nfile-as: %s\n\n"),
             vobj->getVersion(),
             vobj->getProdID(),
             n ? n->getValue() : TEXT("<not set>"),
             fileas ? fileas->getValue() : TEXT("<not set>"));

    // convert into the other version, then back again
    wchar_t *versions[2];
    if (!wcscmp(vobj->getVersion(), TEXT("3.0"))) {
        versions[0] = TEXT("2.1");
        versions[1] = TEXT("3.0");
    } else {
        versions[0] = TEXT("3.0");
        versions[1] = TEXT("2.1");
    }
    for (int index = 0; index < 2; index++) {
        vobj->setVersion(versions[index]);
        VProperty *vprop = vobj->getProperty(TEXT("VERSION"));
        vprop->setValue(versions[index]);
        vobj->fromNativeEncoding();
        wvcard = vobj->toString();
        vobj->toNativeEncoding();
        fwprintf(stdout, sep, versions[index]);
        fwprintf(stdout, TEXT("%s\n"), wvcard.get());
        fwprintf(stdout, sep2);
        fwprintf(stdout, TEXT("\n"));
    }

    // convert into validated contact
    vCardConverter converter;
    converter.setSource(wvcard);
    Contact *contactPtr;
    long errorCode;
    if (!converter.convert(lastErrorMsg, &errorCode)) {
        fwprintf(stdout, TEXT("converter failed: %s (%ld)\n"), lastErrorMsg, errorCode);
        return 1;
    }
    converter.getContact(&contactPtr);
    std::auto_ptr<Contact> contact(contactPtr);
    wvcard = contact->toString();
    fwprintf(stdout, sep, TEXT("after parsing"));
    fwprintf(stdout, TEXT("%s\n"), wvcard.get());
    fwprintf(stdout, sep2);
    fwprintf(stdout, TEXT("\n"));

    // let's see how the Contact class interprets the properties
    Name *name = contact->getName();
    vCardProperty *displayname = name->getDisplayName();
    fwprintf(stdout,
             TEXT("display name\nencoding: %s\ncharset: %s\nlanguage: %s\nvalue: %s\n\n"),
             displayname->getEncoding(),
             displayname->getCharset(),
             displayname->getLanguage(),
             displayname->getValue());

    return 0;
}


