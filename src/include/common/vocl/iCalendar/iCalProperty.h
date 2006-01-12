/**
 * Copyright (C) 2005-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef INCL_ICALENDAR_PROPERTY
#define INCL_ICALENDAR_PROPERTY

#include "base/fscapi.h"
#include "base/util/ArrayList.h"

class iCalProperty : public ArrayElement {

private:
    wchar_t* altre;         //Alternate text representation
    wchar_t* cn;            //Common name
    wchar_t* cutype;        //Calendar user type
    wchar_t* delegatedFrom; //Delegator
    wchar_t* delegatedTo;   //Delegatee
    wchar_t* dir;           //Directory entry
    wchar_t* encoding;      //Inline encoding
    wchar_t* formattype;    //Format type
    wchar_t* fbtype;        //free-busy type
    wchar_t* language;      //Language for text
    wchar_t* member;        //Group or list membership
    wchar_t* partstat;      //Participation status
    wchar_t* range;         //Recurrence identifier range
    wchar_t* trigrel;       //Alarm trigger relationship
    wchar_t* related;       //Relationship type
    wchar_t* role;          //Participation role
    wchar_t* rsvp;          //RSVP expectation
    wchar_t* sentby;        //Sent by
    wchar_t* tzid;          //Reference to time zone object
    wchar_t* valuetype;     //Property value data type
    wchar_t* value;         //the value of property  
    ArrayList* xParams;

    void set(wchar_t** property, wchar_t* v);

public:
    iCalProperty (wchar_t* v = NULL);
    ~iCalProperty();

    // ---------------------------------------------------------- Public methods

    wchar_t* getAltre (wchar_t* buf = NULL, int size = -1);
    wchar_t* getCn (wchar_t* buf = NULL, int size = -1);
    wchar_t* getCutype (wchar_t* buf = NULL, int size = -1);
    wchar_t* getDelegatedFrom (wchar_t* buf = NULL, int size = -1);
    wchar_t* getDelegatedTo (wchar_t* buf = NULL, int size = -1);
    wchar_t* getDir (wchar_t* buf = NULL, int size = -1);
    wchar_t* getEncoding (wchar_t* buf = NULL, int size = -1);
    wchar_t* getFormatType (wchar_t* buf = NULL, int size = -1);
    wchar_t* getFbType (wchar_t* buf = NULL, int size = -1);
    wchar_t* getLanguage (wchar_t* buf = NULL, int size = -1);
    wchar_t* getMember (wchar_t* buf = NULL, int size = -1);
    wchar_t* getPartStat (wchar_t* buf = NULL, int size = -1);
    wchar_t* getRange (wchar_t* buf = NULL, int size = -1);
    wchar_t* getTrigRel (wchar_t* buf = NULL, int size = -1);
    wchar_t* getRelated (wchar_t* buf = NULL, int size = -1);
    wchar_t* getRole (wchar_t* buf = NULL, int size = -1);
    wchar_t* getRsvp (wchar_t* buf = NULL, int size = -1);
    wchar_t* getSentBy (wchar_t* buf = NULL, int size = -1);
    wchar_t* getTzID (wchar_t* buf = NULL, int size = -1);
    wchar_t* getValueType (wchar_t* buf = NULL, int size = -1);
    wchar_t* getValue (wchar_t* buf = NULL, int size = -1);
    ArrayList* getXParam();

    void setAltre (wchar_t* v);
    void setCn (wchar_t* v);
    void setCutype (wchar_t* v);
    void setDelegatedFrom (wchar_t* v);
    void setDelegatedTo (wchar_t* v);
    void setDir (wchar_t* v);
    void setEncoding (wchar_t* v);
    void setFormatType (wchar_t* v);
    void setFbType (wchar_t* v);
    void setLanguage (wchar_t* v);
    void setMember (wchar_t* v);
    void setPartStat (wchar_t* v);
    void setRange (wchar_t* v);
    void setTrigRel (wchar_t* v);
    void setRelated (wchar_t* v);
    void setRole (wchar_t* v);
    void setRsvp (wchar_t* v);
    void setSentBy (wchar_t* v);
    void setTzID (wchar_t* v);
    void setValueType (wchar_t* v);
    void setValue (wchar_t* v);
    void setXParam(ArrayList& list);

    ArrayElement* clone();
};

#endif
