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
#ifndef INCL_PROPPARAM
#define INCL_PROPPARAM

#include "base/fscapi.h"
#include "base/util/ArrayList.h"

class PropParam : public ArrayElement {

    // ------------------------------------------------------- Private interface
private:

    BCHAR* paramName;
    BCHAR* dataType;
    ArrayList* valEnums;
    BCHAR* displayName;



    // ----------------------------------------------------- Protected interface
protected:




    // -------------------------------------------------------- Public interface
public:


    /*
     * PropParam constructor
     * 
     */
    PropParam();

    ~PropParam();


    /*
     * PropParam constructor
     * 
     * @param p0 prop name
     * @param p1 data type
     * @param p2 values enum
     * @param p3 display name
     */
    PropParam(BCHAR* p0, BCHAR* p1, ArrayList* p2, BCHAR* p3);



    /*
     * getDisplayName
     * 
     */
    BCHAR* getDisplayName(BCHAR* displayName = NULL);


    /*
     * getParamName
     * 
     */
    BCHAR* getParamName(BCHAR* paramName = NULL);


    /*
     * setParamName
     * 
     * @param p0
     */
    void setParamName(BCHAR* p0);


    /*
     * getDataType
     * 
     */
    BCHAR* getDataType(BCHAR* dataType = NULL);


    /*
     * setDataType
     * 
     * @param p0
     */
    void setDataType(BCHAR* p0);


    /*
     * getValEnums
     * 
     */
    ArrayList* getValEnums();


    /*
     * setValEnums
     * 
     * @param p0
     */
    void setValEnums(ArrayList* p0);


    /*
     * setDisplayName
     * 
     * @param p0
     */
    void setDisplayName(BCHAR* p0);

    /*
     * Creates a clone of this instance
     *
     * @return the newly created instance
     */
    ArrayElement* clone();


};


#endif
