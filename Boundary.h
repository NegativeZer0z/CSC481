#pragma once

#include "Entity.h"
#include "Player.h"
#include <memory>

class Boundary : public Entity {

    private:
        int leftBound;

        int rightBound;

    public:
        Boundary(sf::Vector2f size, sf::Vector2f position); //constructor

        bool shift(std::shared_ptr<Player> player, sf::RenderWindow& window, sf::View& view); //shift the view of the window if there is collisions with the boundary
};