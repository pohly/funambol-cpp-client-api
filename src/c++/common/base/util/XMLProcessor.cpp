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


#include <stdlib.h>

#include "base/util/utils.h"
#include "base/util/XMLProcessor.h"
#include "base/util/StringBuffer.h"
#include "base/Log.h"

//--------------------------------------------------------------- Static functions

static const BCHAR *findElementContent(const BCHAR *xml,
                          const BCHAR *openTag, const BCHAR *closeTag,
                          unsigned int* pos     ,
                          unsigned int* startPos,
                          unsigned int* endPos  )
{
    const BCHAR *p1, *p2, *xmlptr = xml;

    if (pos) {
        *pos = 0;
    }

    do {
        p1 = bstrstr(xmlptr, openTag);
        p2 = NULL;

        if (!p1) {
            // Tag not found
            //LOG.debug("XMLProcessor: tag %s not found", openTag);
            return 0;
        }
        
        p1 += bstrlen(openTag); // move to end of tag

        // Check the tag type
        switch( *p1 ){
            case ' ':   // <tag attr=xxx>
                // Find the end of the tag (TODO: should check for invalid chars?)
                for (p1++; *p1 != '>'; p1++) {
                    if(*p1 == 0 || *p1 == '<'){
                        LOG.info("XMLProcessor: incomplete tag");
                        return 0;
                    }
                }
                // The break is not missing!
                // After the for, we are in the same case of the tag
                // without attributes
            case '>':   // <tag>
                p1++;   // this is the beginning of content
                if (!p1[0]) {
                    LOG.info("XMLProcessor: tag at end of file");
                    return 0;
                }
                // Find the closing tag
                p2 = bstrstr(p1, closeTag);
                break;
            case '/':
                p1++;
                if(*p1 != '>'){
                    LOG.info("XMLProcessor: invalid empty tag");
                    return 0;
                }
                p1++;
                // The tag is already closed, no content: make end = start
                p2=p1;
                // Invalidate closeTag
                closeTag=0;
                break;
            default:
                // This is not the searched tag, search again
                //LOG.debug("XMLProcessor: this is not tag %s", openTag);
                xmlptr = p1;
                p1 = 0;
        }
    }
    while (!p1); // If p1 is null and we are here, it means that the 'default'
                 // case was hit.

    // Closing tag not found
    if (!p2) {
        //
        // This is abc<tag>xyz\0
        //
        p1 = NULL;
        return 0;
    }
    
    // Okay, if we are here, the tag content has been found
    if (startPos) {
        *startPos = p1 - xml;
    }
    if (endPos) {
        *endPos = p2 - xml ;
    }
    if (pos) {
        *pos = p2-xml;
        if (closeTag){
            *pos += bstrlen(closeTag);
        }
    }

    return p1;
}


const BCHAR* XMLProcessor::getEscapedElementContent(const BCHAR* xml ,
                                              const BCHAR* tag ,
                                              unsigned int* pos ,
                                              unsigned int* startPos,
                                              unsigned int* endPos )
{        
    BCHAR *openTag = 0;
    BCHAR *closeTag = 0;
    
    if (!xml) {
        return 0;
    }

    size_t l = bstrlen(tag);

    if(bstrcmp(tag, T("CDATA")) == 0) {
        openTag = stringdup(T("&lt;![CDATA["));
        closeTag = stringdup(T("]]>"));
    }
    else {
        openTag = new BCHAR[l+10];
        closeTag = new BCHAR[l+10];
        bsprintf(openTag, T("&lt;%s"), tag);
        bsprintf(closeTag, T("&lt;/%s>"), tag);
    }

    const BCHAR *ret = findElementContent(xml, openTag, closeTag, pos, startPos, endPos);

    if (openTag)
        delete [] openTag;
    if (closeTag)
        delete [] closeTag;

    return ret;
}

