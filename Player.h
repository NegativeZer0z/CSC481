#pragma once

#include "Entity.h"
#define MAGE_START_OFFSET 12 //offset to find the starting point for the texture for mage.png
#define MAGE_LEFT_OFFSET 36 //offset to make the sprite smaller to match art for mage.png
#define MAGE_BOT_OFFSET 2 //offset to make the bottom pixel line not appear for mage.png

//player physics 
#define GRAVITY 740.18f
#define MAX_VERTICAL_VELOCITY 8.f
#define PLAYER_SPEED 3600.f
#define JUMP_HEIGHT 16580.f
#define SCREEN_SPEED 20.f

class Player : public Entity {
    
    private:
        bool jump; //can the player jump

        sf::Vector2f velocity; //velocity of the player

        //all of these variables are here because of an visual bug with the art so I have to calculate and adjust the size of the texture applied
        int textureWidth; //width of the texture

        int textureHeight; //height of the texture

        sf::Vector2i leftText; //coords for the specific texture for the left model of the player

        sf::Vector2i rightText; //coords for the specific texture for the right model of the player

        int botOffset; //offset from the bottom

        int leftOffset; //offset from the left

        int startOffset; //offset from the starting x coord

        bool dead; //bool to see if player has died

        float leftBound;

        float rightBound;

    public:
        Player(sf::Vector2f position, sf::Vector2f size); //constructor

        void update(float deltaTime, std::string input); //update the player position/velocity through inputs and handle gravity

        std::string updateEvent(); //check for inputs and return a string of the event type

        void applyGravity(float deltaTime); //apply gravity to player

        void updateTexture(int x, int y); //update the texture to the player

        void initTexture(std::string path, int width, int height, sf::Vector2i left, sf::Vector2i right, int leftOffset, int botOffset, int startOffset); //set the initial texture

        std::string checkCollision(Entity& entity); //check collision when moving to other entities

        void wallCollision(sf::RenderWindow& window, sf::View& view); //check collision with the borders of the window

        void setState(bool state); //set the state of the player(dead or not)

        bool checkState(); //get the state(dead or not) of the player

        void setVelocity(float x, float y); //set the player velocity

        sf::Vector2f getVelocity(); //get the currently player velocity
};