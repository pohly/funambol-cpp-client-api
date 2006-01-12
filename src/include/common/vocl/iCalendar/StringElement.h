#ifndef INCL_STRING_ELEMENT
#define INCL_STRING_ELEMENT

#include "ArrayElement.h"
#include <string.h>

class StringElement : public ArrayElement {

private:
    wchar_t* value;

public:
    StringElement(wchar_t* v = NULL);
    ~StringElement();
    ArrayElement* clone();
    void setValue (wchar_t* value);
    wchar_t* getValue(wchar_t* buf = NULL, int size = -1);
};
#endif