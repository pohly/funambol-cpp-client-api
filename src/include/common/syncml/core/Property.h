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
#ifndef INCL_PROPERTY
#define INCL_PROPERTY

#include "base/fscapi.h"
#include "base/util/ArrayList.h"

class Property : public ArrayElement {

    // ------------------------------------------------------- Private interface
private:

    wchar_t* propName;
    wchar_t* dataType;
    long maxOccur;
    long maxSize;
    BOOL noTruncate;  // -1 undefined, 0 FALSE, 1 TRUE
    ArrayList* valEnums;
    wchar_t* displayName;
    ArrayList* propParams;



    // ----------------------------------------------------- Protected interface
protected:




    // -------------------------------------------------------- Public interface
public:


    /*
     * Property constructor
     *
     */
    Property();

    ~Property();


    /*
     * Property constructor
     *
     * @param p0
     * @param p1
     * @param p2
     * @param p3
     * @param p4
     * @param p5
     * @param p6
     * @param p7
     */
    Property(wchar_t* p0, wchar_t* p1, long p2, long p3, BOOL p4, ArrayList* p5, wchar_t* p6, ArrayList* p7);



    /*
     * getDisplayName
     *
     */
    wchar_t* getDisplayName(wchar_t* displayName = NULL);

    /*
     * getPropName
     *
     */
    wchar_t* getPropName(wchar_t* propName = NULL);


    /*
     * setPropName
     *
     * @param p0
     */
    void setPropName(wchar_t* propName);


    /*
     * getDataType
     *
     */
    wchar_t* getDataType(wchar_t* dataType = NULL);


    /*
     * setDataType
     *
     * @param p0
     */
    void setDataType(wchar_t* p0);


    /*
     * getMaxOccur
     *
     */
    long getMaxOccur();


    /*
     * setMaxOccur
     *
     * @param p0
     */
    void setMaxOccur(long p0);


    /*
     * getMaxSize
     *
     */
    long getMaxSize();


    /*
     * setMaxSize
     *
     * @param p0
     */
    void setMaxSize(long p0);


    /*
     * setNoTruncate
     *
     * @param p0
     */
    void setNoTruncate(BOOL p0);


    /*
     * isNoTruncate
     *
     */
    BOOL isNoTruncate();


    /*
     * getNoTruncate
     *
     */
    BOOL getNoTruncate();


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
    void setDisplayName(wchar_t* p0);


    /*
     * getPropParams
     *
     */
    ArrayList* getPropParams();


    /*
     * setPropParams
     *
     * @param p0
     */
    void setPropParams(ArrayList* p0);

    /*
     * setPropParams
     *
     * @param p0
     */
    void setPropParams(ArrayList& p0);


    /*
     * Creates an exact copy of this Property
     *
     * @return the cloned instance
     */
    ArrayElement* clone();


};


#endif
