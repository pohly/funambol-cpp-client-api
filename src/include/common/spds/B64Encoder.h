/*
 * Copyright (C) 2003-2007 Funambol
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
 #ifndef INCL_B64_ENCODER
    #define INCL_B64_ENCODER

    #include "spds/DataTransformer.h"

    #define DT_B64 "b64"

    class B64Encoder : public DataTransformer {
        public:

        B64Encoder();
        ~B64Encoder();

        /**
         * Encodes the input data in base 64. Note that new memory is
         * allocated for the encoded data. This must be freed by the
         * caller with the C++ delete operator.
         *
         * @param data the data to encode
         * @param param IN/OUT transformation info
         */
        char* transform(char* data, TransformationInfo& info);
    };
 #endif