const BCHAR* XMLProcessor::getElementContent(const BCHAR* xml,
                                       const BCHAR* tag,
                                       unsigned int* pos,
                                       unsigned int* startPos,
                                       unsigned int* endPos  )
{        
    BCHAR *openTag = 0;
    BCHAR *closeTag = 0;
    
    if (!xml) {
        return 0;
    }

    size_t l = bstrlen(tag);

    if(bstrcmp(tag, T("CDATA")) == 0) {
        openTag = stringdup(T("<![CDATA["));
        closeTag = stringdup(T("]]>"));
    }
    else {
        openTag = new BCHAR[l+10];
        closeTag = new BCHAR[l+10];
        bsprintf(openTag, T("<%s"), tag);
        bsprintf(closeTag, T("</%s>"), tag);
    }

    const BCHAR *ret = findElementContent(xml, openTag, closeTag, pos, startPos, endPos);

    if (openTag)
        delete [] openTag;
    if (closeTag)
        delete [] closeTag;

    return ret;
}

BCHAR* XMLProcessor::getContent(const BCHAR* xml,
                                unsigned int startPos,
                                unsigned int endPos  ) {

    BCHAR * ret = NULL;

    if (!xml) {
        return 0;
    }
    if (endPos < startPos) {
        return 0;
    }
    if (bstrlen(xml) < endPos - startPos) {
        return 0;
    }

    ret = new BCHAR[endPos - startPos + 1];

    bstrncpy(ret, xml+startPos, endPos - startPos);
    ret[endPos - startPos] = 0;

    return ret;
}

BCHAR* XMLProcessor::getElementContent(const BCHAR* xml,
                                       const BCHAR* tag,
                                       unsigned int* pos)
{
    unsigned int start, end;

    if( getElementContent (xml, tag, pos, &start, &end) ) {
        return getContent(xml, start, end);
    }
    return 0;
}

/*
* It returns the number of the tag in the xml string
*/

int XMLProcessor::countElementTag(BCHAR* xml, BCHAR* tag) {

    unsigned int count = 0, pos = 0, previous = 0;
    
    while (getElementContent(&xml[pos], tag , &pos, NULL, NULL) != NULL) {
        pos += previous;
        previous = pos;
        count ++;
    }
    return count;
}


/*
* Returns the next tag found in the xml string. It looks at the < and > tags to retrieve 
* the name of the token. 
* If <tag xmlns...> it returns "tag"
* The "pos" argument will contain the position of the close <tag/>
* The return value is a new BCHAR* and must be fred by the caller. If no tag is found, NULL is returned
*/
BCHAR* XMLProcessor::getNextTag(BCHAR* xml, int* pos) {
    
    BCHAR* p1, *p2, *p4, *p3 = NULL, *ret = NULL;
    p1 = p2 = p4 = xml;
    int i = 0, k = 0, len = 0;
    BOOL found = FALSE;
    len = bstrlen(xml);     
    
    for (i = 0; i < len; i++) {
        if (found) {
            if (p4[i] != '/' && p4[i] != '!' && p4[i] != '-' ) {
                break; // the element found is right!
            } else {
                found = FALSE;
            }
        }
        if (p4[i] == '<') {            
            p1 = &p4[i];            
            found = TRUE;
        }
    }
    
    if (found) {
        p2 = p1;           
        for (k = 0; k < len; k++) {             
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == ' ') {
                p3 = p1;
            }
            else if (*p1 == '>') { 
                *pos = p1 -xml + 1;
                if (p3) {                    
                    p1 = p3;
                }
                ret = new BCHAR[(p1)-(p2)];
                bstrncpy(ret, p2+1, (p1)-(p2+1));
                ret[(p1)-(p2+1)] = 0;
                return ret;
                break;
            }            
            p1 = p1 + 1;
        }
    }
    return ret;

}



/*
* count the number of "&" (passed as a string) in the token. 
*/
int XMLProcessor::countAnd(BCHAR* token) {
    return countChar(token, T("&"));
}

int XMLProcessor::countChar(BCHAR* token, BCHAR* element) {

    BCHAR* p1, *p2;
    p1 = p2 = token;
    int i = 0, k = 0, len = 0;

    while (bstrstr(p1, element) != NULL) {
        len = bstrlen(p2);        
        for (k = 0; k < len; k++) {             
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == '&') {                
                p1 = p1 + 1;
                i++;
                break;
            }            
            p1 = p1 + 1;
        }  
    }
    return i;
}


