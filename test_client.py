//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <time.h>
#include <string>
#include <unistd.h>
#include "json.hpp"

#ifndef _WIN32
    #include <unistd.h>
#else
    #include <windows.h>
    #define sleep(n)    Sleep(n)
#endif

using json = nlohmann::json;

int main () {
    
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REP);
    socket.bind("ipc://otter_service");

    
    zmq::message_t request;
    uint32_t target;
    int recv_ts;
    std::string type, power;

    while (true) {

        socket.recv(&request);
        std::string message = std::string(static_cast<char*>(request.data()),request.size());
        std::cout << message << std::endl;

        try {

            auto j = json::parse(message.c_str());

            type = j["type"];
            target = j["target"];
            power = j["power"];
            recv_ts = j["timestamp"];

        } catch(json::parse_error& e) {
            std::cout << "Error: "  << e.what() << std::endl;
        }
        

        time_t ts = time(0);
        std::string s = std::to_string((int)ts);
        
        // Do some stuff
        sleep(1);

        
        std::string msgToClient(s);
        zmq::message_t reply(msgToClient.size());
        memcpy((void *) reply.data(), (msgToClient.c_str()), msgToClient.size());
        socket.send(reply);
    }
    return 0;
}