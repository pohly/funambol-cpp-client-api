#ifndef INCL_VIRTUAL_CONVERTER
#define INCL_VIRTUAL_CONVERTER

#include "VObject.h"

class VConverter{

public:
    static VObject* parse(WCHAR* buffer);

private:
    static VProperty* readFieldHeader(WCHAR* buffer);
    static bool readFieldBody(WCHAR* buffer, VProperty* property);

    // Extract the parameter of certain properties, e.g. "BEGIN:" or "VERSION:".
    // The result is a pointer into buffCopy, which is expected to have
    // buffCopyLen wchars and will be reallocated if necessary.
    static WCHAR* extractObjectProperty(WCHAR* buffer, const WCHAR *property,
                                          WCHAR * &buffCopy, size_t &buffCopyLen);

    // extractObjectType() and extractObjectVersion() contain static buffers,
    // copy the result before calling these functions again!
    static WCHAR* extractObjectType(WCHAR* buffer);
    static WCHAR* extractObjectVersion(WCHAR* buffer);
    static bool extractGroup(WCHAR* propertyName, WCHAR* propertyGroup);
    
};
#endif
