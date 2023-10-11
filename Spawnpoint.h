#pragma once
#include "Entity.h"
#include "Player.h"
#include <memory>

class Spawnpoint : public Entity {
    
    public:
        Spawnpoint(sf::Vector2f pos, sf::Vector2f size); //constructor

        void spawn(std::shared_ptr<Player> player); //respawn the player at this spawnpoint
};