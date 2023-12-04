#include <SFML/Window.hpp>
#include "Player.h"
#include "Timeline.h"
#include <zmq.hpp>
#include <unordered_map>
#include <vector>
#include <memory>
#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include <mutex>

#define SEND zmq::send_flags::none
#define REPLY zmq::recv_flags::none

//map of client ids matching to a player
std::unordered_map<int, std::shared_ptr<Player>> playerList;
int nextId = 1;
std::unordered_map<int, float> playerTimes; //map of player times for activity

Timeline global(nullptr, 64);

std::mutex *mutex;

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
                mutex->lock();
                playerList.erase(p.first);
                playerTimes.erase(p.first);
                mutex->unlock();
            }
        }

        zmq::message_t reply;
        socket.recv(reply, REPLY);
        std::string message = reply.to_string(); //get the message sent(1)

        //initailize the client by giving it an unique id
        if(message == "init") {
            //give the client it's unique id
            std::string str = std::to_string(nextId);
            zmq::message_t idReply(str.size());
            memcpy(idReply.data(), str.data(), str.size());
            socket.send(idReply, SEND);

            //add player and client id to the map and increment nextId for next client
            mutex->lock();
            if(playerList.find(nextId) == playerList.end()) {
                playerList.insert(std::make_pair(nextId, std::make_shared<Player>(sf::Vector2f(200.f, 550.f), sf::Vector2f(28.f, 62.f))));
                playerList.at(nextId)->initTexture("textures/bird.png", 9, 10, sf::Vector2i(0, 0), sf::Vector2i(0, 0), 0, 0, 0);
                playerTimes[nextId] = global.getTime();
            }
            ++nextId;
            mutex->unlock();
            std::cout << "connect" << std::endl;
        }
        else {
            if(message == "getTime") {
                float time = global.getTime(); //get the time and convert it to a string
                std::string str = std::to_string(time);

                //send it over to the client
                zmq::message_t response(str.size());
                memcpy(response.data(), str.data(), str.size());
                socket.send(response, SEND);
            }
            else if(message.find("disconnect") != std::string::npos) {
                int exitId;
                sscanf(message.c_str() + 11, "%d", &exitId); //read in the id
                //std::cout << message << std::endl;
                //std::cout << exitId << std::endl;
                mutex->lock();
                playerList.erase(exitId);
                playerTimes.erase(exitId);
                mutex->unlock();

                std::string tempMess = "exitNow";
                zmq::message_t tempMessSend(tempMess.size());
                memcpy(tempMessSend.data(), tempMess.data(), tempMess.size());
                socket.send(tempMessSend, SEND);
            }
            else if(message.find("active") != std::string::npos) { //make sure player is active
                float connectId;
                sscanf(message.c_str() + 7, "%f", &connectId);

                mutex->lock();
                playerTimes[connectId] = global.getTime();
                mutex->unlock();

                std::string tempMess = "activeNow";
                zmq::message_t tempMessSend(tempMess.size());
                memcpy(tempMessSend.data(), tempMess.data(), tempMess.size());
                socket.send(tempMessSend, SEND);
            }
        }
    }
    return(NULL);
}

int main() {
    std::mutex m;
    mutex = &m;

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