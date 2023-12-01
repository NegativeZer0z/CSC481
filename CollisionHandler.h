#pragma once

#include "Entity.h"
#include "Player.h"
#include "EventHandler.h"
#include <memory>

class CollisionHandler : public EventHandler {

    public:
        bool onEvent(Event e);

        std::shared_ptr<Player> player;

        std::shared_ptr<Entity> entity;
        
};