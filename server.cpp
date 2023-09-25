#include <SFML/Window.hpp>
#include "Player.h"
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
                std::string response;

                //get cap of clients
                int cap = playerList.size();
                response += std::to_string(cap);
                response += " ";

                //loop through all players and get their info
                //response is formatted as (cap id1 x1 y1 id2 x2 y2 etc)
                for(auto i : playerList) {
                    int id = i.first;
                    Player *p = i.second;
                    response += std::to_string(id);
                    response += " ";
                    response += std::to_string(p->getPosition().x);
                    response += " ";
                    response += std::to_string(p->getPosition().y);
                    response += " ";
                }

                //std::cout << response << std::endl;
                zmq::message_t resp(response.size());
                memcpy(resp.data(), response.data(), response.size());
                socket.send(resp, SEND);
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