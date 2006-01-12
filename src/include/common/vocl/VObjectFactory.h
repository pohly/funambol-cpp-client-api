#ifndef INCL_VIRTUAL_OBJECT_FACTORY
#define INCL_VIRTUAL_OBJECT_FACTORY

#include "VObject.h"

class VObjectFactory {

public:
    static VObject* createInstance(wchar_t* objType, wchar_t* objversion);
};

#endif
