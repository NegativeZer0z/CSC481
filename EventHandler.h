#pragma once

#include "Event.h"

class EventHandler {

    public:
        virtual bool onEvent(Event e) = 0;
};