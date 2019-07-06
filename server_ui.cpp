#include "server_ui.hpp"
#include "cryo_control.hpp"
#include <zmq.hpp>
#include "json.hpp"
#include <iostream>
#include <thread>

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
    temp_t target;
    int recv_ts;
    std::string type, power;

    while (!terminate_flag) {

        // Wait for recv msg and parse
        try {
            socket.recv(&request);
        } catch(zmq::error_t& e) {
            // Maybe interrupt for sending status?
            std::cout << "Error receiving message" << std::endl;
        }

        std::string message = std::string(
            static_cast<char*>(request.data()), request.size());
        
        parse_message(socket, message, type, power, target, recv_ts);

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
            if (!valid_temp(target) || get_cryo_ptr()->get_temp_setting() == target) continue;
            std::cout << "Setting cryo target temperature to " << target << " degrees Celsius\n";
            get_cryo_ptr()->update_temp_setting(target);
        }
        

        time_t ts = time(0);
        std::string s = std::to_string((int)ts);
        
        send_message(socket, s);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

void server_ui::parse_message(zmq::socket_t & socket, std::string msg,
                              std::string & type, std::string & power, temp_t & target, 
                              int & recv_ts) {
    
    const auto j = json::parse(msg);
    j.at("type").get_to(type);
    j.at("target").get_to(target);
    j.at("power").get_to(power);
    j.at("timestamp").get_to(recv_ts);
}

void server_ui::send_message(zmq::socket_t & socket, std::string msg) {
    std::string msgToSend(msg);
    zmq::message_t message(msgToSend.size());
    memcpy((void *) message.data(), (msgToSend.c_str()), msgToSend.size());
    socket.send(message);
}

inline bool server_ui::valid_temp(temp_t temp)
{
    if (temp < TEMP_SETTING_MIN || temp > TEMP_SETTING_MAX)
    {
        std::cout << "ERROR: Specified LED must be integer from " <<
            TEMP_SETTING_MIN << " to " << TEMP_SETTING_MAX << '\n';
        return false;
    }
    return true;
}
