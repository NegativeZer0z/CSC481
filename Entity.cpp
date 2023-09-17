#include "Entity.h"

Entity::Entity(sf::Vector2f position, sf::Vector2f size) {
    this->position = position;
    this->size = size;
}

void Entity::render(sf::RenderWindow &window) {
    window.draw(sprite);
}

void Entity::moveSprite(sf::Vector2f velocity) {
    sprite.move(velocity);
    move(velocity);
}

void Entity::setSpritePosition(float x, float y) {
    sprite.setPosition(x, y);
    setPosition(x, y);
}

void Entity::initTexture(std::string path) {
    if(!texture.loadFromFile(path, sf::IntRect(position.x, position.y, size.x, size.y))) {
        std::cout << "Can't load texture" << path << "\n";
    }
    sprite.setPosition(position);
    sprite.setTexture(texture);
}