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

#include <stdlib.h>

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "spds/DataTransformerFactory.h"
#include "spds/B64Decoder.h"
#include "spds/B64Encoder.h"
#include "spds/DESDecoder.h"
#include "spds/DESEncoder.h"

//
// IMPORTANT: this test case encodes/decodes a UNICODE string; make sure
// to update it to UTF8 strings when used with such content. 
//
void testEncryption() {
    wchar_t* clearText = TEXT("This is clear text.\nLet's see if encryption/decryption works!");
    wchar_t* password = TEXT("dummypassword");

    DataTransformer* b64e = DataTransformerFactory::getEncoder(TEXT("b64"));
    DataTransformer* b64d = DataTransformerFactory::getDecoder(TEXT("b64"));
    DataTransformer* dese = DataTransformerFactory::getEncoder(TEXT("des"));
    DataTransformer* desd = DataTransformerFactory::getDecoder(TEXT("des"));


    TransformationInfo infoe, infod;

    infoe.size = wcslen(clearText)*sizeof(wchar_t);
    infoe.password = password;

    MessageBox(0, clearText, TEXT("Clear text"), MB_OK);

    char* desText = dese->transform((char*)clearText, infoe);
    char* b64Text = b64e->transform((char*)desText, infoe);


    wcsprintf(logmsg, TEXT("%S"), b64Text);  // NOTE: %S is not portable!!!
    MessageBox(0, logmsg, TEXT("B64 text"), MB_OK);

    delete [] desText;

    infod.size = infoe.size;
    infod.password = infoe.password;
    desText = b64d->transform((char*)b64Text, infod);
    clearText = (wchar_t*)desd->transform((char*)desText, infod);

    wchar_t* clearString = new wchar_t[infod.size/sizeof(wchar_t)+1];
    wcsncpy(clearString, clearText, infod.size/sizeof(wchar_t));
    clearString[infod.size/sizeof(wchar_t)] = 0;

    wcsprintf(logmsg, TEXT("%s"), clearString);
    MessageBox(0, logmsg, TEXT("Clear text"), MB_OK);

    delete [] clearString; delete [] clearText; 
    delete [] b64Text; delete [] desText;
}