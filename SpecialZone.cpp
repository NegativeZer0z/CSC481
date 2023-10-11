#include "SpecialZone.h"

SpecialZone::SpecialZone(sf::Vector2f pos, sf::Vector2f size, int type) : Entity(pos, size) {
    setPosition(pos);
    setSize(size);
    setFillColor(sf::Color::Green);
    if(type == 0) {
        death = true;
        goal = false;
    }
    else {
        death = false;
        goal = true;
    }
}

bool SpecialZone::checkCollision(std::shared_ptr<Player> player) {
    sf::FloatRect currBounds = sprite.getGlobalBounds();
    sf::FloatRect playerBounds = player->getGlobalBounds();

    if(playerBounds.intersects(currBounds)) {

        //bot collision
        if(currBounds.top < playerBounds.top 
            && currBounds.top + currBounds.height < playerBounds.top + playerBounds.height
            && currBounds.left < playerBounds.left + playerBounds.width
            && currBounds.left + currBounds.width > playerBounds.left) 
        {
            if(death) {
                player->setState(true);
            }
            return true;
        }
        //top collision
        else if(currBounds.top > playerBounds.top 
            && currBounds.top + currBounds.height > playerBounds.top + playerBounds.height
            && currBounds.left < playerBounds.left + playerBounds.width
            && currBounds.left + currBounds.width > playerBounds.left) 
        {
            if(death) {
                player->setState(true);
            }
            return true;
        }
        //right collision
        else if(currBounds.left < playerBounds.left 
            && currBounds.left + currBounds.width < playerBounds.left + playerBounds.width
            && currBounds.top < playerBounds.top + playerBounds.height
            && currBounds.top + currBounds.height > playerBounds.top) 
        {
            if(death) {
                player->setState(true);
            }
            return true;
        }
        //left collision
        else if(currBounds.left > playerBounds.left 
            && currBounds.left + currBounds.width > playerBounds.left + playerBounds.width
            && currBounds.top < playerBounds.top + playerBounds.height
            && currBounds.top + currBounds.height > playerBounds.top) 
        {
            if(death) {
                player->setState(true);
            }
            return true;
        }
    }
    return false;
}
