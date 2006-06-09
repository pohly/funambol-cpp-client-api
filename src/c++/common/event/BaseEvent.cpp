
#include "event/BaseEvent.h"


BaseEvent::BaseEvent(int type, unsigned long date) {

	this->type = type;
	this->date = date;
}

BaseEvent::~BaseEvent() {}

int BaseEvent::getType() {

	return type;
}

unsigned long BaseEvent::getDate() {
	
	return date;
}
