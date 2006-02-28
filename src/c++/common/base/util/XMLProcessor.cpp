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


wchar_t* XMLProcessor::getElementContent(const wchar_t*      xml     ,
                                         const wchar_t*      tag     ,
                                         unsigned int* pos     ,
                                         unsigned int* startPos,
                                         unsigned int* endPos  ) {
        
    wchar_t* p1       = NULL;
    wchar_t* p2       = NULL;
    BOOL charFound    = FALSE;
    unsigned int xmlLength = (unsigned int)-1;
    unsigned int l = (unsigned int)-1;

    wchar_t *openTag = 0;
    wchar_t *closeTag = 0;
    
    if (xml == NULL) {
        goto finally;
    }

    xmlLength = wcslen(xml);
    l = wcslen(tag);

    if (pos != NULL) {
        *pos = 0;
    }

    if(wcscmp(tag, TEXT("CDATA")) == 0) {
        openTag = stringdup(TEXT("<![CDATA["));
        closeTag = stringdup(TEXT("]]>"));
    }
    else {
        openTag = new wchar_t[l+10];
        closeTag = new wchar_t[l+10];
        wsprintf(openTag, TEXT("<%s>"), tag);
        wsprintf(closeTag, TEXT("</%s>"), tag);
    }

    p1 = wcsstr((wchar_t*)xml, openTag);

    if (p1 == NULL) { // tag can have attributes or can be empty

        // if p1 is null I try to discover the next '>' char to close the tag. If does not exist 
        // return NULL
        
        // try to find "<tagName/>". If found it return null.
        wsprintf(openTag, TEXT("<%s/>"), tag);
        p1 = wcsstr((wchar_t*)xml, openTag);
        // ok, found an empty tag
        if (p1 != NULL) {
            goto finally;
        }

        // try to find "<tagName"
        wsprintf(openTag, TEXT("<%s"), tag);
        p1 = wcsstr((wchar_t*)xml, openTag);

        if (p1 == NULL) {
            goto finally;
        }
        
        p1 = p1 + l + 1;   // <body_
        
        for (unsigned int k = 0; k < xmlLength; k++) { // Suppose max length as the xml string
            p1 = p1 + 1;
            if (*p1 == 0) {
                goto finally;
            }
            else if (*p1 == '>') {
                charFound = TRUE;
                p1 = p1 + 1;
                break;
            }            
        }
        if (!charFound)
            goto finally;


    } else {  // tag doesn't have attribute. Original version
    
        p1 += wcslen(openTag);
    
    }
    if (*p1 == 0) {
        //
        // This is abc<tag>\0
        //
        goto finally;
    }

    p2 = wcsstr(p1, closeTag);

    if (p2 == NULL) {
        //
        // This is abc<tag>xyz\0
        //
        p1 = NULL;
        goto finally;
    }
        
    if (pos != NULL) {
        *pos = p1-xml+wcslen(openTag);
    }
    if (startPos != NULL) {
        *startPos = p1 - xml;
    }
    if (endPos != NULL) {
        *endPos = p2 - xml ;
    }

    finally:

    if (openTag)
        delete [] openTag;
    if (closeTag)
        delete [] closeTag;

    return p1;

}

wchar_t* XMLProcessor::getContent(wchar_t*     xml     ,
                                  unsigned int startPos,
                                  unsigned int endPos  ) {

    wchar_t * ret = NULL;

    if (xml == NULL) {
        goto finally;
    }
    if (endPos <= startPos) {
        goto finally;
    }
    if (wcslen(xml) < endPos - startPos) {
        goto finally;
    }

    ret = new wchar_t[endPos - startPos + 1];

    wcsncpy(ret, xml, endPos - startPos);
    ret[endPos - startPos] = 0;

finally:

    return ret;
}

