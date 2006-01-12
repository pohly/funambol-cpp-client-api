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

#include <windows.h>
#include <wincrypt.h>

#include "base/util/utils.h"
#include "spds/DESDecoder.h"


DESDecoder::DESDecoder() : DataTransformer(DT_DES) {
}

DESDecoder::~DESDecoder() {
}

char* DESDecoder::transform(char* data, TransformationInfo& info) {
    BOOL res;

    HCRYPTPROV prov = 0;
    HCRYPTKEY key = 0;

    HCRYPTHASH hash=0;

    DWORD size = info.size; // I reassign it to a DWORD 
                            // just in case a long is not 
                            // of the same size of a DWORD
    DWORD dwParam = 0;

    char* password = wc2utf8(info.password, NULL, 0);

    // --------------------------------------------------------

    res = CryptAcquireContext(
        &prov,             // Variable to hold returned handle.
        NULL,               // Use default key container.
        MS_ENHANCED_PROV,   // Use enhanced CSP.
        PROV_RSA_FULL,      // Type of provider to acquire.
        CRYPT_VERIFYCONTEXT
    );

    if (res == FALSE) {
        lastErrorCode = ERR_DT_FAILURE;
        wcsprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }


    res = CryptCreateHash(
        prov,      // CSP handle
        CALG_MD5,   // hash algorith ID
        0,          // Key not used
        0,          // flags not used
        &hash      // handle to the hash object
    );

    if (res == FALSE) {
        lastErrorCode = ERR_DT_FAILURE;
        wcsprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // hash password
    res = CryptHashData(
        hash,                          // hash handle
        (unsigned char*) password,// pointer to the data buffer
        strlen(password),         // data length
        0                              // flags not used
    );

    if (res == FALSE) {
        lastErrorCode = ERR_DT_FAILURE;
        wcsprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // Derive a session key from the hash object.
    res = CryptDeriveKey (
        prov, 
        CALG_DES, 
        hash, 
        0, 
        &key
    );

    if (res == FALSE) {
        lastErrorCode = ERR_DT_FAILURE;
        wcsprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // set encryption mode to ECB
    dwParam=CRYPT_MODE_ECB;
    res = CryptSetKeyParam(
        key,                      // key handle
        KP_MODE,                   // set key mode flag
        (unsigned char*) &dwParam, // new mode value
        0                          // flags not used
    );

    if (res == FALSE) {
        lastErrorCode = ERR_DT_FAILURE;
        wcsprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    // set padding mode to PKCS5
    dwParam=PKCS5_PADDING;
    res = CryptSetKeyParam(
        key,                      // key handle
        KP_PADDING,                   // set key mode flag
        (unsigned char*) &dwParam, // new mode value
        0                          // flags not used
    );

    if (res == FALSE) {
        lastErrorCode = ERR_DT_FAILURE;
        wcsprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    res = CryptDecrypt (
        key, 
        0, 
        TRUE, 
        0, 
        (unsigned char*)data, 
        &size
    );

    if (res == FALSE) {
        lastErrorCode = ERR_DT_FAILURE;
        wcsprintf(lastErrorMsg, ERRMSG_DT_FAILURE, GetLastError());
        goto exit;
    }

    info.size = size;
    info.newReturnedData = FALSE;

 exit:

   // Destroy the session key.
   if (key)
     CryptDestroyKey (key);

   // Destroy the hash object.
   if (hash)
     CryptDestroyHash (hash);

   // Release the provider handle.
   if (prov)
     CryptReleaseContext (prov, 0);

   if (password) {
       delete [] password;
   }

   return data;
    
}