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

#ifndef INCL_FILE_OBJECT_INPUT_STREAM
#define INCL_FILE_OBJECT_INPUT_STREAM
/** @cond DEV */

#include "base/fscapi.h"
#include "inputStream/FileInputStream.h"


BEGIN_NAMESPACE


/**
 * Extends the FileInputStream class, implements an input stream to read from a 
 * generic file, given its full path.
 * The file's content read is returned as a File Object Data (XML with
 * tags about the file size, name and so on).
 */
class FileObjectInputStream : public FileInputStream {

private:

public:

    /**
     * Constructor. 
     * @param path  the file location (path & file name) to read from.
     */
    FileObjectInputStream(const StringBuffer& path);

    ~FileObjectInputStream();

    /**
     * Reads 'size' bytes from the file. Formats the FileObject XML structure to
     * return as the file's content.
     * @note            The file's content read is returned as a File Object Data (XML with
     *                  tags about the file size, name and so on).
     * @param buffer    [IN/OUT] the buffer of data read, allocated by the caller
     * @param size      the size of the chunk to be read [in bytes]
     * @return          the number of bytes effectively read (<= size)
     */
    virtual int read(void* buffer, const int size);

};


END_NAMESPACE

/** @endcond */
#endif
