#include <SFML/Window.hpp>
#include "Player.h"
#include "Timeline.h"
#include <zmq.hpp>
#include <unordered_map>
#include "StaticPlatform.h"
#include "MovingPlatform.h"
#include <vector>
#include <memory>
#include "Spawnpoint.h"
#include "SpecialZone.h"
#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include "Boundary.h"

#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

//map of client ids matching to a player
std::unordered_map<int, std::shared_ptr<Player>> playerList;
int nextId = 1;
std::unordered_map<int, float> playerTimes; //map of player times for activity

int64_t defaultTic = 64;

//set up the timeline for the game
Timeline global(nullptr, defaultTic);

//timeline bools
bool fast = false;
bool slow = false;

//base platforms
StaticPlatform floor1(sf::Vector2f(0.f, 750.f), sf::Vector2f(1024.f, 18.f));
StaticPlatform floor2(sf::Vector2f(1024.f, 750.f), sf::Vector2f(512.f, 18.f));

//static platforms
StaticPlatform platform(sf::Vector2f(550.f, 700.f), sf::Vector2f(100.f, 15.f));
StaticPlatform platform2(sf::Vector2f(1020.f, 480.f), sf::Vector2f(100.f, 15.f));

//moving platforms
std::shared_ptr<MovingPlatform> moving = std::make_shared<MovingPlatform>(sf::Vector2f(690.f, 650.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(1.0f, 0.0f), 4000.0f, 40.f, 0.f);
std::shared_ptr<MovingPlatform> moving2 = std::make_shared<MovingPlatform>(sf::Vector2f(880.f, 550.f), sf::Vector2f(100.f, 15.f), sf::Vector2f(0.0f, 1.0f), 4000.0f, 40.f, 40.f);

std::unordered_map<int, std::shared_ptr<MovingPlatform>> movingList;

//create spawnpoint
Spawnpoint sp(sf::Vector2f(100.f, 660.f), sf::Vector2f(32.f, 32.f));

//create death zone
SpecialZone dz(sf::Vector2f(650.f, 730.f), sf::Vector2f(1000.f, 15.f), 0);

//create Boundary
Boundary boundary(sf::Vector2f(50.f, 1000.f), sf::Vector2f(850.f, 0.f));

void *worker_routine(void *arg) {

    //connect to socket
    zmq::context_t *context = (zmq::context_t *) arg;
    zmq::socket_t socket(*context, ZMQ_REP);
    socket.connect("inproc://workers");

    while(true) {

        //make sure players are active and if not delete them
        float testTime = global.getTime();
        for(auto& p : playerTimes) {
            if(testTime - p.second > 0.03) {
                playerList.erase(p.first);
                playerTimes.erase(p.first);
            }
            //std::cout << p.first << "|" << p.second << std::endl;
        }

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
                playerList.insert(std::make_pair(nextId, std::make_shared<Player>(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f))));
                playerList.at(nextId)->initTexture("textures/mage.png", 9, 4, sf::Vector2i(8, 1), sf::Vector2i(8, 3), MAGE_LEFT_OFFSET, MAGE_BOT_OFFSET, MAGE_START_OFFSET);\
                playerTimes[nextId] = global.getTime();;
            }
            ++nextId;
            std::cout << "connect" << std::endl;
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
                    std::shared_ptr<Player> p = i.second;
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
                    std::string first = "wasPaused ";
                    float time = global.getTime();
                    first += std::to_string(time);

                    //"wasPaused time" send over message
                    zmq::message_t firstReply(first.size()); 
                    memcpy(firstReply.data(), first.data(), first.size());
                    socket.send(firstReply, SEND);
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
                //slow the game down by doubling tic
                int64_t currTic = global.getTic();
                currTic *= 2;
                //global.changeTic(currTic);
                fast = false;
                slow = true;

                float time = global.getTime(); //update lastTime for the clients
                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message == "standardTic") {
                //set the speed to standard
                //global.changeTic(defaultTic);
                fast = false;
                slow = false;
                float time = global.getTime(); //update lastTime for the clients

                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message == "halfTic") {
                //speed up the game by halfing tic
                int64_t currTic = global.getTic();
                currTic /= 2;
                //global.changeTic(currTic);
                fast = true;
                slow = false;

                float time = global.getTime(); //update lastTime for the clients
                std::string str = std::to_string(time);
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message.find("disconnect") != std::string::npos) {
                int exitId;
                sscanf(message.c_str() + 11, "%d", &exitId); //read in the id
                //std::cout << message << std::endl;
                //std::cout << exitId << std::endl;
                playerList.erase(exitId);
                playerTimes.erase(exitId);

                std::string tempMess = "exitNow";
                zmq::message_t tempMessSend(tempMess.size());
                memcpy(tempMessSend.data(), tempMess.data(), tempMess.size());
                socket.send(tempMessSend, SEND);
            }
            else if(message == "getMoving") { //update the moving platforms for clients return format "cap id x y dir.x dir.y"
                //std::cout << "moving" << std::endl;
                std::string movingRtn = std::to_string(movingList.size());
                movingRtn += " ";
                for(auto& i : movingList) {
                    sf::Vector2f pos = i.second->getPosition();
                    sf::Vector2f dir = i.second->getDirection();
                    movingRtn += std::to_string(i.first);
                    movingRtn += " ";
                    movingRtn += std::to_string(pos.x);
                    movingRtn += " ";
                    movingRtn += std::to_string(pos.y);
                    movingRtn += " ";
                    movingRtn += std::to_string(dir.x);
                    movingRtn += " ";
                    movingRtn += std::to_string(dir.y);
                    movingRtn += " ";
                }
                zmq::message_t movingRtnStr(movingRtn.size());
                memcpy(movingRtnStr.data(), movingRtn.data(), movingRtn.size());
                socket.send(movingRtnStr, SEND);                
            }
            else if(message == "getState") {
                std::string currState;
                if(global.isPaused()) {
                    currState = "pause";
                }
                else if(fast) {
                    currState = "fast";
                }
                else if(slow) {
                    currState = "slow";
                }
                else {
                    currState = "normal";
                }
                zmq::message_t rtnState(currState.size());
                memcpy(rtnState.data(), currState.data(), currState.size());
                socket.send(rtnState, SEND);
            }
            else if(message.find("active") != std::string::npos) { //make sure player is active
                float connectId;
                sscanf(message.c_str() + 7, "%f", &connectId);

                playerTimes[connectId] = global.getTime();

                std::string tempMess = "activeNow";
                zmq::message_t tempMessSend(tempMess.size());
                memcpy(tempMessSend.data(), tempMess.data(), tempMess.size());
                socket.send(tempMessSend, SEND);
            }
            else { //if no other message means we got some time frame so update our entities
                //update the players and platforms
                //std::cout << message << std::endl;

                //read in the number of clients(cap)
                int pos;
                int cap;
                sscanf(message.c_str(), "%d %n", &cap, &pos);

                const char *players = message.c_str();
                players += pos;

                //update all of the clients positions
                for(int i = 0; i < cap; ++i) {
                    int id;
                    float x, y;
                    sscanf(players, "%d %f %f %n", &id, &x, &y, &pos);
                    players += pos;
                    if(playerList.find(id) != playerList.end()) {
                        playerList.at(id)->setSpritePosition(x, y);
                    }
                }

                float deltaTime;
                sscanf(players, "%f", &deltaTime);

                // if(fast) {
                //     deltaTime *= 2;
                // }
                // else if(slow) {
                //     deltaTime /= 2;
                // }
                // else if(global.isPaused()) {
                //     deltaTime = 0;
                // }

                //account for more clients connecting to prevent speedup
                deltaTime /= playerList.size();

                //update moving platform
                moving->update(deltaTime);
                moving2->update(deltaTime);
                //std::cout << moving->getPosition().x << " " << moving->getPosition().y << std::endl;

                //garbage, but we need to send something
                std::string garb = "garb";
                zmq::message_t garbS(garb.size());
                memcpy(garbS.data(), garb.data(), garb.size());
                socket.send(garbS, SEND);
            }
        }
    }
    return(NULL);
}

int main() {
    //init sprites and textures
    floor1.initTexture("textures/grass.png");
    floor2.initTexture("textures/grass.png");
    platform.initTexture("textures/rockfloor.png");
    platform2.initTexture("textures/rockfloor.png");
    moving->initTexture("textures/rockfloor.png");
    moving2->initTexture("textures/rockfloor.png");

    movingList[0] = moving;
    movingList[1] = moving2;

    //connect workers
    zmq::context_t context(1);
    zmq::socket_t clients(context, ZMQ_ROUTER);
    clients.bind("tcp://*:5555");
    zmq::socket_t workers(context, ZMQ_DEALER);
    workers.bind("inproc://workers");

    //create threads
    for(int i = 0; i < 4; ++i) {
        pthread_t worker;
        pthread_create(&worker, NULL, worker_routine, (void *) &context);
    }

    zmq_device(ZMQ_QUEUE, clients, workers);

    return 0;
}