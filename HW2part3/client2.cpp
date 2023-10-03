#include <iostream>
#include <zmq.hpp>
#include <string>
#include <chrono>
#include <thread>

int main(int argc, char* argv[]) {
    int client_id = 0;

    //connect to socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REQ);
    socket.connect("tcp://localhost:5555");

    //send the get message
    std::string id = "get";
    zmq::message_t request(id.size());
    memcpy(request.data(), id.data(), id.size());
    socket.send(request, zmq::send_flags::none);

    //get the id and set it
    zmq::message_t temp;
    socket.recv(temp, zmq::recv_flags::none);
    client_id = std::stoi(temp.to_string());

    while (true) {

        //pass your id and get the corresponding message
        std::string id = std::to_string(client_id);
        zmq::message_t request(id.size());
        memcpy(request.data(), id.data(), id.size());
        socket.send(request, zmq::send_flags::none);

        zmq::message_t response;
        socket.recv(response, zmq::recv_flags::none);
        std::string message = response.to_string();

        std::cout << message << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
