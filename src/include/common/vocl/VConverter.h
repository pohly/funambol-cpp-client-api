#ifndef INCL_VIRTUAL_CONVERTER
#define INCL_VIRTUAL_CONVERTER

#include "VObject.h"

class VConverter{

public:
    static VObject* parse(wchar_t* buffer);

private:
    static VProperty* readFieldHeader(wchar_t* buffer);
    static bool readFieldBody(wchar_t* buffer, VProperty* property);

    // Extract the parameter of certain properties, e.g. "BEGIN:" or "VERSION:".
    // The result is a pointer into buffCopy, which is expected to have
    // buffCopyLen wchars and will be reallocated if necessary.
    static wchar_t* extractObjectProperty(wchar_t* buffer, const wchar_t *property,
                                          wchar_t * &buffCopy, size_t &buffCopyLen);

    // extractObjectType() and extractObjectVersion() contain static buffers,
    // copy the result before calling these functions again!
    static wchar_t* extractObjectType(wchar_t* buffer);
    static wchar_t* extractObjectVersion(wchar_t* buffer);
    static bool extractGroup(wchar_t* propertyName, wchar_t* propertyGroup);
    
};
#endif
