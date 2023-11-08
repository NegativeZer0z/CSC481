#include <SFML/Window.hpp>
#include "StaticPlatform.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "Thread.h"
#include "Timeline.h"
#include <memory>
#include "Spawnpoint.h"
#include "SpecialZone.h"
#include <unordered_map>
#include "Boundary.h"
#include "Event.h"
#include "EventManager.h"
#include "DeathHandler.h"
#include "SpawnHandler.h"

void run_wrapper(Thread *fe, std::vector<std::shared_ptr<MovingPlatform>>& moving, std::shared_ptr<Player> player, float deltaTime, std::vector<Entity*>& list, bool move, EventManager *manager) {
    fe->runMovement(moving, player, deltaTime, list, move, manager);
}

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Platformer", sf::Style::Default, settings);

    window.setFramerateLimit(60);

    //center the window
    window.setPosition(sf::Vector2i(230, 80));

    //create spawnpoint
    Spawnpoint sp(sf::Vector2f(100.f, 660.f), sf::Vector2f(32.f, 32.f));

    //create death zone
    SpecialZone dz(sf::Vector2f(750.f, 730.f), sf::Vector2f(200.f, 15.f), 0);

    //create Boundary
    Boundary boundary(sf::Vector2f(50.f, 1000.f), sf::Vector2f(750.f, 0.f));

    //creates a moving platform
    std::shared_ptr<MovingPlatform> moving = std::make_shared<MovingPlatform>(sf::Vector2f(690.f, 650.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(1.0f, 0.0f), 4000.0f, 40.f, 0.f);
    std::shared_ptr<MovingPlatform> moving2 = std::make_shared<MovingPlatform>(sf::Vector2f(880.f, 550.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(0.0f, 1.0f), 4000.0f, 40.f, 40.f);

    //creats a static platform
    StaticPlatform platform(sf::Vector2f(550.f, 700.f), sf::Vector2f(100.f, 15.f));
    StaticPlatform platform2(sf::Vector2f(1020.f, 480.f), sf::Vector2f(100.f, 15.f));

    //creates a player
    std::shared_ptr<Player> player = std::make_shared<Player>(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f));

    //the base floors of the game
    StaticPlatform floor(sf::Vector2f(0.f, 750.f), sf::Vector2f(1024.f, 18.f));

    StaticPlatform floor2(sf::Vector2f(1024.f, 750.f), sf::Vector2f(512.f, 18.f));

    //both platforms uses rockfloor.png as the texture in the textures folder
    //"100 Seamless Textures - 461223104.jpg" by Mitch Featherston licensed by CC0
    //https://opengameart.org/node/7814
    platform.initTexture("textures/rockfloor.png");
    platform2.initTexture("textures/blueTile.png");
    moving->initTexture("textures/rockfloor.png");
    moving2->initTexture("textures/rockfloor.png");

    //both floor objects uses grass.png as the texture in the textures folder
    //"29 grounds and walls (and water) (1024x1024) - Grass1.png" by Mysteryem licensed GPL 2.0, GPL 3.0, CC-BY-SA 3.0
    //https://opengameart.org/node/8054
    floor.initTexture("textures/grass.png");
    floor2.initTexture("textures/grass.png");

    //the player texture/art is the mage.png file in textures folder
    //"Four characters: My LPC entries" by Redshrike licensed CC-BY 3.0, CC-BY-SA 3.0, OGA-BY 3.0
    //https://opengameart.org/content/four-characters-my-lpc-entries
    //downloaded and utilize the "mage walking poses sheet copy.png"
    player->initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);

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

    //push all static platforms to a list
    std::vector<Entity*> list;
    list.push_back(&floor);
    list.push_back(&platform);
    list.push_back(&floor2);
    list.push_back(&platform2);

    //init timeline
    Timeline global(nullptr, 64);
    float lastTime = global.getTime();

    std::unordered_map<int, std::shared_ptr<MovingPlatform>> movingList;
    movingList.insert(std::make_pair(1, moving));
    movingList.insert(std::make_pair(2, moving2));

    std::vector<std::shared_ptr<MovingPlatform>> movingVector;
    movingVector.push_back(moving);
    movingVector.push_back(moving2);

    EventManager manager;

    //keep the window open while program is running
    while(true) {

        //calc frame delta
        float currTime = global.getTime();
        deltaTime = currTime - lastTime;
        //std::cout << "curr " << currTime << " last " << lastTime << std::endl;
        lastTime = currTime;

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
                if(event.key.code == sf::Keyboard::P) { //pause and unpause the game when the p key is pressed
                    if(global.isPaused()) { //game is paused so unpause
                        global.unpause();
                        lastTime = global.getTime();
                    }
                    else { //game is unpaused so pause
                        global.pause();
                    }
                }
                if(event.key.code == sf::Keyboard::J) { //change speed to 0.5 by pressing J
                    global.changeTic(128);
                    lastTime = global.getTime();
                }
                if(event.key.code == sf::Keyboard::K) { //change speed to 1.0 by pressing K
                    global.changeTic(64);
                    lastTime = global.getTime();
                }
                if(event.key.code == sf::Keyboard::L) { //change speed to 2.0 by pressing L
                    global.changeTic(32);
                    lastTime = global.getTime();
                }
            }
            if(mode && event.type == sf::Event::Resized) {
                //make a new view with the proper size and set window view to it
                sf::FloatRect view2(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(view2));
            }
        }

        if(global.isPaused()) {
            deltaTime = 0.0;
        }

        //update the state of the moving platform and player with threads and check collision
        std::mutex m;
        std::condition_variable cv;

        std::mutex m2;

        manager.mutex = &m2;

        Thread t1(0, NULL, &m, &cv);
        Thread t2(1, &t1, &m, &cv);

        std::thread first(run_wrapper, &t1, std::ref(movingVector), player, deltaTime, std::ref(list), true, &manager);
        std::thread second(run_wrapper, &t2, std::ref(movingVector), player, deltaTime, std::ref(list), true, &manager);

        first.join();
        second.join();

        // if(!manager.queue.empty()) {
        //     Event e2 = manager.queue.top();
        //     std::cout << e2.getEventType() << std::endl;
        // }

        moving->update(deltaTime);
        moving2->update(deltaTime);

        if(dz.checkCollision(player) && !player->checkState()) {
            Event dead("deathEvent", 0);
            Event spawn("spawnEvent", global.getTime() + 0.03f); //delay for 3 secs
            manager.raise(dead);
            manager.raise(spawn);

            DeathHandler *d = new DeathHandler;
            d->player = player;

            SpawnHandler *s = new SpawnHandler;
            s->player = player;
            s->view = view;
            s->window = &window;
            s->sp = &sp;

            manager.registerEvent("deathEvent", d);
            manager.registerEvent("spawnEvent", s);
            std::cout << "here" << std::endl;
        }

        player->wallCollision(window, view);
        boundary.shift(player, window, view);
        player->applyGravity(deltaTime);

        {
            float eventTime = global.getTime();
            //std::cout << eventTime << std::endl;
            std::lock_guard<std::mutex> lock(*manager.mutex);
            while(!manager.queue.empty()) {
                Event e = manager.queue.top();
                if(e.getTime() < eventTime) {
                    for(auto i : manager.handlers) {
                        bool eventFlag = i.second->onEvent(e);
                        if(eventFlag) {
                            manager.deregisterEvent(e.getEventType());
                            break;
                        }
                    }
                    manager.queue.pop();
                }
                else {
                    break; //reached an event that currently can't be handled or needs to wait to be handled
                }
            }
        }


        //clear window for drawing
        window.clear(sf::Color::Black);

        //draw/render everything
        platform.render(window);
        platform2.render(window);
        moving->render(window);
        moving2->render(window);
        floor.render(window);
        player->render(window);
        floor2.render(window);
        window.draw(dz);
        //window.draw(boundary);

        //display everything
        window.display();
    }

    return 0;
}