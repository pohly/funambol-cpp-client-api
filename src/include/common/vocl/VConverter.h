#ifndef INCL_VIRTUAL_CONVERTER
#define INCL_VIRTUAL_CONVERTER

#include "VObject.h"

class VConverter{

public:
    static VObject* parse(wchar_t* buffer);

private:
    static VProperty* readFieldHeader(wchar_t* buffer);
    static bool readFieldBody(wchar_t* buffer, VProperty* property);
    static wchar_t* extractObjectType(wchar_t* buffer);
    static wchar_t* extractObjectVersion(wchar_t* buffer);
    static bool extractGroup(wchar_t* propertyName, wchar_t* propertyGroup);
    
};
#endif
