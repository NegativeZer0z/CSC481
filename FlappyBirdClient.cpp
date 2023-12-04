#include <SFML/Window.hpp>
#include "SpecialZone.h"
#include "Player.h"
#include "Entity.h"
#include <zmq.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Spawnpoint.h"
#include "SpecialZone.h"
#include <signal.h>
#include "Event.h"
#include "EventManager.h"
#include "DeathHandler.h"
#include "SpawnHandler.h"
#include "InputHandler.h"

#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

int main() {

    //connect to socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "FlappyBird", sf::Style::Default, settings);

    window.setFramerateLimit(60);

    //initial positions of all pipes
    std::vector<float> initPositions;
    initPositions.push_back(400);
    initPositions.push_back(700);
    initPositions.push_back(1000);
    initPositions.push_back(1300);
    initPositions.push_back(1600);
    initPositions.push_back(1900);
    initPositions.push_back(2200);
    initPositions.push_back(2500);
    initPositions.push_back(2800);
    initPositions.push_back(3100);

    //center the window
    window.setPosition(sf::Vector2i(230, 80));

    //the set of pipes and their cooresponding scoring zones
    SpecialZone pipe(sf::Vector2f(400.f, 0.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone pipe2(sf::Vector2f(400.f, 500.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone score(sf::Vector2f(400.f, 350.f), sf::Vector2f(50.f, 150.f), 1);
    score.setFillColor(sf::Color::Red);

    SpecialZone pipe3(sf::Vector2f(700.f, 0.f), sf::Vector2f(50.f, 250.f), 0);
    SpecialZone pipe4(sf::Vector2f(700.f, 400.f), sf::Vector2f(50.f, 500.f), 0);
    SpecialZone score2(sf::Vector2f(700.f, 250.f), sf::Vector2f(50.f, 150.f), 1);
    score2.setFillColor(sf::Color::Red);

    SpecialZone pipe5(sf::Vector2f(1000.f, 0.f), sf::Vector2f(50.f, 400.f), 0);
    SpecialZone pipe6(sf::Vector2f(1000.f, 550.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone score3(sf::Vector2f(1000.f, 400.f), sf::Vector2f(50.f, 150.f), 1);
    score3.setFillColor(sf::Color::Red);

    SpecialZone pipe7(sf::Vector2f(1300.f, 0.f), sf::Vector2f(50.f, 475.f), 0);
    SpecialZone pipe8(sf::Vector2f(1300.f, 625.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone score4(sf::Vector2f(1300.f, 475.f), sf::Vector2f(50.f, 150.f), 1);
    score4.setFillColor(sf::Color::Red);

    SpecialZone pipe9(sf::Vector2f(1600.f, 0.f), sf::Vector2f(50.f, 300.f), 0);
    SpecialZone pipe10(sf::Vector2f(1600.f, 450.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone score5(sf::Vector2f(1600.f, 300.f), sf::Vector2f(50.f, 150.f), 1);
    score5.setFillColor(sf::Color::Red);

    SpecialZone pipe11(sf::Vector2f(1900.f, 0.f), sf::Vector2f(50.f, 200.f), 0);
    SpecialZone pipe12(sf::Vector2f(1900.f, 350.f), sf::Vector2f(50.f, 600.f), 0);
    SpecialZone score6(sf::Vector2f(1900.f, 200.f), sf::Vector2f(50.f, 150.f), 1);
    score6.setFillColor(sf::Color::Red);

    SpecialZone pipe13(sf::Vector2f(2200.f, 0.f), sf::Vector2f(50.f, 550.f), 0);
    SpecialZone pipe14(sf::Vector2f(2200.f, 700.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone score7(sf::Vector2f(2200.f, 550.f), sf::Vector2f(50.f, 150.f), 1);
    score7.setFillColor(sf::Color::Red);

    SpecialZone pipe15(sf::Vector2f(2500.f, 0.f), sf::Vector2f(50.f, 250.f), 0);
    SpecialZone pipe16(sf::Vector2f(2500.f, 400.f), sf::Vector2f(50.f, 450.f), 0);
    SpecialZone score8(sf::Vector2f(2500.f, 250.f), sf::Vector2f(50.f, 150.f), 1);
    score8.setFillColor(sf::Color::Red);

    SpecialZone pipe17(sf::Vector2f(2800.f, 0.f), sf::Vector2f(50.f, 450.f), 0);
    SpecialZone pipe18(sf::Vector2f(2800.f, 600.f), sf::Vector2f(50.f, 350.f), 0);
    SpecialZone score9(sf::Vector2f(2800.f, 450.f), sf::Vector2f(50.f, 150.f), 1);
    score9.setFillColor(sf::Color::Red);

    SpecialZone pipe19(sf::Vector2f(3100.f, 0.f), sf::Vector2f(50.f, 150.f), 0);
    SpecialZone pipe20(sf::Vector2f(3100.f, 300.f), sf::Vector2f(50.f, 550.f), 0);
    SpecialZone score10(sf::Vector2f(3100.f, 150.f), sf::Vector2f(50.f, 150.f), 1);
    score10.setFillColor(sf::Color::Red);

    //list of all the pipes and scoring zones
    std::vector<SpecialZone*> list;
    list.push_back(&pipe);
    list.push_back(&pipe2);
    list.push_back(&score);
    list.push_back(&pipe3);
    list.push_back(&pipe4);
    list.push_back(&score2);
    list.push_back(&pipe5);
    list.push_back(&pipe6);
    list.push_back(&score3);
    list.push_back(&pipe7);
    list.push_back(&pipe8);
    list.push_back(&score4);
    list.push_back(&pipe9);
    list.push_back(&pipe10);
    list.push_back(&score5);
    list.push_back(&pipe11);
    list.push_back(&pipe12);
    list.push_back(&score6);
    list.push_back(&pipe13);
    list.push_back(&pipe14);
    list.push_back(&score7);
    list.push_back(&pipe15);
    list.push_back(&pipe16);
    list.push_back(&score8);
    list.push_back(&pipe17);
    list.push_back(&pipe18);
    list.push_back(&score9);
    list.push_back(&pipe19);
    list.push_back(&pipe20);
    list.push_back(&score10);

    Spawnpoint sp(sf::Vector2f(200.f, 400.f), sf::Vector2f(32.f, 32.f));

    sf::Texture backgroundTex;
    backgroundTex.loadFromFile("textures/Transparent PNG/sky.png");
    sf::Sprite background;
    background.setTexture(backgroundTex);

    std::shared_ptr<Player> player = std::make_shared<Player>(sf::Vector2f(200.f, 400.f), sf::Vector2f(20.f, 20.f));
    
    //the player texture/art is from the bird.png file in textures folder
    //"(Update) Animated Birds character sheet" by Mantis licensed CC0
    //https://opengameart.org/content/update-animated-birds-character-sheet
    //using the first bird texture of the blue bird
    player->initTexture("textures/bird.png", 9, 10, sf::Vector2i(0, 0), sf::Vector2i(0, 0), 0, 0, 0);

    int playerId = 0;

    //send message to server to get the player id for this client
    std::string init = "init";
    zmq::message_t initRequest(init.size());
    memcpy(initRequest.data(), init.data(), init.size());
    socket.send(initRequest, SEND);

    //get the id and set it and create the static pipes and player and the sfml window
    zmq::message_t id;
    socket.recv(id, REPLY);
    playerId = std::stoi(id.to_string());

    sf::View view;
    float windowX = window.getSize().x;
    float windowY = window.getSize().y;
    view.setCenter(windowX / 2, windowY / 2);
    view.setSize(windowX, windowY);
    view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    window.setView(view);

    float deltaTime = 0.f;

    //get the initial time to calc deltaTime
    std::string last = "getTime";
    zmq::message_t timeInit(last.size());
    memcpy(timeInit.data(), last.data(), last.size());
    socket.send(timeInit, SEND);

    zmq::message_t initTime;
    socket.recv(initTime, REPLY);
    float lastTime = std::stof(initTime.to_string());

    EventManager manager; //event manager

    std::mutex m;
    manager.mutex = &m;

    bool focused = false;

    //set up the text and font used for the score counter
    //the font used is in the textures/fonts folder called Boxy-Bold.ttf
    //"Boxy Bold - TrueType Font" by William.Thompsonj licensed CC0
    //https://opengameart.org/content/boxy-bold-truetype-font
    sf::Font font;
    if(!font.loadFromFile("textures/fonts/Boxy-Bold.ttf")) {
        std::cout << "can't load font" << std::endl;
        exit(1);
    }

    sf::Text text;
    text.setFont(font);
    text.setString(std::to_string(player->getScore()));
    text.setCharacterSize(48);
    text.setFillColor(sf::Color::White);
    text.setPosition(windowX / 2, 50.f);

    //frame count (number of game loops)
    int frame = 0;

    while(true) {

        //send message to server to make sure we are active
        std::string active = "active ";
        active += std::to_string(playerId);
        zmq::message_t activeSend(active.size());
        memcpy(activeSend.data(), active.data(), active.size());
        socket.send(activeSend, SEND);

        zmq::message_t activeRtn;
        socket.recv(activeRtn, REPLY);

        sf::Event event; //checking for window events
        while(window.pollEvent(event)) {

            if(event.type == sf::Event::Closed) { //check close window event
                std::string dis = "disconnect ";
                dis += std::to_string(playerId);
                zmq::message_t disSend(dis.size());
                memcpy(disSend.data(), dis.data(), dis.size());
                socket.send(disSend, SEND);

                zmq::message_t exited;
                socket.recv(exited, REPLY);

                window.close();
                exit(1);
            }
            //check for focus of window
            if(event.type == sf::Event::LostFocus) {
                std::cout << "lost focused" << std::endl;
                focused = false;
            }
            if(event.type == sf::Event::GainedFocus) {
                std::cout << "focused" << std::endl;
                focused = true;
            }
        }

        //get the curr time
        std::string curr = "getTime";
        zmq::message_t currInit(curr.size());
        memcpy(currInit.data(), curr.data(), curr.size());
        socket.send(currInit, SEND);

        zmq::message_t nextTime;
        socket.recv(nextTime, REPLY);

        //set the currtime and calc the deltaTime
        float currTime = std::stof(nextTime.to_string());
        deltaTime = currTime - lastTime;
        lastTime = currTime;

        ++frame; //increment it for updating the score

        if(!player->checkState()) {
            for(auto &it: list) {
                it->move(-2.0, 0);
                std::string str = player->checkCollision(*it);
                if(str == "rightCollision" && it->getType() == 1 && frame % 13 == 0) { //need check every 13 frames for collision to update score
                    player->incrementScore();
                }

                bool flag = it->checkCollision(player);
                if(flag && frame % 13 == 0 && it->getType() == 0) {
                    Event dead("deathEvent", 0);
                    Event spawn("spawnEvent", currTime + 0.03f); //delay for 3 secs
                    manager.raise(dead);
                    manager.raise(spawn);

                    DeathHandler *d = new DeathHandler;
                    d->player = player;

                    SpawnHandler *s = new SpawnHandler;
                    s->player = player;
                    s->sp = &sp;

                    manager.registerEvent("deathEvent", d);
                    manager.registerEvent("spawnEvent", s);
                }
                if(it->getPosition().x < -50.0) {
                    it->setPosition(2950, it->getPosition().y);
                }
            }
        }

        //input event
        std::string input = player->updateEvent();
        if(input == "jump") {
            Event e("flappyJump", 0);
            manager.raise(e);

            InputHandler *ih = new InputHandler;
            ih->player = player;
            ih->deltaTime = deltaTime;

            manager.registerEvent("flappyJump", ih);
        }
        
        player->applyGravity(deltaTime, 3);

        //check for collision agasint pipes
        bool collide = player->wallCollision(window, view);
        if(collide && !player->checkState()) {
            Event dead("deathEvent", 0);
            Event spawn("spawnEvent", currTime + 0.03f); //delay for 3 secs
            manager.raise(dead);
            manager.raise(spawn);

            DeathHandler *d = new DeathHandler;
            d->player = player;

            SpawnHandler *s = new SpawnHandler;
            s->player = player;
            s->sp = &sp;

            manager.registerEvent("deathEvent", d);
            manager.registerEvent("spawnEvent", s);
        }

        text.setString(std::to_string(player->getScore()));

        //handle events
        {
            float eventTime = currTime;
            std::lock_guard<std::mutex> lock(*manager.mutex);
            while(!manager.queue.empty()) {
                Event e = manager.queue.top();
                if(e.getTime() < eventTime) {
                    for(auto i : manager.handlers) {
                        bool eventFlag = i.second->onEvent(e);
                        //reset the game on spawnEvent
                        if(i.first == "spawnEvent") {
                            int idx = 0;
                            for(int v = 0; v < 10; ++v) {
                                float x = initPositions[v];
                                list[idx]->setPosition(x, list[idx]->getPosition().y);
                                ++idx;
                                list[idx]->setPosition(x, list[idx]->getPosition().y);
                                ++idx;
                                list[idx]->setPosition(x, list[idx]->getPosition().y);
                                ++idx;
                            }
                        }
                        if(eventFlag) {
                            manager.deregisterEvent(e.getEventType());
                            break;
                        }
                    }
                    manager.queue.pop();
                }
                else {
                    break; //don't handle the rest of the events, need to wait on them
                }
            }
        }

        //rendering
        window.clear(sf::Color::Black);

        window.draw(background);
        if(!player->checkState()) {
            player->render(window);
        }

        for(auto &it: list) {
            if(it->getType() == 0) {
                window.draw(*it);
            }
        }
        window.draw(text);

        window.display();
    }

    return 0;
}