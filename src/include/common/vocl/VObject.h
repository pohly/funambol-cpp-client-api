#ifndef INCL_VIRTUAL_OBJECT
#define INCL_VIRTUAL_OBJECT

#include "VProperty.h"

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
