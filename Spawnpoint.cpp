#include "Spawnpoint.h"

Spawnpoint::Spawnpoint(sf::Vector2f pos, sf::Vector2f size) : Entity(pos, size) {
    setPosition(pos);
    setSize(size);
}

//spawn the player by setting its position to the spawnpoint position
void Spawnpoint::spawn(std::shared_ptr<Player> player) {
    player->setSpritePosition(position.x, position.y);
    player->setState(false);
}
