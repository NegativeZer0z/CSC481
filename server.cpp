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
    std::unordered_map<int, Player> playerList;
    int nextId = 1;

    //set up the timeline for the game
    Timeline global(nullptr, 64);

    while(true) {
        zmq::message_t reply;
        socket.recv(reply, REPLY);
        std::string message = reply.to_string();

        // if(message == "getPlayers") {
        //     std::string temp = "temp"; //(2)
        //     zmq::message_t tempReply(temp.size());
        //     memcpy(tempReply.data(), temp.data(), temp.size());
        //     socket.send(tempReply, SEND);

        //     zmq::message_t resp; //(3)
        //     socket.recv(resp, REPLY);
        //     int cap = std::stoi(resp.to_string());

        //     if(cap < playerList.size()) {
        //         std::string mess = "updatePlayers";
        //         zmq::message_t messReply(mess.size());
        //         memcpy(messReply.data(), mess.data(), mess.size());
        //         socket.send(messReply, SEND);

        //         zmq::message_t tempId;
        //         socket.recv(tempId, REPLY);
        //         int id = std::stoi(tempId.to_string());
        //         int size = playerList.size();

        //         //maxCap send
        //         std::string mess2 = std::to_string(size);
        //         zmq::message_t mess2R(mess2.size());
        //         memcpy(mess2R.data(), mess2.data(), mess2.size());
        //         socket.send(mess2R, SEND);

        //         zmq::message_t useless;
        //         socket.recv(useless, REPLY);

        //         for(auto p : playerList) {
        //             int otherId = p.first;
        //             if(otherId != id) {
        //                 std::string over = std::to_string(otherId);
        //                 zmq::message_t otherR(over.size());
        //                 memcpy(otherR.data(), over.data(), over.size());
        //                 socket.send(otherR, SEND);
        //             }
        //             else {
        //                 std::string nothing = "nothing";
        //                 zmq::message_t nothingR(nothing.size());
        //                 memcpy(nothingR.data(), nothing.data(), nothing.size());
        //                 socket.send(nothingR, SEND);
        //             }
        //             socket.recv(useless, REPLY);
        //         }
        //     }
        //     else {
        //         socket.send(tempReply, SEND);
        //     }
        // }
        //initailize the client by giving it an unique id
        if(message == "init") {
            std::string str = std::to_string(nextId);
            zmq::message_t idReply(str.size());
            memcpy(idReply.data(), str.data(), str.size());
            socket.send(idReply, SEND);

            //add player and client id to the map and increment nextId for next client
            if(playerList.find(nextId) == playerList.end()) {
                Player temp(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f));
                temp.initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);
                playerList.insert(std::make_pair(nextId, temp));
            }
            ++nextId;
        }
        else {

            //getTime message so return time back to the client
            if(message == "getTime") {
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
                    global.pause();
                    std::string first = "pausing";
                    zmq::message_t firstReply(first.size());
                    memcpy(firstReply.data(), first.data(), first.size());
                    socket.send(firstReply, SEND);
                }
            }
            else if(message == "doubleTic") {
                global.changeTic(128);
                float time = global.getTime(); //update lastTime for the clients
                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message == "standardTic") {
                global.changeTic(64);
                float time = global.getTime(); //update lastTime for the clients
                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message == "halfTic") {
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