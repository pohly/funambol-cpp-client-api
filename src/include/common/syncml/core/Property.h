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
#ifndef INCL_PROPERTY
#define INCL_PROPERTY
/** @cond DEV */

#include "base/fscapi.h"
#include "base/util/ArrayList.h"

class Property : public ArrayElement {

    // ------------------------------------------------------- Private interface
private:

    char*  propName;
    char*  dataType;
    long maxOccur;
    long maxSize;
    BOOL noTruncate;  // -1 undefined, 0 FALSE, 1 TRUE
    ArrayList* valEnums;
    char*  displayName;
    ArrayList* propParams;



    // ----------------------------------------------------- Protected interface
protected:




    // -------------------------------------------------------- Public interface
public:

    /*
     * Property constructor
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
    Property(char*  p0, char*  p1, long p2, long p3, BOOL p4, ArrayList* p5, char*  p6, ArrayList* p7);


    /*
     * getDisplayName
     *
     */
    const char* getDisplayName();

    /*
     * getPropName
     *
     */
    const char* getPropName();


    /*
     * setPropName
     *
     * @param p0
     */
    void setPropName(const char* propName);


    /*
     * getDataType
     *
     */
    const char* getDataType();


    /*
     * setDataType
     *
     * @param p0
     */
    void setDataType(const char* p0);


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
    void setDisplayName(const char* p0);


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


/** @endcond */
#endif
