#pragma once

#include "Entity.h"
#include "Player.h"
#include "EventHandler.h"
#include <memory>

class CollisionHandler : public EventHandler {

    public:
        void onEvent(Event e);

        std::shared_ptr<Player> player;

        Entity *entity;
};