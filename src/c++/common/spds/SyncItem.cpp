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



#include <string.h>
#include <stdlib.h>

#include "base/util/utils.h"
#include "spds/SyncItem.h"

/*
 * Default constructor
 */
SyncItem::SyncItem() {
    initialize();
}


/*
 * Constructs a new SyncItem identified by the given key. The key must
 * not be longer than DIM_KEY (see SPDS Constants).
 *
 * @param key - the key
 */
SyncItem::SyncItem(const wchar_t* itemKey) {
    initialize();
    wcsncpy(key, itemKey, DIM_KEY);
    key[DIM_KEY-1] = 0;
}

/**
 * Initializes private members
 */
void SyncItem::initialize() {
    wcscpy(type, TEXT(TYPE_UNKNOWN));
    data = NULL;
    size = -1;
    lastModificationTime = -1;
    key[0] = 0;
    targetParent = NULL;
    sourceParent = NULL;
}

/*
 * Destructor. Free the allocated memory (if any)
 */
SyncItem::~SyncItem() {
    if (data) {
        delete [] data; data = NULL;
    }
    if (targetParent) {
        delete [] targetParent; targetParent = NULL;
    }
    if (sourceParent) {
        delete [] sourceParent; sourceParent = NULL;
    }
}

/*
 * Returns the SyncItem's key. If key is NULL, the internal buffer is
 * returned; if key is not NULL, the value is copied in the caller
 * allocated buffer and the given buffer pointer is returned.
 *
 * @param key - buffer where the key will be stored
 */
wchar_t* SyncItem::getKey() {
        return key;
    }

/*
 * Changes the SyncItem key. The key must not be longer than DIM_KEY
 * (see SPDS Constants).
 *
 * @param key - the key
 */
void SyncItem::setKey(const wchar_t* itemKey) {
    wcsncpy(key, itemKey, DIM_KEY);
    key[DIM_KEY-1] = 0;
}

/*
 * Sets the SyncItem modification timestamp. timestamp is a milliseconds
 * timestamp since a reference time (which is platform specific).
 *
 * @param timestamp - last modification timestamp
 */
 void SyncItem::setModificationTime(long timestamp) {
     lastModificationTime = timestamp;
 }

/*
 * Returns the SyncItem modeification timestamp. The returned value
 * is a milliseconds timestamp since a reference time (which is
 * platform specific).
 */
long SyncItem::getModificationTime() {
    return lastModificationTime;
}

/*
 * Sets the SyncItem content data. The passed data are copied into an
 * internal buffer so that the caller can release the buffer after
 * calling setData(). The buffer is fred in the destructor.
 * If when calling setData, there was an existing allocated data block,
 * it is reused (shrinked or expanded as necessary).
 */
void* SyncItem::setData(const void* itemData, long dataSize) {
    if (data) {
        delete [] data; data = NULL;
    }

    if (itemData == NULL) {
        size = 0;
        return NULL;
    }

    data = new char[dataSize];
    if (data == NULL) {
        lastErrorCode = ERR_NOT_ENOUGH_MEMORY;
        wcsprintf(lastErrorMsg, ERRMSG_NOT_ENOUGH_MEMORY, dataSize);
        return NULL;
    }

    size = dataSize;
    memcpy(data, itemData, size);

    return data;
}

/*
 * Returns the SyncItem data buffer. It is deleted in the destructor.
 */
void* SyncItem::getData() {
    return data;
}

/*
 * Returns the SyncItem data size.
 */
long SyncItem::getDataSize() {
    return size;
}

/*
 * Sets the SyncItem data mime type
 *
 * @param - type the content mimetype
 */
void SyncItem::setDataType(const wchar_t* mimeType) {
    wcsncpy(type, mimeType, DIM_MIME_TYPE);
    type[DIM_MIME_TYPE-1] = 0;
}

/*
 * Returns the SyncItem data mime type.
 *
 */
wchar_t* SyncItem::getDataType() {
    return type;
}

/*
 * Sets the SyncItem state
 *
 * @param state the new SyncItem state
 */
void SyncItem::setState(SyncState newState) {
    state = newState;
}

/*
 * Gets the SyncItem state
 */
SyncState SyncItem::getState() {
    return state;
}

/**
 * Gets the taregtParent property
 *
 * @return the taregtParent property value
 */
wchar_t* SyncItem::getTargetParent(wchar_t* parent) {
    if (parent == NULL) {
        return targetParent;
    }
    return wcscpy(parent, targetParent);
}

/**
 * Sets the taregtParent property
 *
 * @param parent the taregtParent property
 */
void SyncItem::setTargetParent(wchar_t* parent) {
    if (targetParent) {
        delete [] targetParent; targetParent = NULL;
    }
    targetParent = stringdup(parent);  
}

/**
 * Gets the sourceParent property
 *
 * @return the sourceParent property value
 */
wchar_t* SyncItem::getSourceParent(wchar_t* parent) {
    if (parent == NULL) {
        return sourceParent;
    }
    return wcscpy(parent, sourceParent);
}

/**
 * Sets the sourceParent property
 *
 * @param parent the sourceParent property
 */
void SyncItem::setSourceParent(wchar_t* parent) {
    if (sourceParent) {
        delete [] sourceParent; sourceParent = NULL;
    }
    sourceParent = stringdup(parent);  
}

ArrayElement* SyncItem::clone() {
    SyncItem* ret = new SyncItem(key);

    ret->setData(data, size);
    ret->setDataType(type);
    ret->setModificationTime(lastModificationTime);
    ret->setState(state);
    ret->setSourceParent(sourceParent);
    ret->setTargetParent(targetParent);

    return ret;
}
