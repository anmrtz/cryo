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
    int target, recv_ts;
    std::string type, power;

    while (true) {

        // Wait for recv msg and parse
        try {
            socket.recv(&request);
        } catch(zmq::error_t& e) {
            // Maybe interrupt for sending status?
            std::cout << "Error receiving message" << std::endl;
        }

        std::string message = std::string(
            static_cast<char*>(request.data()), request.size());
        
        parse_message(socket, message, type, target, power, recv_ts);

        if (type == "power"){
            if (power == "on") {
                std::cout << "Turning on power pin...\n";
                get_cryo_ptr()->set_power_enable(true);
            }
            else if (power == "off") {
                std::cout << "Turning off power pin...\n";
                get_cryo_ptr()->set_power_enable(false);
            }
        }
        
        else if (type == "temp") {
            if (!valid_temp(target)) continue;
            std::cout << "Setting cryo target temperature to " << target << " degrees Celsius\n";
            get_cryo_ptr()->update_temp_setting(target);
        }
        

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
                              std::string &type, std::string &power, int &target, 
                              int &recv_ts) {
    
    auto j = json::parse(msg.c_str());
    type = j["type"];
    target = j["target"];
    power = j["power"];
    recv_ts = j["timestamp"];
}

void server_ui::send_message(zmq::socket_t socket, std::string msg) {
    std::string msgToSend(msg);
    zmq::message_t message(msgToSend.size());
    memcpy((void *) message.data(), (msgToSend.c_str()), msgToSend.size());
    socket.send(message);
}

static inline bool valid_temp(int temp)
{
    if (temp < TEMP_SETTING_MIN || temp > TEMP_SETTING_MAX)
    {
        std::cout << "ERROR: Specified LED must be integer from " <<
            TEMP_SETTING_MIN << " to " << TEMP_SETTING_MAX << '\n';
        return false;
    }
    return true;
}