wchar_t* XMLProcessor::getElementContent(const wchar_t*      xml       ,
                                         const wchar_t*      tag       ,
                                         unsigned int* pos) {
    wchar_t* p1       = NULL;
    wchar_t* p2       = NULL;
    wchar_t* ret      = NULL;
    BOOL charFound    = FALSE;
    unsigned int xmlLength  = (unsigned int)-1;
    unsigned int l          = (unsigned int)-1;

    if (xml == NULL) {
        goto finally;
    }

    wchar_t openTag[40];
    wchar_t closeTag[40];
    
    xmlLength = wcslen(xml);
    l = wcslen(tag);

    if (pos != NULL) {
        *pos = 0;
    }

    wsprintf(openTag, TEXT("<%s>"), tag);
    wsprintf(closeTag, TEXT("</%s>"), tag);

    p1 = wcsstr((wchar_t*)xml, openTag);

    if (p1 == NULL) { // tag can have attributes
        //
        // This is abcxyz
        //
        // goto finally;
    
        // try to find "<tagName/>". If found it return empty string.
        wsprintf(openTag, TEXT("<%s/>"), tag);
        p1 = wcsstr((wchar_t*)xml, openTag);

        if (p1 != NULL) {
            ret = new wchar_t[2];
            ret[0] = 0;
            ret[1] = 0;
            p2 = p1 + l + 3;
            if (pos != NULL) {
                *pos = p2-xml+l+3;
            }
            goto finally;
        }
        
        // try to find "<tagName"
        wsprintf(openTag, TEXT("<%s"), tag);
        p1 = wcsstr((wchar_t*)xml, openTag);

        if (p1 == NULL) {
            goto finally;
        }
        
        p1 = p1 + l + 1;   // <body_ 

        for (unsigned int k = 0; k < xmlLength; k++) { // Suppose max length as the xml string
            p1 = p1 + 1;
            if (*p1 == 0) {
                goto finally;
            }
            else if (*p1 == '>') {
                charFound = TRUE;
                p1 = p1 + 1;
                break;
            }            
        }
        if (!charFound)
            goto finally;             
    
    } else {  // tag doesn't have attribute. Original version
        
        p1 = p1+l+2;

    }
    if (*p1 == 0) {
        //
        // This is abc<tag>\0
        //
        goto finally;
    }

    p2 = wcsstr(p1, closeTag);

    if (p2 == NULL) {
        //
        // This is abc<tag>xyz\0
        //
        goto finally;
    }

    ret = new wchar_t[p2-p1+1];
    
    if (ret != NULL) {
        wcsncpy(ret, p1, p2-p1);
        ret[p2-p1] = 0;
    } 
    // if no enough memory to instantiate the new object...
    else {
        ret = TEXT("");
    }

    if (pos != NULL) {
        *pos = p2-xml+l+3;
    }

    finally:

    return ret;

}

/*
* It returns the number of the tag in the xml string
*/

