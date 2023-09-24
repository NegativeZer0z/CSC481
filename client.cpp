#include <SFML/Window.hpp>
#include "StaticPlatform.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "Thread.h"
#include "Timeline.h"
#include "Entity.h"
#include <zmq.hpp>
#include <unordered_map>

//default flags for sending and receiving messages
#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

int main() {
    //connect to socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

    //player id passed to the server to update position for this player for other clients
    int playerId = 0;

    //number of clients connected to the server
    int clientNum = 1;
    
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

    //creats a static platform
    StaticPlatform platform(sf::Vector2f(550.f, 700.f), sf::Vector2f(100.f, 15.f));

    //creates a moving platform
    MovingPlatform moving(sf::Vector2f(770.f, 650.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(1.0f, 0.0f), 4000.0f, 40.f, 0.f);

    //creates the current client's player and add to the list of players
    Player player(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f));
    std::unordered_map<int, Player*> clients;
    clients.insert(std::make_pair(playerId, &player));

    //the base floor of the game
    StaticPlatform floor(sf::Vector2f(0.f, 750.f), sf::Vector2f(1024.f, 18.f));

    //initalize the textures

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
    std::vector<Entity> list;
    list.push_back(floor);
    list.push_back(platform);

    //get the initial time to calc deltaTime
    std::string last = "getTime";
    zmq::message_t timeInit(last.size());
    memcpy(timeInit.data(), last.data(), last.size());
    socket.send(timeInit, SEND);

    zmq::message_t initTime;
    socket.recv(initTime, REPLY);
    float lastTime = std::stof(initTime.to_string());

    bool isPaused = false;
    bool wtf = false;

    while(true) {

        //update the client about all other players/clients in the game (1)
        // std::string getPlayers = "getPlayers";
        // zmq::message_t playerUp(getPlayers.size());
        // memcpy(playerUp.data(), getPlayers.data(), getPlayers.size());
        // socket.send(playerUp, SEND);

        // zmq::message_t rtnPlayer; //(2)
        // socket.recv(rtnPlayer, REPLY);
        
        // std::string cap = std::to_string(clientNum); //(3)
        // zmq::message_t capUp(cap.size());
        // memcpy(capUp.data(), cap.data(), cap.size());
        // socket.send(capUp, SEND);

        // //message to check if we have to update our map of players
        // zmq::message_t mess;
        // socket.recv(mess, REPLY);
        // if(mess.to_string() == "updatePlayers") {
        //     std::string mess2 = std::to_string(playerId);
        //     zmq::message_t mess2R(mess2.size());
        //     memcpy(mess2R.data(), mess2.data(), mess2.size());
        //     socket.send(mess2R, SEND);
            
        //     zmq::message_t maxCap;
        //     socket.recv(maxCap, REPLY);
        //     int max = std::stoi(maxCap.to_string());

        //     std::string useless = "useless";
        //     zmq::message_t uselessR(useless.size());
        //     memcpy(uselessR.data(), useless.data(), useless.size());
        //     socket.send(uselessR, SEND);

        //     for(int i = 0; i < max; ++i) {
        //         zmq::message_t mess3;
        //         socket.recv(mess3, REPLY);
        //         std::string otherId = mess3.to_string();
        //         if(otherId != "nothing") {
        //             int id = std::stoi(otherId);
        //             clients.insert(std::make_pair(id, new Player(sf::Vector2f(300.f, 550.f), sf::Vector2f(28.f, 62.f))));
        //             clients.at(id)->initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);
        //         }
        //         socket.send(uselessR, SEND);
        //     }
        //     clientNum = max;
        // }

        sf::Event event; //checking for window events

        //get the curr time
        std::string curr = "getTime";
        zmq::message_t currInit(curr.size());
        memcpy(currInit.data(), curr.data(), curr.size());
        socket.send(currInit, SEND);

        zmq::message_t nextTime;
        socket.recv(nextTime, REPLY);

        //set the currtime and calc the delatTime
        float currTime = std::stof(nextTime.to_string());
        deltaTime = currTime - lastTime;
        lastTime = currTime;

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
                    std::string p = "pause";
                    zmq::message_t paused(p.size());
                    memcpy(paused.data(), p.data(), p.size());
                    socket.send(paused, SEND);

                    zmq::message_t reply;
                    socket.recv(reply, REPLY);
                    p = reply.to_string();

                    //update the last Time for next frame of deltaTime and set isPaused to correct boolean
                    if(p == "wasPaused") {
                        std::string temp = "updateTime";
                        zmq::message_t updateTemp(temp.size());
                        memcpy(updateTemp.data(), temp.data(), temp.size());
                        socket.send(updateTemp, SEND);

                        zmq::message_t updateReply;
                        socket.recv(updateReply, REPLY);
                        lastTime = std::stof(updateReply.to_string());
                        isPaused = false;
                    }
                    else {
                        isPaused = true;
                    }

                }
                if(event.key.code == sf::Keyboard::J) { //change speed to 0.5 by pressing J
                    //message to double tic to half speed the game
                    std::string half = "doubleTic";
                    zmq::message_t halfTime(half.size());
                    memcpy(halfTime.data(), half.data(), half.size());
                    socket.send(halfTime, SEND);

                    //update lastTime
                    zmq::message_t resp;
                    socket.recv(resp, REPLY);
                    lastTime = std::stof(resp.to_string());
                }
                if(event.key.code == sf::Keyboard::K) { //change speed to 1.0 by pressing K
                    //message to change tic to normal speed the game
                    std::string half = "standardTic";
                    zmq::message_t halfTime(half.size());
                    memcpy(halfTime.data(), half.data(), half.size());
                    socket.send(halfTime, SEND);

                    //update lastTime
                    zmq::message_t resp;
                    socket.recv(resp, REPLY);
                    lastTime = std::stof(resp.to_string());
                }
                if(event.key.code == sf::Keyboard::L) { //change speed to 2.0 by pressing L
                    //message to half tic to double the speed of the game
                    std::string half = "halfTic";
                    zmq::message_t halfTime(half.size());
                    memcpy(halfTime.data(), half.data(), half.size());
                    socket.send(halfTime, SEND);

                    //update lastTime
                    zmq::message_t resp;
                    socket.recv(resp, REPLY);
                    lastTime = std::stof(resp.to_string());
                }
            }
            if(mode && event.type == sf::Event::Resized) {
                //make a new view with the proper size and set window view to it
                sf::FloatRect view2(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(view2));
            }
        }

        //game is paused, no movement
        if(isPaused) {
            deltaTime = 0.f;
        }

        //clear window for drawing
        window.clear(sf::Color::Black);

        Player *p = clients.at(playerId);
        for(auto i : clients) {
            int id = i.first;
            if(id != playerId) {
                i.second->update(deltaTime);
            }
        }
        
        p->update(deltaTime);
        moving.update(deltaTime);
        player.checkCollision(floor);
        player.checkCollision(platform);

        //draw/render everything
        for(auto i : clients) {
            int id = i.first;
            if(id != playerId) {
                i.second->render(window);
            }
        }
        p->render(window);
        platform.render(window);
        floor.render(window);
        moving.render(window);

        //display everything
        window.display();
    }
    
    return 0;
}