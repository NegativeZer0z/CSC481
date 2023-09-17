#pragma once

#include "GeneralPlatform.h"

class MovingPlatform : public GeneralPlatform {

    private:
        float speed;    // the speed the platform travels at
    
        sf::Vector2f direction; // the direction the platform travels
    
        sf::Vector2f initPosition;  // the initial position of the created platform
    
        float xBound;   // how much the platform can move from its initial position in the x-axis
    
        float yBound;   // how much the platform can move from its initial position in the y-axis
    
        sf::Vector2f velocity;  //velocity of the platform
    
    public:
        //constructor
        MovingPlatform(sf::Vector2f position, sf::Vector2f size, sf::Vector2f direction, float speed, float xBound, float yBound);
    
        //update the platform velocity
        void update(float deltaTime);
    
        //check to see if we are within the boundaries
        bool checkBoundaries();

        //collision for the moving platform, carry things when on top of it
        bool checkCollision(Entity& entity);
};
