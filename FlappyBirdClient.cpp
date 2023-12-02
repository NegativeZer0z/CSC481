#include <SFML/Window.hpp>
#include "SpecialZone.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "Thread.h"
#include "Timeline.h"
#include "Entity.h"
#include <zmq.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Spawnpoint.h"
#include "SpecialZone.h"
#include "Boundary.h"
#include <signal.h>
#include "Event.h"
#include "EventManager.h"
#include "DeathHandler.h"
#include "SpawnHandler.h"

#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Platformer", sf::Style::Default, settings);

    window.setFramerateLimit(60);

    //center the window
    window.setPosition(sf::Vector2i(230, 80));

    SpecialZone platform(sf::Vector2f(300.f, 0.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone platform2(sf::Vector2f(300.f, 500.f), sf::Vector2f(50.f, 350.f), 0);

    SpecialZone platform3(sf::Vector2f(600.f, 0.f), sf::Vector2f(50.f, 250.f), 0);
    SpecialZone platform4(sf::Vector2f(600.f, 400.f), sf::Vector2f(50.f, 500.f), 0);

    SpecialZone platform5(sf::Vector2f(900.f, 0.f), sf::Vector2f(50.f, 400.f), 0);
    SpecialZone platform6(sf::Vector2f(900.f, 550.f), sf::Vector2f(50.f, 350.f), 0);

    SpecialZone platform7(sf::Vector2f(1200.f, 0.f), sf::Vector2f(50.f, 475.f), 0);
    SpecialZone platform8(sf::Vector2f(1200.f, 625.f), sf::Vector2f(50.f, 350.f), 0);

    SpecialZone platform9(sf::Vector2f(1500.f, 0.f), sf::Vector2f(50.f, 300.f), 0);
    SpecialZone platform10(sf::Vector2f(1500.f, 450.f), sf::Vector2f(50.f, 350.f), 0);

    SpecialZone platform11(sf::Vector2f(1800.f, 0.f), sf::Vector2f(50.f, 200.f), 0);
    SpecialZone platform12(sf::Vector2f(1800.f, 350.f), sf::Vector2f(50.f, 600.f), 0);

    SpecialZone platform13(sf::Vector2f(2100.f, 0.f), sf::Vector2f(50.f, 550.f), 0);
    SpecialZone platform14(sf::Vector2f(2100.f, 700.f), sf::Vector2f(50.f, 350.f), 0);

    SpecialZone platform15(sf::Vector2f(2400.f, 0.f), sf::Vector2f(50.f, 250.f), 0);
    SpecialZone platform16(sf::Vector2f(2400.f, 400.f), sf::Vector2f(50.f, 450.f), 0);

    SpecialZone platform17(sf::Vector2f(2700.f, 0.f), sf::Vector2f(50.f, 450.f), 0);
    SpecialZone platform18(sf::Vector2f(2700.f, 600.f), sf::Vector2f(50.f, 350.f), 0);

    SpecialZone platform19(sf::Vector2f(3000.f, 0.f), sf::Vector2f(50.f, 150.f), 0);
    SpecialZone platform20(sf::Vector2f(3000.f, 300.f), sf::Vector2f(50.f, 550.f), 0);

    std::vector<SpecialZone*> list;
    list.push_back(&platform);
    list.push_back(&platform2);
    list.push_back(&platform3);
    list.push_back(&platform4);
    list.push_back(&platform5);
    list.push_back(&platform6);
    list.push_back(&platform7);
    list.push_back(&platform8);
    list.push_back(&platform9);
    list.push_back(&platform10);
    list.push_back(&platform11);
    list.push_back(&platform12);
    list.push_back(&platform13);
    list.push_back(&platform14);
    list.push_back(&platform15);
    list.push_back(&platform16);
    list.push_back(&platform17);
    list.push_back(&platform18);
    list.push_back(&platform19);
    list.push_back(&platform20);

    sf::Texture backgroundTex;
    backgroundTex.loadFromFile("textures/Transparent PNG/sky.png");
    sf::Sprite background;
    background.setTexture(backgroundTex);

    std::shared_ptr<Player> player = std::make_shared<Player>(sf::Vector2f(200.f, 450.f), sf::Vector2f(20.f, 20.f));
    
    //the player texture/art is from the bird.png file in textures folder
    //"(Update) Animated Birds character sheet" by Mantis licensed CC0
    //https://opengameart.org/content/update-animated-birds-character-sheet
    //using the first bird texture of the blue bird
    player->initTexture("textures/bird.png", 9, 10, sf::Vector2i(0, 0), sf::Vector2i(0, 0), 0, 0, 0);

    sf::View view;
    float windowX = window.getSize().x;
    float windowY = window.getSize().y;
    view.setCenter(windowX / 2, windowY / 2);
    view.setSize(windowX, windowY);
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    window.setView(view);

    Timeline global(nullptr, 64);
    float lastTime = global.getTime();
    float deltaTime = 0.f;

    while(true) {

        //calc frame delta
        float currTime = global.getTime();
        deltaTime = currTime - lastTime;
        lastTime = currTime;

        sf::Event event; //checking for window events
        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) { //check close window event
                window.close();
                exit(1);
            }
        }

        for(auto &it: list) {
            it->move(-2.0, 0);
            if(it->getPosition().x < -50.0) {
                it->setPosition(2950, it->getPosition().y);
            }
        }

        //std::cout << deltaTime << std::endl;
        player->tempUpdate(deltaTime);
        player->wallCollision(window, view);

        window.clear(sf::Color::Black);

        window.draw(background);
        player->render(window);
        //window.draw(*player);

        for(auto &it: list) {
            window.draw(*it);
        }

        window.display();
    }

    return 0;
}