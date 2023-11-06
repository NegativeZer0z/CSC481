#pragma once

#include "EventHandler.h"
#include "Player.h"
#include <memory>

class InputEvent : public EventHandler {


    public:
        void onEvent(Event e);

        int direction;

        std::shared_ptr<Player> player;
};