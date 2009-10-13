/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

# include <cppunit/extensions/TestFactoryRegistry.h>
# include <cppunit/extensions/HelperMacros.h>

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "spds/SyncManagerConfig.h"

#include "inputStream/BufferInputStream.h"
#include "testUtils.h"


#define SMALL_BUFFER    "1234567890123456789012345678901234567890"



USE_NAMESPACE

class BufferInputStreamTest : public CppUnit::TestFixture {

    CPPUNIT_TEST_SUITE(BufferInputStreamTest);
    CPPUNIT_TEST(testRead);
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp()    {}
    void tearDown() {}

private:

    /// Reads a small buffer of data (40 chars)
    void testRead() {

        // Input buffer
        StringBuffer smallBuffer(SMALL_BUFFER);
        const void* data = (const void*)(smallBuffer.c_str());
        const unsigned int dataSize = smallBuffer.length();

        // Output buffer
        const int outBufferSize = dataSize + 10;
        char* outBuffer = new char[outBufferSize + 1];
        

        BufferInputStream stream(data, dataSize);
        CPPUNIT_ASSERT (stream.getPosition() == 0);
        CPPUNIT_ASSERT (!stream.eof());


        //
        // test 1. Chunks read > data size
        //
        memset(outBuffer, '-', outBufferSize);
        int bytesRead = stream.read(outBuffer, dataSize + 5);

        CPPUNIT_ASSERT (bytesRead == dataSize);
        CPPUNIT_ASSERT (memcmp(outBuffer, data, dataSize) == 0);
        CPPUNIT_ASSERT (stream.getPosition() == dataSize);
        CPPUNIT_ASSERT (stream.eof());

        // no more data expected...
        bytesRead = stream.read(outBuffer, 10);
        CPPUNIT_ASSERT (bytesRead == 0);


        //
        // test 2. Chunks read < data size
        //
        stream.reset();
        memset(outBuffer, '-', outBufferSize);
        bytesRead = stream.read(outBuffer, dataSize - 15);

        CPPUNIT_ASSERT (bytesRead == dataSize - 15);
        CPPUNIT_ASSERT (memcmp(outBuffer, data, dataSize - 15) == 0);
        CPPUNIT_ASSERT (stream.getPosition() == dataSize - 15);
        CPPUNIT_ASSERT (!stream.eof());

        // 2nd chunk
        memset(outBuffer, '-', outBufferSize);
        int offset = stream.getPosition();
        bytesRead = stream.read(outBuffer, dataSize);
        
        CPPUNIT_ASSERT (bytesRead == 15);
        CPPUNIT_ASSERT (memcmp(outBuffer, (byte*)data + offset, 15) == 0);
        CPPUNIT_ASSERT (stream.getPosition() == dataSize);
        CPPUNIT_ASSERT (stream.eof());


        //
        // test 3. Small chunks read < data size
        //
        int smallChunkSize = 8;
        stream.reset();
        while (!stream.eof()) {
            memset(outBuffer, '-', outBufferSize);
            offset = stream.getPosition();
            bytesRead = stream.read(outBuffer, smallChunkSize);

            CPPUNIT_ASSERT (bytesRead == smallChunkSize);
            CPPUNIT_ASSERT (memcmp(outBuffer, (byte*)data + offset, smallChunkSize) == 0);
        }

        delete [] outBuffer;
    }


private:

};

CPPUNIT_TEST_SUITE_REGISTRATION( BufferInputStreamTest );
