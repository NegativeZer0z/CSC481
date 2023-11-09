#include "CollisionHandler.h"

bool CollisionHandler::onEvent(Event e) {
    sf::FloatRect playerBounds = player->getGlobalBounds();
    sf::FloatRect entityBounds = entity->getGlobalBounds();
    sf::Vector2f vel = player->getVelocity();
    
    if(e.getEventType() == "botCollision") {
        vel.y = 0.f;
        player->setVelocity(vel.x, vel.y);
        player->setSpritePosition(playerBounds.left, entityBounds.top - playerBounds.height);
        return true;
    }
    else if(e.getEventType() == "topCollision") {
        vel.y = 0.f;
        player->setVelocity(vel.x, vel.y);
        player->setSpritePosition(playerBounds.left, entityBounds.top + playerBounds.height);
        return true;
    }
    else if(e.getEventType() == "rightCollision") {
        vel.x = 0.f;
        player->setVelocity(vel.x, vel.y);
        player->setSpritePosition(entityBounds.left - playerBounds.width, playerBounds.top);
        return true;
    } 
    else if(e.getEventType() == "leftCollision") {
        vel.x = 0.f;
        player->setVelocity(vel.x, vel.y);
        player->setSpritePosition(entityBounds.left + playerBounds.width, playerBounds.top);
        return true;
    }
    return false;
}