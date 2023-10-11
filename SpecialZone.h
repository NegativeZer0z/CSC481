#pragma once
#include "Entity.h"
#include "Player.h"
#include <memory>

//class for death zone and end zone
class SpecialZone : public Entity {

    private:
        bool death; //a death zone if the type passed in the constructor is 0

        bool goal; //a death zone if the type passed in the constructor is 1

    public:
        SpecialZone(sf::Vector2f pos, sf::Vector2f size, int type); //constructor

        bool checkCollision(std::shared_ptr<Player> player);
};