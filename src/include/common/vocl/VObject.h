#ifndef INCL_VIRTUAL_OBJECT
#define INCL_VIRTUAL_OBJECT

#include "VProperty.h"



/*
 * ************************************* Class VObject *********************************************
 * *************************************************************************************************
 * A VObject object rapresents an item that can be a vCard, a vCalendar or vTodo.
 * A VObject contains an array of VProperty, each one rapresents a property.
 * A property is the definition of an individual attribute describing the vCard/vCal/vTodo.
 * Each property has a name, an array of parameters and an array of values,
 * for example, the property:
 *
 *    TEL;HOME:+1-919-555-1234
 *
 * has name 'TEL', one parameter 'HOME' and one value '+1-919-555-1234'
 *
 * Some properties have more than one parameter, each parameter is a pair: (name, value).
 * Use VProperty::add/getParameter() methods to access them.
 *
 * Some properties have more than one value, each value is a string of text.
 * Values MUST be inserted in the right order (as vCard specific).
 * Use VProperty::add/getValue() methods to access them.
 * Note:
 *       If one of these value is not present, it must be inserted however as
 *       an empty string, to make sure the right order is respected.
 *
 *
 * Properties with more than one value are:
 *
 * Property         name     1^value     2^value        3^value         4^value           5^value
 * -------------------------------------------------------------------------------------------------
 * Name         ->  N:      <LastName>  <FirstName>    <MiddleName>    <Title>           <Suffix>
 * Address      ->  ADR:    <Street>    <City>         <State>         <PostalCode>      <Country>
 * Audio Alarm  ->  AALARM: <RunTime>   <Snooze Time>  <Repeat Count>  <Audio Content>
 *
 *
 * Property values should be added as simple text, Quoted-Printable encoding is 
 * automatically managed inside VObject. Also the escaping of special chars (";" "\") is
 * automatically managed inside VObject.
 * Use VObject::toString() method to obtain the correspondent vCard/vCal/vTodo.
 *
 */


class VObject {

private:

    wchar_t* version;
    wchar_t* productID;
    ArrayList* properties;
    void set(wchar_t**, wchar_t*);

public:       

    VObject();
    ~VObject();
    void setVersion (wchar_t* ver);
    void setProdID (wchar_t* prodID);
    wchar_t* getVersion();
    wchar_t* getProdID();
    void addProperty(VProperty* property);
    void addFirstProperty(VProperty* property);
    void insertProperty(VProperty* property);
    bool removeProperty(int index);
    void removeProperty(wchar_t* propName);
    void removeAllProperies(wchar_t* propName);
    //removes all properties having name - propName;
    bool containsProperty(wchar_t* propName);
    int propertiesCount();
    VProperty* getProperty(int index);
    VProperty* getProperty(wchar_t* propName);
    wchar_t* toString();

    // Patrick Ohly:
    //
    // Normally the class does not change the encoding
    // of properties. That means that users of this class
    // have to be prepared to handle e.g. quoted-printable
    // encoding of non-ASCII characters or \n as line break
    // in vCard 3.0.
    //
    // This function decodes all property strings into the
    // native format. It supports:
    // - decoding quoted-printable characters if
    //   ENCODING=QUOTED-PRINTABLE
    // - replacement of \n with a line break and \, with
    //   single comma if version is 3.0
    //
    // It does not support charset conversions, so everything
    // has to be UTF-8 to work correctly.
    //
    // This function does not modify the property parameters,
    // so one could convert back into the original format.
    // 
    // Consider this function a hack: at this time it is not
    // clear how the class is supposed to handle different
    // encodings, but I needed a solution, so here it is.
    // 
    void toNativeEncoding();

    //
    // Converts properties in native format according to
    // their parameters.
    //
    // It only supports:
    // - replacement of line breaks and commas (if 3.0)
    //
    // It does not support any charsets or encoding.
    // ENCODING=QUOTED-PRINTABLE will be removed because
    // it no longer applies when toNativeEncoding() was
    // used before, but the other parameters are preserved:
    // this might be good enough to recreate the original
    // object.
    //
    void fromNativeEncoding();
};


#endif
