#include "GeneralPlatform.h"

GeneralPlatform::GeneralPlatform(sf::Vector2f position, sf::Vector2f size) : Entity(position, size) {
    this->position = position;
    this->size = size;
    setPosition(position);
    setSize(size);
    setFillColor(sf::Color::Blue);
}