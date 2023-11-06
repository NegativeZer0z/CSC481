#pragma once

#include "EventHandler.h"
#include "Player.h"
#include <memory>

class DeathEvent : public EventHandler {

    public:
        std::shared_ptr<Player> player;

        void onEvent(Event e);
};