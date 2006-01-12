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
 
 
#include "syncml/core/Get.h"
 

Get::Get() {
    lang = NULL;
    COMMAND_NAME = new wchar_t[wcslen(GET_COMMAND_NAME) + 1];
    wsprintf(COMMAND_NAME, GET_COMMAND_NAME);
}

Get::~Get() {
    if (lang) {
        delete [] lang; lang = NULL;
    }
    if (COMMAND_NAME) {
        delete [] COMMAND_NAME; COMMAND_NAME = NULL;
    }
}
/** 
* Creates a new Get object with the given command identifier, 
* noResponse, language, credential, meta and an array of item
*
* @param cmdID the command identifier - NOT NULL
* @param noResp true if no response is required
* @param lang the preferred language for results data
* @param cred the authentication credential
* @param meta the meta information
* @param items the array of item - NOT NULL
*
*/
Get::Get(CmdID* cmdID,
         BOOL noResp,
         wchar_t* lang,
         Cred* cred,
         Meta* meta,
         ArrayList* items) : ItemizedCommand(cmdID, meta, items) {
    
    this->lang = NULL;
    COMMAND_NAME = new wchar_t[wcslen(GET_COMMAND_NAME) + 1];
    wsprintf(COMMAND_NAME, GET_COMMAND_NAME);

    setCred(cred);
    setNoResp(noResp);   
    setLang(lang);    
}


/**
* Returns the preferred language
*
* @return the preferred language
*
*/
wchar_t* Get::getLang(wchar_t* retLang) {
    if (retLang == NULL) {
        return lang;
    }
    return wcscpy(retLang, lang);
}

/**
* Sets the preferred language
*
* @param lang new preferred language
*/
void Get::setLang(wchar_t* lang) {
    if (this->lang) {
        delete [] this->lang; this->lang = NULL;
    }
    this->lang = stringdup(lang);
}

/**
* Gets the command name property
*
* @return the command name property
*/   
wchar_t* Get::getName() {
    return COMMAND_NAME;
}

ArrayElement* Get::clone() {
    Get* ret = new Get(getCmdID(), getNoResp(), lang, getCred(), getMeta(), getItems());
    return ret;
}
