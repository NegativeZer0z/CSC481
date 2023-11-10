#include <SFML/Window.hpp>
#include "StaticPlatform.h"
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

//default flags for sending and receiving messages
#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

void run_wrapper(Thread *fe, std::vector<std::shared_ptr<MovingPlatform>>& moving, std::shared_ptr<Player> player, float deltaTime, std::vector<std::shared_ptr<Entity>>& list, bool move, EventManager *manager) {
    fe->runMovement(moving, player, deltaTime, list, move, manager);
}

int main() {
    //connect to socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

    //player id passed to the server to update position for this player for other clients
    int playerId = 0;
    
    //send message to server to get the player id for this client
    std::string init = "init";
    zmq::message_t initRequest(init.size());
    memcpy(initRequest.data(), init.data(), init.size());
    socket.send(initRequest, SEND);

    //get the id and set it and create the static platforms and player and the sfml window
    zmq::message_t id;
    socket.recv(id, REPLY);
    playerId = std::stoi(id.to_string());

    //sfml window setup
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Platformer", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    //center the window
    window.setPosition(sf::Vector2i(230, 80));

    //create spawnpoint
    Spawnpoint sp(sf::Vector2f(100.f, 660.f), sf::Vector2f(32.f, 32.f));

    //create death zone
    SpecialZone dz(sf::Vector2f(650.f, 730.f), sf::Vector2f(1000.f, 15.f), 0);

    //create Boundary
    Boundary boundary(sf::Vector2f(50.f, 1000.f), sf::Vector2f(700.f, 0.f));

    //creats a static platform
    std::shared_ptr<StaticPlatform> platform = std::make_shared<StaticPlatform>(sf::Vector2f(550.f, 700.f), sf::Vector2f(100.f, 15.f));
    std::shared_ptr<StaticPlatform> platform2 = std::make_shared<StaticPlatform>(sf::Vector2f(1020.f, 480.f), sf::Vector2f(100.f, 15.f));

    //creates a moving platform
    std::shared_ptr<MovingPlatform> moving = std::make_shared<MovingPlatform>(sf::Vector2f(690.f, 650.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(1.0f, 0.0f), 4000.0f, 40.f, 0.f);
    std::shared_ptr<MovingPlatform> moving2 = std::make_shared<MovingPlatform>(sf::Vector2f(880.f, 550.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(0.0f, 1.0f), 4000.0f, 40.f, 40.f);

    //creates the current client's player and add to the list of players
    //Player player(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f)); //temp here for testing, should add to the map in the loop of each new client
    //std::unordered_map<int, Player*> clients;
    //clients.insert(std::make_pair(playerId, &player));

    //the base floor of the game
    std::shared_ptr<StaticPlatform> floor = std::make_shared<StaticPlatform>(sf::Vector2f(0.f, 750.f), sf::Vector2f(1024.f, 18.f));
    std::shared_ptr<StaticPlatform> floor2 = std::make_shared<StaticPlatform>(sf::Vector2f(1024.f, 750.f), sf::Vector2f(512.f, 18.f));

    //initalize the textures

    //both moving platforms and one static platform uses rockfloor.png as the texture in the textures folder
    //"100 Seamless Textures - 461223104.jpg" by Mitch Featherston licensed by CC0
    //https://opengameart.org/node/7814
    platform->initTexture("textures/rockfloor.png");
    moving->initTexture("textures/rockfloor.png");
    moving2->initTexture("textures/rockfloor.png");

    //the second static platform uses blueTile.png as the texture from the textures folder
    //"40 procedural textures - texture26.png" by drummyfish licensed by CC0
    //https://opengameart.org/content/40-procedural-textures-texture26png
    platform2->initTexture("textures/blueTile.png");

    //both floor platforms uses grass.png as the texture in the textures folder
    //"29 grounds and walls (and water) (1024x1024) - Grass1.png" by Mysteryem licensed GPL 2.0, GPL 3.0, CC-BY-SA 3.0
    //https://opengameart.org/node/8054
    floor->initTexture("textures/grass.png");
    floor2->initTexture("textures/grass.png");

    //the player texture/art is the mage.png file in textures folder
    //"Four characters: My LPC entries" by Redshrike licensed CC-BY 3.0, CC-BY-SA 3.0, OGA-BY 3.0
    //https://opengameart.org/content/four-characters-my-lpc-entries
    //downloaded and utilize the "mage walking poses sheet copy.png"
    //player.initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);

    //deltaTime for movement/velocity of entities
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
    std::vector<std::shared_ptr<Entity>> list;
    list.push_back(floor);
    list.push_back(platform);
    list.push_back(platform2);
    list.push_back(floor2);

    //get the initial time to calc deltaTime
    std::string last = "getTime";
    zmq::message_t timeInit(last.size());
    memcpy(timeInit.data(), last.data(), last.size());
    socket.send(timeInit, SEND);

    zmq::message_t initTime;
    socket.recv(initTime, REPLY);
    float lastTime = std::stof(initTime.to_string());

    //game state vars
    bool isPaused = false;
    bool fast = false;
    bool slow = false;

    bool focused = false;

    //list of all players
    std::unordered_map<int, std::shared_ptr<Player>> clients;

    std::shared_ptr<Player> player;

    //store our moving platforms
    std::unordered_map<int, std::shared_ptr<MovingPlatform>> movingList;
    movingList[0] = moving;
    movingList[1] = moving2;

    std::vector<std::shared_ptr<MovingPlatform>> movingVector;
    movingVector.push_back(moving);
    movingVector.push_back(moving2);

    bool initalize = false;

    //a list of available ids still connected to the server
    std::vector<int> available;

    EventManager manager; //event manager

    bool justDied = false;

    while(true) {

        //send message to server to make sure we are active
        std::string active = "active ";
        active += std::to_string(playerId);
        zmq::message_t activeSend(active.size());
        memcpy(activeSend.data(), active.data(), active.size());
        socket.send(activeSend, SEND);

        zmq::message_t activeRtn;
        socket.recv(activeRtn, REPLY);

        //add the clients to the clients map
        std::string getClient = "getClient"; //(1)
        zmq::message_t getClientS(getClient.size());
        memcpy(getClientS.data(), getClient.data(), getClient.size());
        socket.send(getClientS, SEND); 

        //(2)
        zmq::message_t info; //how many clients in the server
        socket.recv(info, REPLY);
        std::string info2 = info.to_string();
        //std::cout << info2 << std::endl;
        int cap;
        int pos; //current position of processing the string
        sscanf(info2.c_str(), "%d %n", &cap, &pos);
        const char *players = info2.c_str();
        players += pos;
        //std::cout << players << std::endl;

        //load in all of the players
        for(int i = 0; i < cap; ++i) {
            int id;
            float x, y;
            sscanf(players, "%d %f %f %n", &id, &x, &y, &pos);
            players += pos;

            available.push_back(id); //add the id to the available list

            if(clients.find(id) == clients.end()) {
                clients.insert(std::make_pair(id, std::make_shared<Player>(sf::Vector2f(x, y), sf::Vector2f(28.f, 62.f))));
                //clients.insert(std::make_pair(id, new Player(sf::Vector2f(x, y), sf::Vector2f(28.f, 62.f))));
                clients.at(id)->initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);
            }
            else {
                //update the position of all other clients
                if(id != playerId) {
                    clients.at(id)->setSpritePosition(x, y);
                }
                else if(justDied && id == playerId) { //update from server the spawn
                    clients.at(id)->setSpritePosition(x, y);
                    justDied = false;
                }
            }
        }

        //init player
        if(!initalize) {
            for(auto i : clients) {
                if(i.first == playerId) {
                    player = i.second;
                    initalize = true;
                }
            }
        }

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
                    // std::string p = "pause";
                    // zmq::message_t paused(p.size());
                    // memcpy(paused.data(), p.data(), p.size());
                    // socket.send(paused, SEND);

                    // zmq::message_t reply;
                    // socket.recv(reply, REPLY);
                    // p = reply.to_string();
                    // float dt;

                    // char *s = new char[p.length() + 1];
                    // sscanf(p.c_str(), "%s %f", s, &dt);
                    // std::string s2 = s;

                    // //update the last Time for next frame of deltaTime and set isPaused to correct boolean
                    // if(s2 == "wasPaused") {
                    //     lastTime = dt;
                    //     isPaused = false;
                    // }
                    // else {
                    //     isPaused = true;
                    // }
                    // delete s;
                    if(isPaused) {
                        isPaused = false;
                        fast = false;
                        slow = false;
                    }
                    else {
                        isPaused = true;
                        fast = false;
                        slow = false;
                    }
                }
                if(event.key.code == sf::Keyboard::J) { //change speed to 0.5 by pressing J
                    //message to double tic to half speed the game
                    std::string half = "doubleTic";
                    zmq::message_t halfTime(half.size());
                    memcpy(halfTime.data(), half.data(), half.size());
                    socket.send(halfTime, SEND);

                    if(!isPaused) {
                        fast = false;
                        slow = true;
                    }

                    //update lastTime
                    zmq::message_t resp;
                    socket.recv(resp, REPLY);
                    lastTime = std::stof(resp.to_string());
                }
                if(event.key.code == sf::Keyboard::K) { //change speed to 1.0 by pressing K
                    //message to change tic to normal speed the game
                    // std::string half = "standardTic";
                    // zmq::message_t halfTime(half.size());
                    // memcpy(halfTime.data(), half.data(), half.size());
                    // socket.send(halfTime, SEND);

                    if(!isPaused) {
                        fast = false;
                        slow = false;
                    }

                    //update lastTime
                    // zmq::message_t resp;
                    // socket.recv(resp, REPLY);
                    // lastTime = std::stof(resp.to_string());
                }
                if(event.key.code == sf::Keyboard::L) { //change speed to 2.0 by pressing L
                    //message to half tic to double the speed of the game
                    // std::string half = "halfTic";
                    // zmq::message_t halfTime(half.size());
                    // memcpy(halfTime.data(), half.data(), half.size());
                    // socket.send(halfTime, SEND);

                    if(!isPaused) {
                        fast = true;
                        slow = false;
                    }

                    //update lastTime
                    // zmq::message_t resp;
                    // socket.recv(resp, REPLY);
                    // lastTime = std::stof(resp.to_string());
                }
            }
            if(mode && event.type == sf::Event::Resized) {
                //make a new view with the proper size and set window view to it
                sf::FloatRect view2(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(view2));
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

        //get the current state of the game, fast, slow, paused, etc
        // std::string getState = "getState";
        // zmq::message_t getStateS(getState.size());
        // memcpy(getStateS.data(), getState.data(), getState.size());
        // socket.send(getStateS, SEND);

        // zmq::message_t rtnState;
        // socket.recv(rtnState, REPLY);
        // std::string currState = rtnState.to_string();

        // if(currState == "pause") {
        //     isPaused = true;
        //     fast = false;
        //     slow = false;
        // }
        // else if(currState == "fast") {
        //     fast = true;
        //     slow = false;
        // }
        // else if(currState == "slow") {
        //     slow = true;
        //     fast = false;
        // }
        // else if(currState == "normal") {
        //     isPaused = false;
        //     slow = false;
        //     fast = false;
        // }

        //update moving platforms positions
        std::string getMoving = "getMoving";
        zmq::message_t getMovingS(getMoving.size());
        memcpy(getMovingS.data(), getMoving.data(), getMoving.size());
        socket.send(getMovingS, SEND);

        zmq::message_t movingInfo;
        socket.recv(movingInfo, REPLY);
        std::string movingStr = movingInfo.to_string();

        //parse string of postions and directions of moving platforms
        int movingCap;
        int movingPos;
        sscanf(movingStr.c_str(), "%d %n", &movingCap, &movingPos);
        const char *movingPlats = movingStr.c_str();
        movingPlats += movingPos;

        for(int i = 0; i < movingCap; ++i) {
            float x, y;
            float dirx, diry;
            int id;
            sscanf(movingPlats, "%d %f %f %f %f %n", &id, &x, &y, &dirx, &diry, &movingPos);
            movingPlats += movingPos;
            movingList.at(id)->setSpritePosition(x, y);
            movingList.at(id)->setDirection(sf::Vector2f(dirx, diry));
        }

        //send a string of all of the clients positions and ids
        //string format of (number of clients id1 x1 y1 id2 x2 y2 ... deltaTiime)
        std::string upServer = std::to_string(clients.size());
        upServer += " ";
        for(auto i : clients) {
            upServer += std::to_string(i.first);
            upServer += " ";
            upServer += std::to_string(i.second->getPosition().x);
            upServer += " ";
            upServer += std::to_string(i.second->getPosition().y);
            upServer += " ";
        }

        upServer += std::to_string(deltaTime);

        zmq::message_t updateServer(upServer.size());
        memcpy(updateServer.data(), upServer.data(), upServer.size());
        socket.send(updateServer, SEND);

        zmq::message_t noUse;
        socket.recv(noUse, REPLY);

        //game is paused, no movement
        if(isPaused) {
            deltaTime = 0.f;
        }

        //clear window for drawing
        window.clear(sf::Color::Black);

        if(fast) {
            deltaTime *= 2;
        }
        else if(slow) {
            deltaTime /= 2;
        }

        //bug regarding unpausing
        if(deltaTime < 0.f) {
            deltaTime = 0.0025f;
        }

        //threads for collision and movement for current player
        std::mutex m;
        std::condition_variable cv;

        std::mutex m2;
        manager.mutex = &m2;

        Thread t1(0, NULL, &m, &cv);
        Thread t2(1, &t1, &m, &cv);

        std::thread first(run_wrapper, &t1, std::ref(movingVector), player, deltaTime, std::ref(list), focused, &manager);
        std::thread second(run_wrapper, &t2, std::ref(movingVector), player, deltaTime, std::ref(list), focused, &manager);

        first.join();
        second.join();

        if(dz.checkCollision(player) && !player->checkState()) {
            Event dead("deathEvent", 0);
            
            manager.raise(dead);

            DeathHandler *d = new DeathHandler;
            d->player = player;


            manager.registerEvent("deathEvent", d);

            //tell the server this player has died so create and handle the spawn event on the server
            std::string spawnStr = "spawnEvent ";
            spawnStr += std::to_string(playerId);

            zmq::message_t spawnEvent(spawnStr.size());
            memcpy(spawnEvent.data(), spawnStr.data(), spawnStr.size());
            socket.send(spawnEvent, SEND);

            zmq::message_t spawnRtn;
            socket.recv(spawnRtn, REPLY);

            view.setCenter(windowX / 2, windowY / 2);
            window.setView(view);

            justDied = true;

            //std::cout << spawnStr << std::endl;
        }

        player->wallCollision(window, view);
        boundary.shift(player, window, view);
        if(focused) {
            player->applyGravity(deltaTime);
        }

        //handle events
        {
            float eventTime = currTime;
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
                    break; //don't handle the rest of the events, need to wait on them
                }
            }
        }

        //draw/render everything
        for(auto i : clients) {
            if(std::find(available.begin(), available.end(), i.first) != available.end()) {
                i.second->render(window);
            }
        }
        platform->render(window);
        platform2->render(window);
        floor->render(window);
        floor2->render(window);

        for(auto& i : movingVector) {
            i->render(window);
        }
        //std::cout << moving->getPosition().x << " " << moving->getPosition().y << std::endl;

        //display everything
        window.display();

        available.clear();
    }
    
    return 0;
}