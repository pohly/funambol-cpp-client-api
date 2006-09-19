#ifndef INCL_STRING_ELEMENT
#define INCL_STRING_ELEMENT

#include "ArrayElement.h"
#include <string.h>

class StringElement : public ArrayElement {

private:
    WCHAR* value;

public:
    StringElement(WCHAR* v = NULL);
    ~StringElement();
    ArrayElement* clone();
    void setValue (WCHAR* value);
    WCHAR* getValue(WCHAR* buf = NULL, int size = -1);
};
#endif