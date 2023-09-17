#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>

//window dimensions
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

//player physics
#define GRAVITY 9.18f
#define MAX_VERTICAL_VELOCITY 8.f
#define PLAYER_SPEED 120.f
#define JUMP_HEIGHT 280.f

class Entity : public sf::RectangleShape {

    protected:
        sf::Vector2f size; //the size of the entity

        sf::Vector2f position; //the initial position of the entity

        sf::Texture texture; //texture for the rectangle
        
        sf::Sprite sprite; //sprite that is displayed

    public:
        Entity(sf::Vector2f position, sf::Vector2f size); //constructor

        void render(sf::RenderWindow& window); //how the entity is rendered/drawn, overwritten by each entity

        void moveSprite(sf::Vector2f velocity); //move both the sprite and rectangle

        void initTexture(std::string path); //initialize the texture and sprite
};