#ifndef INCL_ICALENDAR_CALENDAR
#define INCL_ICALENDAR_CALENDAR

#define ICALENDAR_BUFFER 30000

#include "vocl/VProperty.h"
#include "vocl/iCalendar/iCalProperty.h"
#include "vocl/iCalendar/Event.h"
#include "vocl/iCalendar/ToDo.h"

class Calendar
{
private:        
    iCalProperty* prodID;
    iCalProperty* version;
    iCalProperty* calScale;
    iCalProperty* method;
    ArrayList* xTags;
    ArrayList* events;
    ArrayList* todos;
    void set(iCalProperty** oldProperty, iCalProperty& newProperty);
    iCalProperty* getiCalPropertyFromVProperty(VProperty* vp);
    VProperty* getVPropertyFromiCalProperty(wchar_t* name, iCalProperty* prop);

public:
    Calendar();
    ~Calendar();
    void setProdID(iCalProperty& p);
    void setVersion(iCalProperty& p);
    void setCalScale(iCalProperty& p);
    void setMethod(iCalProperty& p);
    void setXTags(ArrayList& list);
    void setEvents(ArrayList& list);
    void setToDos(ArrayList& list);
    void addEvent(Event* ev);
    void addToDo(ToDo* task);
    iCalProperty* getProdID();
    iCalProperty* getVersion();
    iCalProperty* getCalScale();
    iCalProperty* getMethod();
    ArrayList* getXTags();
    ArrayList* getEvents();
    ArrayList* getToDos();
    wchar_t* toString();

    ArrayElement* clone();
};

#endif