/*
* it's as getElementContent but it doesn't get the content of a tag if the parent match except.
* The parent can be more than one. They have to be separated by &
* i.e.  
*
* getElementContentExcept(xmlPtr, T("Add"), T("Sync&Atomic"), &post)
*
* The function returns "... to keep ... " content only 
*
* <SyncBody>
   <Sync>
     <Add>... to avoid ...</Add>
   </Sync>
   <Add>... to keep ...</Add>
   <Sync>
     <Add>... to avoid ...</Add>
   </Sync>
    <Atomic>
     <Add>... to avoid ...</Add>
   </Atomic>
 </SyncBody>
*/

BCHAR* XMLProcessor::getElementContentExcept(BCHAR*      xmlPtr    ,
                                               BCHAR*      tag       ,
                                               BCHAR*      except    ,
                                               unsigned int* post) {
    
    BCHAR*  ret    = NULL;
    const BCHAR*  found  = NULL;
    BCHAR*  xml    = NULL;
    BCHAR** array = NULL;
    int*  validElement = NULL;   
    int count        = 0, countTag = 0;
    BOOL notValid  = FALSE;

    unsigned int pos      = 0, previous         = 0,
                 position = 0, previousPosition = 0,
                 startPos = 0, endPos           = 0;
    
    xml = xmlPtr;
    
    if (xml == NULL) {
        return NULL;
    }     
    
    if (except == NULL) {
        ret = getElementContent(xml, tag, &pos);
        if (post) {
            *post = pos;
        }
        return ret;
    }
    count = countAnd(except);    
    count++;
    
    array = new BCHAR*[count + 1];
	 int l;
    for (l = 0; l <= count; l++) {
        array[l] = NULL;
    }
    
    // represent a element found that can be used properly
    countTag = countElementTag(xml, tag);
    validElement = new int[countTag];
    for (l = 0; l < countTag; l++) {
        validElement[l] = 1;
    }

    BCHAR* internal = stringdup(except);
    BCHAR* p1, *p2;
    p1 = p2 = internal;
    int i = 0, k = 0, len = 0;

    while (bstrstr(p2, T("&")) != NULL) {
        len = bstrlen(p2);        
        for (k = 0; k < len; k++) {             
            if (*p1 == 0) {
                break;
            }
            else if (*p1 == '&') {                
                *p1 = 0;
                array[i] = stringdup(p2);
                p1 = p1 + 1;
                p2 = p1;
                i++;
                break;
            }            
            p1 = p1 + 1;
        }  
    }
    
    if (i == 0 || k < len) {
        array[i] = stringdup(p2);
        i++;
    }
             
    for (int s = 0; s < count; s ++) {
        i = 0;

        do {
            safeDel(&ret);
            k = 0;

            pos = 0, previous = 0;
            while ((ret = getElementContent(&xml[pos], tag, &pos)) != NULL) {    
            
                if (validElement[k] == 1) {
                    pos += previous;
                    position = 0; 
                    previousPosition = 0;
                    startPos = 0; 
                    endPos   = 0;
                    while ((found = getElementContent(&xml[position], array[i], &position, &startPos, &endPos)) != NULL ) {
        
                        startPos += previousPosition;
                        endPos   += previousPosition;
                        position += previousPosition;
                        if (startPos < pos && pos < endPos) {
                            notValid = TRUE;                    
                            break;
                        }
                        previousPosition = position;
                    }
                
                    if (notValid) {
                        notValid = FALSE;     
                         safeDel(&ret);
                        validElement[k] = 0;
                    } else {
                        if (post) {
                            *post = pos;
                        }
                        break;
                    }
                    previous = pos;
                    k++;
                } else {
                    pos += previous;
                    previous = pos;
                    k++;
                }
            }
            i++;

        } while(array[i] != NULL);

        if (count > 1) {
            BCHAR* tmp = stringdup(array[0]);
    
            for (int m = 0; m < count - 1; m++) {
                safeDel(&array[m]);  
                array[m] = stringdup(array[m+1]);
            }
            //safeDel(&array[count-1]);
            array[count-1] = stringdup(tmp);
            safeDel(&tmp); 
        }
    }       
    
    if (notValid) {
        if (ret) {
            safeDel(&ret);
        }
        if (post) {
            *post = 0;
        }
    } else {
        ;
    }
    
    safeDel(&internal);
    if (validElement) {
        delete [] validElement; validElement = NULL;
    }

    safeDelete(array);   

    return ret;
}

