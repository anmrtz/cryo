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

#include "defs.hpp"

#ifndef _WIN32
    #include <unistd.h>
#else
    #include <windows.h>
    #define sleep(n)    Sleep(n)
#endif

using json = nlohmann::json;

int main () {
    
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_PULL);
    socket.connect("tcp://localhost:5556");

    
    zmq::message_t request;
    temp_t target;
    uint64_t recv_ts;
    std::string type;
    duty_t duty;
    bool cooling_active;

    while (true) {

        socket.recv(&request);
        std::string message = std::string(static_cast<char*>(request.data()),request.size());
        std::cout << message << std::endl;

        try {

            auto j = json::parse(message.c_str());
            std::cout << "JSON dump: " << j.dump() << '\n';

            j.at("type").get_to(type);
            j.at("curr_temp").get_to(target);
            j.at("duty").get_to(duty);
            j.at("timestamp").get_to(recv_ts);
            j.at("cooling_on").get_to(cooling_active);

        } catch(json::parse_error& e) {
            std::cout << "Error: "  << e.what() << std::endl;
        }

        std::cout << "Type: " << type <<'\n'
        << "Temp: " << target <<'\n'
        << "Duty: " << duty <<'\n'
        << "Timestamp: " << recv_ts <<'\n'
        << "Cooling active: " << cooling_active << '\n';
        

        //time_t ts = time(0);
        //std::string s = std::to_string((int)ts);
        
        // Do some stuff
        sleep(1);

        
        //std::string msgToClient(s);
        //zmq::message_t reply(msgToClient.size());
        //memcpy((void *) reply.data(), (msgToClient.c_str()), msgToClient.size());
        //socket.send(reply);
    }
    return 0;
}