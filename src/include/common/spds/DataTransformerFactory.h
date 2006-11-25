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
 * This class is factory for other DataTransformer istances.
 */

 #ifndef INCL_DATA_TRANSFORMER_FACTORY
    #define INCL_DATA_TRANSFORMER_FACTORY

    #include "spds/DataTransformer.h"

    #define DF_FORMATTERS "b64;des;"


    class DataTransformerFactory {

    public:

        static DataTransformer* getEncoder(const char*  name) EXTRA_SECTION_01;
        static DataTransformer* getDecoder(const char*  name) EXTRA_SECTION_01;
        static BOOL isSupportedEncoder(const char*  name) EXTRA_SECTION_01;
        static BOOL isSupportedDecoder(const char*  name) EXTRA_SECTION_01;

    };

#endif
