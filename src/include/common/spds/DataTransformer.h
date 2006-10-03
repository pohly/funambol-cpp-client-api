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

/**
 * This class represents a data encoder or decoder
 *
 * A DataTransformer is a component that give some data in some format
 * as input, it generates a transformation of the source data in output.
 * Examples of transformations are:
 *
 * - base64 encoding/decoding
 * - encryption/decription
 */

 #ifndef INCL_DATA_TRANSFORMER
    #define INCL_DATA_TRANSFORMER

    #include "base/fscapi.h"

    /**
     * Properties used by a DataTransformer. See the design document
     * for details.
     */
    struct TransformationInfo {

        BOOL newReturnedData;
        long size;
        const char*  username;
        const char*  password;
        const char*  sourceName;

        TransformationInfo() : newReturnedData(FALSE)
                             , size(-1)
                             , username(NULL)
                             , password(NULL)
                             , sourceName(NULL) {}

    };

    class DataTransformer {

    private:

        char*  name;

    public:
        /*
         * Default constructor-destructor
         */
        DataTransformer() EXTRA_SECTION_01;

        DataTransformer(char*  name) EXTRA_SECTION_01;

        virtual ~DataTransformer() EXTRA_SECTION_01;

        static DataTransformer* getEncoder(char*  name) EXTRA_SECTION_01;
        static DataTransformer* getDecoder(char*  name) EXTRA_SECTION_01;
        static BOOL isSupportedEncoder(char*  name) EXTRA_SECTION_01;
        static BOOL isSupportedDecoder(char*  name) EXTRA_SECTION_01;

        void setName(char*  name) EXTRA_SECTION_01;
        char*  getName(char*  name = NULL) EXTRA_SECTION_01;

        /**
         * Performs the transformation. data is the pointer to the
         * data buffer to transform and info is a TransformationInfo
         * object containing some session propertiues that can be
         * used by a transformer. The function allocates the returned data
         * with the new operator; the caller is responsible to free
         * the allocated memory with the delete operator.
         * In case of error NULL is returned and lastErrorCode and
         * lastErrorMsg are set accordingly.
         *
         * @param data the data block to transform
         * @param info transformation info
         */
        virtual char* transform(char* data, TransformationInfo& info) = 0 EXTRA_SECTION_01;

    };

#endif
