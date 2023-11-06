#pragma once

#include "Event.h"

class EventHandler {

    public:
        virtual void onEvent(Event e) = 0;
};