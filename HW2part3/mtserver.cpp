/*
    Multithreaded Hello World server in C
*/
#include <pthread.h>
#include <unistd.h>
#include <cassert>
#include <string>
#include <iostream>
#include <zmq.hpp>
#include <unordered_map>
#include <thread>

std::unordered_map<std::string, int> client_last_iteration;
int next_id = 1;

void *worker_routine (void *arg)
{
    zmq::context_t *context = (zmq::context_t *) arg;

    zmq::socket_t socket (*context, ZMQ_REP);
    socket.connect ("inproc://workers");

    while (true) {
        zmq::message_t message;
        socket.recv(message, zmq::recv_flags::none);
        std::string client_id = message.to_string();

        if (client_id == "get") { //get message so get the id and return it
            std::string str = std::to_string(next_id);
            zmq::message_t reply(str.size());
            memcpy(reply.data(), str.data(), str.size());
            socket.send(reply, zmq::send_flags::none);
            next_id++;
        } 
        else {
            if (client_last_iteration.find(client_id) == client_last_iteration.end()) {
                client_last_iteration[client_id] = 1;
            }

            //std::string response = "Client " + client_id + ": Iteration " + std::to_string(client_last_iteration[client_id]);
            std::string response;
            int idx = 0;
            for(auto i : client_last_iteration) {
                response += "Client ";
                response += i.first;
                response += ": Iteration ";
                response += std::to_string(i.second);
                idx += 1;
                if(idx != client_last_iteration.size()) {
                    response += "\n";
                }
            }

            zmq::message_t zmq_response(response.size());
            memcpy(zmq_response.data(), response.c_str(), response.size());

            socket.send(zmq_response, zmq::send_flags::none);

            client_last_iteration[client_id]++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return (NULL);
}


int main ()
{
    //  Prepare our context and sockets
    zmq::context_t context (1);
    zmq::socket_t clients (context, ZMQ_ROUTER);
    clients.bind ("tcp://*:5555");
    zmq::socket_t workers (context, ZMQ_DEALER);
    workers.bind ("inproc://workers");

    //  Launch pool of worker threads
    for (int thread_nbr = 0; thread_nbr < 6; thread_nbr++) {
        pthread_t worker;
        pthread_create (&worker, NULL, worker_routine, (void *) &context);
    }
    
    //  Connect work threads to client threads via a queue
    zmq_device(ZMQ_QUEUE, clients, workers);
    return 0;
}
    