/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#include "base/util/utils.h"
#include "spds/ItemReader.h"
#include "inputStream/InputStream.h"
#include "base/Log.h"

USE_NAMESPACE

ItemReader::ItemReader(unsigned long size, EncodingHelper& help) : helper(help){ 
    maxChunkSize = size; 
    buffer = new char[maxChunkSize + 1];    
    isFirstChunk = true;        
}

ItemReader::~ItemReader() {
    delete [] buffer;
}

void ItemReader::setSyncItem(SyncItem* item) {
    syncItem = item;
    isFirstChunk = true;
}

void ItemReader::resetBuffer(unsigned long size) {
    if (size > maxChunkSize) {
        delete [] buffer;
        buffer = new char[size + 1];
        maxChunkSize = size;
    }
    memset(buffer, 0, maxChunkSize + 1);
}


// set the right encoding/encryption to have the buffer properly ok
// what to with the des encoding? b64 ok but des not
Chunk* ItemReader::getNextChunk(unsigned long size) {
        
    resetBuffer(size);
    unsigned long bytesRead = 0;
    unsigned long toRead    = size;
    Chunk* chunk            = NULL;
    bool res                = true;
    char* value             = NULL;

    if (syncItem == NULL) {
        LOG.error("ItemReader: the syncItem is null");
        return NULL;
    }
    
    InputStream* istream = syncItem->getInputStream();

    if (syncItem->getDataEncoding() == NULL) {
        toRead = helper.getMaxDataSizeToEncode(size);
    } 
    
    bytesRead = istream->read((void*)buffer, toRead);

    bool last = true;
    // set if it is the last chunk
    if (istream->eof() == 0) {
        last = false; 
    }
    //if (bytesRead == toRead) {
    //    last = false; 
    //} 

    value = helper.encode(buffer, &bytesRead);     
    
    if (value == NULL) {
        LOG.info("ItemReader: getNextChunk NULL after transformation");
        return NULL;
    }

    chunk = new Chunk(value);
    chunk->setLast(last);

    // set if it is the first chunk
    if (isFirstChunk) {
        chunk->setFirst(true);
        isFirstChunk = false;        
    } else {
        chunk->setFirst(false);
    }
    
    chunk->setTotalDataSize(helper.getDataSizeAfterEncoding(syncItem->getDataSize()));
    chunk->setDataEncoding(helper.getDataEncoding());

    delete [] value;

    return chunk;
}
