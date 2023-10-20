#include "Boundary.h"

Boundary::Boundary(sf::Vector2f size, sf::Vector2f position) : Entity(size, position) {
    setFillColor(sf::Color::Red);
    setPosition(position);
    setSize(size);
    leftBound = 512.f;
    rightBound = 1024.f;
}

bool Boundary::shift(std::shared_ptr<Player> player, sf::RenderWindow& window, sf::View& view) {
    sf::FloatRect boundary = getGlobalBounds();
    sf::FloatRect playerBounds = player->getGlobalBounds();

    if(playerBounds.intersects(boundary)) {

        //right collision
        if(boundary.left < playerBounds.left 
            && boundary.left + boundary.width < playerBounds.left + playerBounds.width
            && boundary.top < playerBounds.top + playerBounds.height
            && boundary.top + boundary.height > playerBounds.top) 
        {
            view.setCenter(rightBound, window.getSize().y / 2);
            window.setView(view);
        }
        //left collision
        else if(boundary.left > playerBounds.left 
            && boundary.left + boundary.width > playerBounds.left + playerBounds.width
            && boundary.top < playerBounds.top + playerBounds.height
            && boundary.top + boundary.height > playerBounds.top) 
        {
            view.setCenter(leftBound, window.getSize().y / 2);
            window.setView(view);
        }

        return true;
    }
    return false;
}
