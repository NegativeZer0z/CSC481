#pragma once

#include "EventHandler.h"
#include <memory>
#include "Player.h"
#include "Spawnpoint.h"
#include "SFML/Graphics.hpp"

class SpawnHandler : public EventHandler {

    public:
        std::shared_ptr<Player> player;

        Spawnpoint *sp;

        bool onEvent(Event e);
};