/*
* It returns getElementContent value but it depends on the level that is specified.
* It return only ... to keep ... independently on the tag in which it is contained if lev is zero.
* 
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
*
* The parameters are:
* xml: the xml to analize
* tag: the name of the tag to use
* lev: the inner level in which discover the tag
* startLevel: the starting level from which start the search
* pos: the position index search end
*
* The follow piece of code explain how get the value tag "... to keep ..." that is in the root (0) level.
* Note the startLevel declaration and initialization to -1 value
*
*
*    BCHAR* p = NULL;
*    unsigned int pos = 0, previous = 0;
*    int startLevel = -1;
*    while ((p = XMLProcessor::getElementContentLevel(&xml[pos], T("Add"), 0, &startLevel, &pos)) != NULL) {        
*        pos += previous;
*        previous = pos;                
*    }
*
*/

BCHAR* XMLProcessor::getElementContentLevel(BCHAR*      xml   ,
                                              BCHAR*      tag   ,                                              
                                              unsigned int* pos, 
                                              int           lev ,   
                                              int*          startLevel)  {
    
    BCHAR* p1       = NULL;
    BCHAR* p2       = NULL;
    BCHAR* ret      = NULL;
    BOOL openBracket  = FALSE;  // <
    BOOL closeBracket = FALSE;  // >
    BOOL aloneBracket = FALSE;  // </
    BOOL preCloseBracket = FALSE;  //<.../
    BOOL openTag      = FALSE;
    BOOL closeTag     = FALSE;

    BCHAR tagNameFound[40];
    
    int level               = -1;
    unsigned int xmlLength  = (unsigned int)-1;
    unsigned int l          = (unsigned int)-1;
    unsigned int previousIndex = (unsigned int)-1;
    unsigned int i          =  0;    
    
    if (xml == NULL) {
        goto finally;
    }
    
    if (lev < 0) {
        return getElementContent(xml, tag, pos);
    }        
   
    xmlLength = bstrlen(xml);
    l = bstrlen(tag);

    if (pos != NULL) {
        *pos = 0;
    }
    if (startLevel != NULL) {
       level = *startLevel;
    }

    p1 = p2 = xml;
    
    for (i = 0; i < xmlLength; i ++) {
        
        if (p1[i] == '<') {
            openBracket = TRUE;
            previousIndex = i;
            p2 = &p1[i];

        } else if (p1[i] == '/') {
            if (previousIndex == (i - 1)) {
                // <.../>                
                preCloseBracket = TRUE;
            } else {
                // TBD  </...>
            }

        } else if (p1[i] == '>') {  
            
            if (openBracket == FALSE) {
                closeBracket = FALSE;
                preCloseBracket = FALSE;
            } else {
                if (preCloseBracket) {
                    closeTag = TRUE;
                }
                else { 
                    openTag = TRUE;                    
                }
                closeBracket = TRUE;
                
                if (closeTag) {
                    level--;
                    openBracket  = FALSE;
                    closeBracket = FALSE;
                    preCloseBracket = FALSE;
                    openTag      = FALSE;
                    closeTag     = FALSE;

                } else if (openTag) {
                    level++;
                } else {
                    openBracket  = FALSE;
                    closeBracket = FALSE;
                    preCloseBracket = FALSE;
                    openTag      = FALSE;
                    closeTag     = FALSE;

                }                
            }
        }         
          if (openTag && openBracket && closeBracket) {
            int n = (&p1[i] - p2 - 1);
            bstrncpy(tagNameFound, p2 + 1, n);         
            tagNameFound[n] = 0;
            if (bstrcmp(tagNameFound, tag) == 0 && (level == lev)) {
                unsigned int internalPos;
                ret = getElementContent(p2, tag, &internalPos);
                if (pos) {
                    *pos = p2 - xml + internalPos;                    
                }
                if (startLevel) {
                    *startLevel = level - 1;
                }
                break;
            }
            openBracket  = FALSE;
            closeBracket = FALSE;           
        }
    }

finally:
    openBracket  = FALSE;
    closeBracket = FALSE;
    preCloseBracket = FALSE;
    openTag      = FALSE;
    closeTag     = FALSE;
    return ret;

}

