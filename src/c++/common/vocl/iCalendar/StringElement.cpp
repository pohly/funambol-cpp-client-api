#include "StringElement.h"

StringElement::StringElement(wchar_t* v) {
    value = (v) ? stringdup(v) : NULL;
}

StringElement::~StringElement() {
    if (value) {
        delete [] value; value = NULL;
    }
}

void StringElement::setValue(wchar_t* v) {
    if (value) {
        delete [] value;
    }
    value = (v) ? stringdup(v) : NULL;
}

wchar_t* StringElement::getValue(wchar_t* buf, int size) {

    if (buf == NULL) {
        return value;
    }

    if (size >= 0) {
        wcsncpy(buf, value, size);
    } 
    else {
        wcscpy(buf, value);
    }

    return buf;
}

ArrayElement* StringElement::clone() {
    
    StringElement* clone = new StringElement(value);
    return (ArrayElement*)clone;

    return NULL;
}