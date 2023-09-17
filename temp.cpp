#include <SFML/Graphics.hpp>

class movingPlatform : public sf::RectangleShape {
    private:
        //the speed the platform travels at
        float speed;
        
        //the direction the platform travels
        sf::Vector2f direction;

        //the initial position of the created platform
        sf::Vector2f initPosition;

        //how much the platform can move from its initial position in the x axis
        float xBound;

        //how much the platform can move from its initial position in the y axis
        float yBound;

        //function to check if the platform is out of bounds or not
        bool checkBoundaries() {
            if(getPosition().x < initPosition.x - xBound || getPosition().x > initPosition.x + xBound ||
                getPosition().y < initPosition.y - yBound || getPosition().y > initPosition.y + yBound) {
                return true;
            }
            return false;
        }

    public:
        //Constructor for a moving platform that takes a direction, size, start position, and boundaries for the x and y
        movingPlatform(const sf::Vector2f& dir, const sf::Vector2f& size, const sf::Vector2f& position, float platformSpeed, float x, float y)
            : speed(platformSpeed), direction(dir), xBound(x), yBound(y), initPosition(position)
        {
            setSize(size);
            setPosition(position);
            setFillColor(sf::Color::Blue);
            setDirection(dir);
        }

        //Updates the position of the platform by moving it and checking the boundaries of the platform
        void update() {
            sf::Vector2f movement = direction * speed; //calculate the movement for the platform
            move(movement);

            //check to see if we need to reverse direction
            if(checkBoundaries()) {
                direction = -direction;
            }
        }

        //sets the direction of the platform
        void setDirection(const sf::Vector2f& newDirection) {
            direction = newDirection;
        }
};
