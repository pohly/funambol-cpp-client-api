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

 #ifndef INCL_SYNC_ITEM
    #define INCL_SYNC_ITEM

    #include "base/fscapi.h"
    #include "base/constants.h"
    #include "base/util/ArrayElement.h"
    #include "spds/constants.h"
    #include "spds/SyncStatus.h"

    typedef enum {
        SYNC_STATE_NEW     = 'N',
        SYNC_STATE_UPDATED = 'U',
        SYNC_STATE_DELETED = 'D',
        SYNC_STATE_NONE    = ' '
    } SyncState;

    class SyncItem : public ArrayElement {

    private:

        char* data;
        long size;

        WCHAR key[DIM_KEY];
        WCHAR type[DIM_MIME_TYPE];

        long lastModificationTime;
        SyncState state;

        WCHAR* targetParent;
        WCHAR* sourceParent;

        /**
         * Initializes private members
         */
        void initialize() EXTRA_SECTION_01;

    public:
        /*
         * Default constructor
         */
        SyncItem() EXTRA_SECTION_01;

        ~SyncItem() EXTRA_SECTION_01;

        /*
         * Constructs a new SyncItem identified by the given key. The key must
         * not be longer than DIM_KEY (see SPDS Constants).
         *
         * @param key - the key
         */
        SyncItem(const WCHAR* key) EXTRA_SECTION_01;

        /*
         * Returns the SyncItem's key. If key is NULL, the internal buffer is
         * returned; if key is not NULL, the value is copied in the caller
         * allocated buffer and the given buffer pointer is returned.
         *
         * @param key - buffer where the key will be stored
         */
        WCHAR* getKey() EXTRA_SECTION_01;

        /*
         * Changes the SyncItem key. The key must not be longer than DIM_KEY
         * (see SPDS Constants).
         *
         * @param key - the key
         */
        void setKey(const WCHAR* key) EXTRA_SECTION_01;

        /*
         * Sets the SyncItem modification timestamp. timestamp is a milliseconds
         * timestamp since a reference time (which is platform specific).
         *
         * @param timestamp - last modification timestamp
         */
        void setModificationTime(long timestamp) EXTRA_SECTION_01;

        /*
         * Returns the SyncItem modeification timestamp. The returned value
         * is a milliseconds timestamp since a reference time (which is
         * platform specific).
         */
        long getModificationTime() EXTRA_SECTION_01;

        /*
         * Sets the SyncItem content data. The passed data is copied into an
         * internal buffer so that the caller can release the buffer after
         * calling setData().
         *
         * The data currently cannot contain nul-bytes because it is
         * treated like a C-style string. The size parameter should
         * not include the nul-byte which terminates C strings, so
         * pass size==0 for an empty string. A nul-byte is always
         * appended at the end of the data automatically.
         *
         * Older versions of this API required that clients include
         * the nul-byte in the string; to be backwards compatible, this
         * behavior is still accepted by this call which then simply
         * reduces the size by one byte.
         *
         * @param data        memory to be copied, may be NULL; in that case an empty buffer is allocated
         * @param size        length of the given data or, if data is NULL, the desired buffer size
         */
        void* setData(const void* data, long size) EXTRA_SECTION_01;

        /*
         * Returns the SyncItem data buffer, in read-write mode.
         *
         * There is guaranteed to be a nul-byte after the data which
         * is not included in the data size.
         */
        void* getData() EXTRA_SECTION_01;

        /*
         * Returns the amount of bytes stored in the item,
         * excluding the implicit nul-byte after the real data.
         */
        long getDataSize() EXTRA_SECTION_01;
        
         /*
         * Sets the SyncItem data size without changing the data buffer.
         *
         * @param s the new size
         */
        void setDataSize(long s) EXTRA_SECTION_01;

        /*
         * Sets the SyncItem data mime type
         *
         * @param - type the content mimetype
         */
        void setDataType(const WCHAR* type) EXTRA_SECTION_01;

        /*
         * Returns the SyncItem data mime type.
         */
        WCHAR* getDataType() EXTRA_SECTION_01;

        /*
         * Sets the SyncItem state
         *
         * @param state the new SyncItem state
         */
        void setState(SyncState newState) EXTRA_SECTION_01;

        /*
         * Gets the SyncItem state
         */
        SyncState getState() EXTRA_SECTION_01;

       /**
         * Returns the SyncItem targetParent
         *
         * @return the SyncItem target parent
         */
        WCHAR* getTargetParent(WCHAR* parent = NULL) EXTRA_SECTION_01;
    
        /**
         * Sets the SyncItem targetParent
         *
         * @param parent the target parent
         *
         */
        void setTargetParent(const WCHAR* parent) EXTRA_SECTION_01;    

        /**
         * Returns the SyncItem sourceParent
         *
         * @return the SyncItem source parent
         */
        WCHAR* getSourceParent(WCHAR* parent = NULL) EXTRA_SECTION_01;
    
        /**
         * Sets the SyncItem sourceParent
         *
         * @param parent the source parent
         *
         */
        void setSourceParent(const WCHAR* parent) EXTRA_SECTION_01;

        /**
         * Creates a new instance of SyncItem from the content of this
         * object. The new instance is created the the C++ new operator and
         * must be removed with the C++ delete operator.
         */
        ArrayElement* clone() EXTRA_SECTION_01;


    };

#endif
