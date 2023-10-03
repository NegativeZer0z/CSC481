#include <iostream>
#include <zmq.hpp>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>

int main() {

    //connect to socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    socket.bind("tcp://*:5555");

    //list of ids and clients to keep track of
    std::unordered_map<std::string, int> clients;
    int next_id = 1;

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
            if (clients.find(client_id) == clients.end()) {
                clients[client_id] = 1;
            }

            //std::string response = "Client " + client_id + ": Iteration " + std::to_string(clients[client_id]);
            std::string response;
            int idx = 0;
            for(auto i : clients) {
                response += "Client ";
                response += i.first;
                response += ": Iteration ";
                response += std::to_string(i.second);
                idx += 1;
                if(idx != clients.size()) {
                    response += "\n";
                }
            }

            zmq::message_t zmq_response(response.size());
            memcpy(zmq_response.data(), response.c_str(), response.size());

            socket.send(zmq_response, zmq::send_flags::none);

            clients[client_id]++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    return 0;
}
