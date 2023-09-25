#include <SFML/Window.hpp>
#include "StaticPlatform.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "Thread.h"
#include "Timeline.h"
#include <zmq.hpp>
#include <unordered_map>

#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

int main() {
    //conect to the socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    //map of client ids matching to a player
    std::unordered_map<int, Player*> playerList;
    int nextId = 1;

    //set up the timeline for the game
    Timeline global(nullptr, 64);

    while(true) {
        zmq::message_t reply;
        socket.recv(reply, REPLY);
        std::string message = reply.to_string(); //get the message sent(1)
        //std::cout << message << std::endl;

        //initailize the client by giving it an unique id
        if(message == "init") {
            //give the client it's unique id
            std::string str = std::to_string(nextId);
            zmq::message_t idReply(str.size());
            memcpy(idReply.data(), str.data(), str.size());
            socket.send(idReply, SEND);

            //add player and client id to the map and increment nextId for next client
            if(playerList.find(nextId) == playerList.end()) {
                playerList.insert(std::make_pair(nextId, new Player(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f))));
                playerList.at(nextId)->initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);
            }
            ++nextId;
        }
        else {
            if(message == "getClient") { //somewhere error
                //(2)
                std::string cap = std::to_string(playerList.size());
                zmq::message_t capS(cap.size());
                memcpy(capS.data(), cap.data(), cap.size());
                socket.send(capS, SEND);

                //(3)
                zmq::message_t checking;
                socket.recv(checking, REPLY);
                
                if(checking.to_string() == "garb") {
                    //(4)
                    std::string garbage = "garb";
                    zmq::message_t garbageS(garbage.size());
                    memcpy(garbageS.data(), garbage.data(), garbage.size());
                    socket.send(garbageS, SEND);
                }
                else {
                    //(5)
                    for(auto i : playerList) {
                        //create a string of the players id, x and y position
                        std::string id = std::to_string(i.first);
                        std::string x = std::to_string(i.second->getPosition().x);
                        std::string y = std::to_string(i.second->getPosition().y);
                        std::string info = id + " " + x + " " + y;

                        //send over this info to the client (6)
                        zmq::message_t infoS(info.size());
                        memcpy(infoS.data(), info.data(), info.size());
                        socket.send(infoS, SEND);

                        //receive message of getting the info (7)
                        zmq::message_t useless;
                        socket.recv(useless, REPLY);
                    }
                }
            }
            
            //getTime message so return time back to the client
            else if(message == "getTime") {
                float time = global.getTime(); //get the time and convert it to a string
                std::string str = std::to_string(time);

                //send it over to the client
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message == "pause") { //pause message to pause and unpause the game
                if(global.isPaused()) {
                    global.unpause(); //unpause and send a message to client to update lastTime
                    std::string first = "wasPaused";
                    zmq::message_t firstReply(first.size());
                    memcpy(firstReply.data(), first.data(), first.size());
                    socket.send(firstReply, SEND);

                    zmq::message_t second;
                    socket.recv(second, REPLY);

                    float time = global.getTime(); //update lastTime for the clients
                    std::string str = std::to_string(time);
                    zmq::message_t response(str.size());
                    memcpy(response.data(), str.data(), str.size());
                    socket.send(response, SEND);
                }
                else {
                    //pause the game
                    global.pause();
                    std::string first = "pausing";
                    zmq::message_t firstReply(first.size());
                    memcpy(firstReply.data(), first.data(), first.size());
                    socket.send(firstReply, SEND);
                }
            }
            else if(message == "doubleTic") {
                //slow the game down
                global.changeTic(128);
                float time = global.getTime(); //update lastTime for the clients
                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message == "standardTic") {
                //set the speed to standard
                global.changeTic(64);
                float time = global.getTime(); //update lastTime for the clients
                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message == "halfTic") {
                //speed up the game
                global.changeTic(32);
                float time = global.getTime(); //update lastTime for the clients
                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
        }
    }

    return 0;
}