/**
 * Get the attribute list of the forst element 'tag', returning a pointer 
 * to the beginning of the string in the original buffer 'xml', and the 
 * starting and ending position of the substring.
 *
 * @param xml - the XML document to process.
 * @param tag - the tag name to find
 * @param startPos - return value - the start pos of the attribute list
 * @param endPos - return value - the end position of the attribute list
 */
const BCHAR* XMLProcessor::getElementAttributes(const BCHAR* xml,
                                          const BCHAR* tag,
                                          unsigned int* startPos,
                                          unsigned int* endPos, 
                                          bool escaped) {
        
    const BCHAR* p1 = NULL;
    const BCHAR* p2 = NULL;
    BOOL charFound  = FALSE;

    // example ot tag with attribute list
    // <body enc="base64">
    BCHAR *openTag = 0; //<tag
    
    if (!xml) {
        goto finally;
    }

    unsigned int l = bstrlen(tag);

    if(bstrcmp(tag, T("CDATA")) == 0) {
        goto finally;
    }
    else {
        openTag = new BCHAR[l+10];
        if (escaped){
            bsprintf(openTag, T("&lt;%s "), tag);
        }
        else{
            bsprintf(openTag, T("<%s "), tag);
        }
    }

    p1 = bstrstr(xml, openTag);

    if (!p1) {
        LOG.info("XMLProcessor: tag %s not found", tag);
        goto finally;
    }
    // move to the beginning of the attribute list
    p1 += bstrlen(openTag);

    // find the end of the tag
    for (p2 = p1; *p2 != '>'; p2++) {
        if(*p2 == 0 || *p2 == '<'){
            LOG.info("XMLProcessor: incomplete tag");
            goto finally;
        }
    }
    // set the return parameters
    if (startPos != NULL) {
        *startPos = p1 - xml;
    }
    if (endPos != NULL) {
        *endPos = p2 - xml ;
    }

    finally:

    if (openTag)
        delete [] openTag;

    return p1;

}


StringBuffer XMLProcessor::makeElement(const BCHAR* tag, const BCHAR* val, const BCHAR* attr)
{
    StringBuffer s;
        
    if (!val)
        return s;
    if (!val[0])
        return s;
    
    size_t len = bstrlen(tag);
    BCHAR* t1 = new BCHAR[len + 4]; // <  >  0, whitout closing >
    BCHAR* t2 = new BCHAR[len + 6]; // </ > \n 0

    sprintf(t1, T("<%s"), tag);    
    sprintf(t2, T("</%s>\n"), tag);

    s = t1; 
    if (attr != NULL)
    {
        s += " ";
        s += attr;        
    }
    s += ">";
    s += val; s += t2;

    delete [] t1;
    delete [] t2;
    
    return s;    
}


StringBuffer XMLProcessor::makeElement(const BCHAR* tag, 
                                    const BCHAR* val,
                                    ArrayList attrList) {

    StringBuffer s;
    
    for (int i = 0; i < attrList.size(); i++)
    {
        KeyValuePair* item = (KeyValuePair*)attrList[i];
        if (i > 0)
            s += " ";
        s += item->getKey(); 
        s += "=\"";
        s += item->getValue(); 
        s += "\"";
    }    
    s = makeElement(tag, val, s.c_str());

    return s;
}