int XMLProcessor::countElementTag(wchar_t* xml, wchar_t* tag) {

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
* The return value is a new wchar_t* and must be fred by the caller. If no tag is found, NULL is returned
*/
wchar_t* XMLProcessor::getNextTag(wchar_t* xml, int* pos) {
    
    wchar_t* p1, *p2, *p4, *p3 = NULL, *ret = NULL;
    p1 = p2 = p4 = xml;
    int i = 0, k = 0, len = 0;
    BOOL found = FALSE;
    len = wcslen(xml);     
    
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
                ret = new wchar_t[(p1)-(p2)];
                wcsncpy(ret, p2+1, (p1)-(p2+1));
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
int XMLProcessor::countAnd(wchar_t* token) {
    return countChar(token, TEXT("&"));
}

int XMLProcessor::countChar(wchar_t* token, wchar_t* element) {

    wchar_t* p1, *p2;
    p1 = p2 = token;
    int i = 0, k = 0, len = 0;

    while (wcsstr(p1, element) != NULL) {
        len = wcslen(p2);        
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
* getElementContentExcept(xmlPtr, TEXT("Add"), TEXT("Sync&Atomic"), &post)
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

wchar_t* XMLProcessor::getElementContentExcept(wchar_t*      xmlPtr    ,
                                               wchar_t*      tag       ,
                                               wchar_t*      except    ,
                                               unsigned int* post) {
    
    wchar_t*  ret    = NULL;
    wchar_t*  found  = NULL;
    wchar_t*  xml    = NULL;
    wchar_t** array = NULL;
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
    
    array = new wchar_t*[count + 1];
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

    wchar_t* internal = stringdup(except);
    wchar_t* p1, *p2;
    p1 = p2 = internal;
    int i = 0, k = 0, len = 0;

    while (wcsstr(p2, TEXT("&")) != NULL) {
        len = wcslen(p2);        
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
            wchar_t* tmp = stringdup(array[0]);
    
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
*    wchar_t* p = NULL;
*    unsigned int pos = 0, previous = 0;
*    int startLevel = -1;
*    while ((p = XMLProcessor::getElementContentLevel(&xml[pos], TEXT("Add"), 0, &startLevel, &pos)) != NULL) {        
*        pos += previous;
*        previous = pos;                
*    }
*
*/

wchar_t* XMLProcessor::getElementContentLevel(wchar_t*      xml   ,
                                              wchar_t*      tag   ,                                              
                                              unsigned int* pos, 
                                              int           lev ,   
                                              int*          startLevel)  {
    
    wchar_t* p1       = NULL;
    wchar_t* p2       = NULL;
    wchar_t* ret      = NULL;
    BOOL openBracket  = FALSE;  // <
    BOOL closeBracket = FALSE;  // >
    BOOL aloneBracket = FALSE;  // </
    BOOL preCloseBracket = FALSE;  //<.../
    BOOL openTag      = FALSE;
    BOOL closeTag     = FALSE;

    wchar_t tagNameFound[40];
    
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
   
    xmlLength = wcslen(xml);
    l = wcslen(tag);

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
            wcsncpy(tagNameFound, p2 + 1, n);         
            tagNameFound[n] = 0;
            if (wcscmp(tagNameFound, tag) == 0 && (level == lev)) {
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

StringBuffer XMLProcessor::makeElement(const wchar_t* tag, const wchar_t* val, ArrayList* attrList)
{
    StringBuffer s;
    
    if (attrList == NULL)
    {
        if (!val)
            return s;
        if (!val[0])
            return s;
        
        size_t len = wcslen(tag);
        wchar_t* t1 = new wchar_t[len + 4]; // <  >  0
        wchar_t* t2 = new wchar_t[len + 6]; // </ > \n 0

        if(!t1 || !t2){
            fprintf(stderr, "Memory error.\n");
            exit(1);
        }
        wsprintf(t1, TEXT("<%s>"), tag);
        wsprintf(t2, TEXT("</%s>\n"), tag);

        s = t1; s += val; s += t2;

        delete [] t1;
        delete [] t2;
    }
    else
    {
        size_t tagLen = wcslen(tag);
        wchar_t* t1 = new wchar_t[tagLen + 4]; // <  >  0, whitout closing >
        wchar_t* t2 = new wchar_t[tagLen + 6]; // </ > \n 0
        wsprintf(t1, TEXT("<%s"), tag);
        wsprintf(t2, TEXT("</%s>\n"), tag);
        s = t1;
        for (int i = 0; i < attrList->size(); i++)
        {
            KeyValuePair* item = (KeyValuePair*)attrList->get(i);
            wchar_t* attr = item->getKey();
            wchar_t* value = item->getValue();
            s += TEXT(" ");
            s += attr; 
            s += TEXT("=\"");
            s += value; 
            s += TEXT("\"");
        }
        s += TEXT(">"); s+= val; s+= t2;
        delete [] t1;
        delete [] t2;
        
    }
    return s;    
}


wchar_t* XMLProcessor::getElementAttributes(const wchar_t* xml,
                                            const wchar_t* tag,                                            
                                            unsigned int*  startPos,
                                            unsigned int*  endPos  ) {
        
    wchar_t* p1       = NULL;
    wchar_t* p2       = NULL;
    BOOL charFound    = FALSE;
    unsigned int xmlLength = (unsigned int)-1;
    unsigned int l = (unsigned int)-1;

    // exemple ot tag with attribute list
    // <body enc="base64">
    wchar_t *openTag = 0; //<tag
    
    if (xml == NULL) {
        goto finally;
    }

    xmlLength = wcslen(xml);
    l = wcslen(tag);


    if(wcscmp(tag, TEXT("CDATA")) == 0) {
        goto finally;
    }
    else {
        openTag = new wchar_t[l+10];
        wsprintf(openTag, TEXT("<%s>"), tag);        
    }

    p1 = wcsstr((wchar_t*)xml, openTag);

    if (p1 == NULL) { // tag can have attributes or can be empty

        // if p1 is null I try to discover the next '>' char to close the tag. If does not exist 
        // return NULL
        
        // try to find "<tagName/>". If found it return null.
        wsprintf(openTag, TEXT("<%s/>"), tag);
        p1 = wcsstr((wchar_t*)xml, openTag);
        // ok, found an empty tag
        if (p1 != NULL) {
            goto finally;
        }

        // try to find "<tagName"
        wsprintf(openTag, TEXT("<%s"), tag);
        p1 = wcsstr((wchar_t*)xml, openTag);

        if (p1 == NULL) {
            goto finally;
        }
        
        p1 = p1 + l + 1;   // <body_
        p2 = p1 + 1;
        for (unsigned int k = 0; k < xmlLength; k++) { // Suppose max length as the xml string
            p2 = p2 + 1;
            if (*p2 == 0) {
                goto finally;
            }
            else if (*p2 == '>') {
                charFound = TRUE;                
                break;
            }            
        }
        if (!charFound)
            goto finally;
    } else {  // tag doesn't have attribute.    
        p1 = NULL;
        *startPos = 0;
        *endPos = 0;
        goto finally;    
    }

    if (*p1 == 0) {
        //
        // This is abc<tag>\0
        //
        goto finally;
    }
    
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

StringBuffer XMLProcessor::makeElement(const wchar_t* tag, 
                                    const wchar_t* val,
                                    const wchar_t* attr) {


int i = 0;


return NULL;

}
