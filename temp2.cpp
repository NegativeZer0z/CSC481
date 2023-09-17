#include <SFML/Graphics.hpp>
#define DEFAULTSPEED 50.f

class character: public sf::CircleShape {
    private:
        sf::Vector2f velocity;

        sf::Vector2f currPosition;

        float gravity;

        bool jumping;

        float speed;
    
    public:
        //constructor for a player object
        character(const float radius, const sf::Vector2f position): velocity(sf::Vector2f(0.0f, 0.0f)), jumping(false), gravity(0.5f), speed(DEFAULTSPEED) {
            setRadius(radius);
            setPosition(position);
            setFillColor(sf::Color::Green);
        }

        //updates the state of the keyboard by checking the specific keys pressed
        void update(float deltaTime) {
            //move left
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                //move(speed * deltaTime, 0.f);
                velocity.x = 0;
                velocity.x -= speed * deltaTime;
                move(velocity);
            }

            //move right
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                //move(-speed * deltaTime, 0.f);
                velocity.x = 0;
                velocity.x += speed * deltaTime;
                move(velocity);
            }

            //jumping
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                if(!jumping) {
                    velocity.y += speed * deltaTime;
                    jumping = true;
                    move(velocity);
                }
            }
            else if(jumping) {
                while(velocity.y > 0.0f) {
                    velocity.y -= gravity;
                }
                velocity.y = 0;
                jumping = false;
            }
        }
};