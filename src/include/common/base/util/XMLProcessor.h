/**
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


#ifndef INCL_XML_PROCESSOR
#define INCL_XML_PROCESSOR

#include "base/fscapi.h"
#include "base/util/StringBuffer.h"
#include "base/util/KeyValuePair.h"
#include "base/util/ArrayList.h"

#define DIM_TAG 32


/*
 * This class implements a minimalist XML processor used to extract
 * the content between a known tag, and to make an XML element with
 * a specified value.
 *
 */

class XMLProcessor  {

public:

    /*
     * Extracts the content of a tag into an XML message. It is supposed that the
     * message is a valid XML message. It returns NULL in case the tag is not
     * found or the XML fragment is not in the expected form.
     * The returned pointer (if not NULL) is allocated with the new operator and
     * must be discarded with the operator delete.
     *
     * @param xml the xml fragment
     * @param tag the tag we want the content
     * @param pos (OUTPUT) the position where the tag is found (ignored if NULL)
     *
     */
    static char*  getElementContent(const char*  xml, const char*  tag, unsigned int* pos = NULL) EXTRA_SECTION_00;

    /*
     * It's like getElementContent but it doesn't allocate new memory.
     *
     * @param xml the xml fragment
     * @param tag the tag we want the content
     * @param pos (OUTPUT) the position where the tag is found (ignored if NULL)
     * @param startPos (OUTPUT) the start position of the tag content (ignored if NULL)
     * @param endPos (OUTPUT) the end position of the tag content (ignored if NULL)
     */
    static const char*  getElementContent(const char*  xml, const char*  tag,
                    unsigned int* pos, unsigned int* startPos, unsigned int* endPos) EXTRA_SECTION_00;
    
    /*
     * It's like getElementContent above but it works on escaped XML tags.
     *
     * @param xml the xml fragment
     * @param tag the tag we want the content
     * @param pos (OUTPUT) the position where the tag is found (ignored if NULL)
     * @param startPos (OUTPUT) the start position of the tag content (ignored if NULL)
     * @param endPos (OUTPUT) the end position of the tag content (ignored if NULL)
     */
    static const char*  getEscapedElementContent(const char*  xml, const char*  tag,
                    unsigned int* pos, unsigned int* startPos, unsigned int* endPos) EXTRA_SECTION_00;
    
    /*
    * It returns the number of the tag in the xml string
    */
    static int countElementTag(char*  xml, char*  tag) EXTRA_SECTION_00;
    
    static int countAnd(char*  token) EXTRA_SECTION_00;

    static int countChar(char*  token, char*  element) EXTRA_SECTION_00;
    
    static char*  getNextTag(char*  xml, int* pos);
   /*
    * it's as getElementContent but it doesn't get the content of a tag if
    * the parent match except.
    * The parent can be more than one. They have to be separated by &
    * i.e.  
    *
    * getElementContentExcept(xmlPtr, T("Add"), T("Sync&Atomic"), &post)
    *
    * The function returns "... to keep ... " content only 
    *
    * <SyncBody>
    *   <Sync>
    *     <Add>... to avoid ...</Add>
    *   </Sync>
    *   <Add>... to keep ...</Add>
    *   <Sync>
    *     <Add>... to avoid ...</Add>
    *   </Sync>
    *    <Atomic>
    *     <Add>... to avoid ...</Add>
    *   </Atomic>
    * </SyncBody>
    */
    static char*  getElementContentExcept(char*       xml       ,
                                            char*       tag       ,
                                            char*       except    ,
                                            unsigned int* pos) EXTRA_SECTION_00;

    static char*  getElementContentLevel(char*       xml,
                                           char*       tag,
                                           unsigned int* pos,
                                           int           lev = 0 ,  // the root value level
                                           int* startLevel   = NULL) EXTRA_SECTION_00;

    /*
     * It returns the content of the buffer specified by startPos (initial position)
     * and and endPos (the end position)
     *
     * It allocates new memory that has to be freed by caller.
     *
     * @param xml the xml fragment
     * @param startPos the start position of the tag content
     * @param endPos  the end position of the tag content
     *
     */

    static char*  getContent(const char*  xml, unsigned int startPos, unsigned int endPos) EXTRA_SECTION_00;
    
    /**
     * Create an XML element with the specified tag and value.
     *
     * @param tag - the tag we want the content
     * @param val - the element value
     */
    //static StringBuffer makeElement(const WCHAR* tag, const WCHAR* val) EXTRA_SECTION_00;

    /**
     * Create an XML element with the specified tag, value and attribute list.
     *
     * @param tag - the tag we want the content
     * @param val - the element value
     * @param attr - attribute list, already formated - ex: enc="base64"
     */
    static StringBuffer makeElement(const char*  tag, 
                                    const char*  val,
                                    const char*  attr = NULL) EXTRA_SECTION_00;

    /**
     * Create an XML element with the specified tag, value and attribute list.
     *
     * @param tag - the tag we want the content
     * @param val - the element value
     * @param attr - attribute list, pairs of name and value
     */
    static StringBuffer makeElement(const char*  tag, 
                                    const char*  val,
                                    ArrayList attrList) EXTRA_SECTION_00;

	/**
	 * Same with bool value
	 */
    static StringBuffer makeElement(const char*  tag, bool val) {
        return makeElement( tag, (val?T("true"):T("false")) ) EXTRA_SECTION_00 ;
    }

    /**
	 * Same with integer value
	 */
	static StringBuffer makeElement(const char*  tag, int val) {
        return makeElement( tag, StringBuffer().append(val) ) EXTRA_SECTION_00 ;
    }
    
    /**
     * Extracts the attributes list from a tag into an XML message. It is supposed that the
     * message is a valid XML message. It returns NULL in case the tag is not
     * found or the XML fragment is not in the expected form. The calling function must 
     * further parse the resultto extract attribute name and value
     *
     * @param xml the xml fragment
     * @param tag the tag whom attributes we want
     * @param startPos (OUTPUT) the start position of the tag attributes (ignored if NULL)
     * @param endPos (OUTPUT) the end position of the tag attributes (ignored if NULL)
     *
    */
    static const char*  getElementAttributes(
                                         const char*  xml,
                                         const char*  tag,
                                         unsigned int*  startPos, 
                                         unsigned int*  endPos,
                                         bool escaped = false) EXTRA_SECTION_00;

};

#endif

