#include <SFML/Window.hpp>
#include "StaticPlatform.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "Thread.h"

void run_wrapper(Thread *fe, MovingPlatform *moving, Player *player, float deltaTime, std::vector<Entity>& list) {
    fe->runMovement(moving, player, deltaTime, list);
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Platformer", sf::Style::Default, settings);

    window.setFramerateLimit(60);

    //center the window
    window.setPosition(sf::Vector2i(230, 80));

    //creates a moving platform
    MovingPlatform moving(sf::Vector2f(770.f, 650.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(1.0f, 0.0f), 50.0f, 40.f, 0.f);

    //creats a static platform
    GeneralPlatform platform(sf::Vector2f(550.f, 700.f), sf::Vector2f(100.f, 15.f));

    //creates a player
    Player player(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f));

    //the base floor of the game
    StaticPlatform floor(sf::Vector2f(0.f, 750.f), sf::Vector2f(1024.f, 18.f));

    //for calculating how many seconds has passed
    sf::Clock clock;

    //both platforms and the floor uses grass.png as the texture in the textures folder
    //"29 grounds and walls (and water) (1024x1024) - Grass1.png" by Mysteryem licensed GPL 2.0, GPL 3.0, CC-BY-SA 3.0
    //https://opengameart.org/node/8054
    platform.initTexture("textures/grass.png");
    floor.initTexture("textures/grass.png");
    moving.initTexture("textures/grass.png");

    //the player texture/art is the mage.png file in textures folder
    //"Four characters: My LPC entries" by Redshrike licensed CC-BY 3.0, CC-BY-SA 3.0, OGA-BY 3.0
    //https://opengameart.org/content/four-characters-my-lpc-entries
    //downloaded and utilize the "mage walking poses sheet copy.png"
    player.initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);

    float deltaTime = 0.f;

    //set up default view for the window
    //Referenced from the SFML view page
    sf::View view;
    float windowX = window.getSize().x;
    float windowY = window.getSize().y;
    view.setCenter(windowX / 2, windowY / 2);
    view.setSize(windowX, windowY);
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    window.setView(view);

    //switch between the two scaling modes, false = proportional, true = constant
    bool mode = false;

    std::vector<Entity> list;
    list.push_back(moving);
    list.push_back(floor);
    list.push_back(platform);

    std::vector<MovingPlatform> movingList;
    movingList.push_back(moving);

    //keep the window open while program is running
    while(true) {

        deltaTime = clock.restart().asSeconds();

        sf::Event event; //checking for window events

        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) { //check close window event
                window.close();
                exit(1);
            }
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Z) { //if z key is pressed switch modes between constant and proportional
                    if(mode) {
                        //change to default view
                        window.setView(view);
                        mode = false;
                    }
                    else {
                        mode = true;
                    }
                }
            }
            if(mode && event.type == sf::Event::Resized) {
                //make a new view with the proper size and set window view to it
                sf::FloatRect view2(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(view2));
            }
        }

        //update the state of the moving platform and player
        
        std::mutex m;
        std::condition_variable cv;

        Thread t1(0, NULL, &m, &cv);
        Thread t2(1, &t1, &m, &cv);

        std::thread first(run_wrapper, &t1, &moving, &player, deltaTime, std::ref(list));
        std::thread second(run_wrapper, &t2, &moving, &player, deltaTime, std::ref(list));

        first.join();
        second.join();

        //player.wallCollision();
        //player.checkCollision(platform);
        // moving.checkCollision(player);
        // player.checkCollision(moving);
        //player.checkCollision(list[1]);

        //clear window for drawing
        window.clear(sf::Color::Black);

        //draw/render everything
        platform.render(window);
        moving.render(window);
        floor.render(window);
        player.render(window);

        //display everything
        window.display();
    }

    return 0;
}