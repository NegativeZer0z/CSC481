#include "MovingPlatform.h"

MovingPlatform::MovingPlatform(sf::Vector2f position, sf::Vector2f size, sf::Vector2f direction, float speed, float xBound, float yBound)
                                : GeneralPlatform(position, size), speed(speed), xBound(xBound), yBound(yBound), direction(direction), initPosition(position) 
{
    setFillColor(sf::Color::Cyan);
}

bool MovingPlatform::checkBoundaries() {
    if(sprite.getPosition().x < initPosition.x - xBound || sprite.getPosition().x > initPosition.x + xBound
        || sprite.getPosition().y < initPosition.y - yBound || sprite.getPosition().y > initPosition.y + yBound) 
    {
        direction = -direction;
        return true;
    }
    return false;
}

void MovingPlatform::update(float deltaTime) {
    velocity = direction * speed * deltaTime;
    moveSprite(velocity);
}

bool MovingPlatform::checkCollision(Entity& entity) {
    sf::FloatRect platformBounds = sprite.getGlobalBounds();
    sf::FloatRect entityBounds = entity.getGlobalBounds();

    //only need to check for the top and if there is collision move the entity by current platform velocity
    if(entityBounds.intersects(platformBounds)) {
        if(platformBounds.top > entityBounds.top 
            && platformBounds.top + platformBounds.height > entityBounds.top + entityBounds.height
            && platformBounds.left < entityBounds.left + entityBounds.width
            && platformBounds.left + platformBounds.width > entityBounds.left) 
        {
            entity.moveSprite(velocity);
            return true;
        }
    }
    return false;
}