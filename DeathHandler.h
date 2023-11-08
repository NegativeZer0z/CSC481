#pragma once

#include "EventHandler.h"
#include "Player.h"
#include <memory>

class DeathHandler : public EventHandler {

    public:
        std::shared_ptr<Player> player;

        bool onEvent(Event e);
};