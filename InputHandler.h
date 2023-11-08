#pragma once

#include "EventHandler.h"
#include <memory>
#include "Player.h"

class InputHandler : public EventHandler {

    public:
        std::shared_ptr<Player> player;

        float deltaTime;

        bool onEvent(Event e);
};