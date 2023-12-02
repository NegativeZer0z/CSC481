#include "StaticPlatform.h"

StaticPlatform::StaticPlatform(sf::Vector2f position, sf::Vector2f size) : GeneralPlatform(position, size) {
    setFillColor(sf::Color::Green);
}