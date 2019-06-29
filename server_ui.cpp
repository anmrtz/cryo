#include "server_ui.hpp"
#include <zmq.hpp>
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

// this is used to set or read the process-wide termination flag
extern std::atomic_bool terminate_flag;

server_ui::server_ui(const std::shared_ptr<cryo_control> & cryo_ptr) :
    control_ui(cryo_ptr)
{}

void server_ui::task_loop()
{
    
    zmq::socket_t socket = initialize_socket();

    zmq::message_t request;
    int target, power, recv_ts;
    std::string type;

    while (true) {

        // Wait for recv msg and parse
        try {
            socket.recv(&request);
        } catch(zmq::error_t& e) {
            // Maybe interrupt for sending status?
        }

        
        std::string message = std::string(
            static_cast<char*>(request.data()), request.size());
        
        parse_message(socket, message, type, target, power, recv_ts);

        time_t ts = time(0);
        std::string s = std::to_string((int)ts);
        
        send_message(socket, s)
        
    }
}

zmq::socket_t server_ui::initialize_socket() const {
    zmq::context_t context(1);
    zmq::socket_t socket (context, ZMQ_REP);
    try {
        socket.bind("ipc://otter_service");
    } catch(zmq::error_t& e) {
        std::cout << "Unable to bind to socket" << std::endl;
    }
    return socket;
}

void server_ui::parse_message(zmq::socket_t socket, std::string msg,
                              std::string &type, int &target, int &power, 
                              int &recv_ts) {
    try {

        auto j = json::parse(message.c_str());
        type = j["type"];
        target = j["target"];
        power = j["power"];
        recv_ts = j["timestamp"];

    } catch(json::parse_error& e) {
        // std::cout << "Error: "  << e.what() << std::endl;
    }
}

void server_ui::send_message(zmq::socket_t socket, std::string msg) {
    std::string msgToClient(s);
        zmq::message_t reply(msgToClient.size());
        memcpy((void *) reply.data(), (msgToClient.c_str()), msgToClient.size());
        socket.send(reply);
}