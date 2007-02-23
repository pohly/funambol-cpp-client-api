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

#include "base/fscapi.h"
#include "base/util/utils.h"
#include "spds/DataTransformer.h"


DataTransformer::DataTransformer() : name(NULL) {

}

DataTransformer::DataTransformer(char* n) {
    name = stringdup(n);
}

DataTransformer::~DataTransformer() {
    safeDelete(&name);
}

void DataTransformer::setName(const char*n) {
    if (name) {
        safeDelete(&name);
    }

    name = stringdup(n);
}

const char* DataTransformer::getName() {
    return name;
}

