#include "Event.h"

Event::Event(std::string e, float t) {
    this->type = e;
    this->time = t;
}

std::string Event::getEventType() {
    return type;
}

float Event::getTime() const {
    return time;
}