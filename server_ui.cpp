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
    control_ui(cryo_ptr),
    m_zmq_context(zmq::context_t(1)),
    m_zmq_socket(zmq::socket_t(m_zmq_context, ZMQ_REP))
{
    try 
    {
        m_zmq_socket.bind("tcp://*:5555");
    } 
    catch(zmq::error_t& e) 
    {
        throw std::runtime_error("server_ui::server_ui error - failed to initialize ZMQ\n");
    }
    std::cout << "ZMQ initialized\n";
}

void server_ui::task_loop()
{
    zmq::message_t request;
    temp_t target;
    int recv_ts;
    std::string type, power;

    while (!terminate_flag) 
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        try 
        {
            int err = m_zmq_socket.recv(&request, ZMQ_NOBLOCK);
            if (err)
                continue;
        } 
        catch(zmq::error_t& e) 
        {
            // Maybe interrupt for sending status?
            throw std::runtime_error("server_ui::task_loop error - ZMQ recv error\n");
        }

        std::string message = std::string(
            static_cast<char*>(request.data()), request.size());
        
        std::cout << "MSG received! Parsing msg [" << message << "]\n";
        parse_message(message, type, power, target, recv_ts);

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
        else if (type == "target") {
            if (!valid_temp(target) || get_cryo_ptr()->get_temp_setting() == target) 
            {
                std::cout << "server_ui::task_loop invalid target temperature received: " << target << '\n';
                continue;
            }
            std::cout << "Setting cryo target temperature to " << target << " degrees Celsius\n";
            get_cryo_ptr()->update_temp_setting(target);
        }
        else
        {
            std::cout << "server_ui::task_loop invalid message type received: " << type << '\n';
        }
        
        time_t ts = time(0);
        std::string s = std::to_string((int)ts);
        
        std::cout << "Sending message..\n";
        send_message(s);
        std::cout << "Message sent\n";
    }
}

void server_ui::parse_message(std::string msg,
                              std::string & type, std::string & power, temp_t & target, 
                              int & recv_ts) 
{
    const auto j = json::parse(msg);
    j.at("type").get_to(type);
    j.at("target").get_to(target);
    j.at("power").get_to(power);
    j.at("timestamp").get_to(recv_ts);
}

void server_ui::send_message(std::string msg)
{
    std::string msgToSend(msg);
    zmq::message_t message(msgToSend.size());
    memcpy((void *) message.data(), (msgToSend.c_str()), msgToSend.size());
    m_zmq_socket.send(message);
}

bool server_ui::valid_temp(temp_t temp)
{
    if (temp < TEMP_SETTING_MIN || temp > TEMP_SETTING_MAX)
    {
        std::cout << "ERROR: Specified LED must be integer from " <<
            TEMP_SETTING_MIN << " to " << TEMP_SETTING_MAX << '\n';
        return false;
    }
    return true;
}
