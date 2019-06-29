#pragma once

#include <map>
#include <string>
#include <mutex>
#include <memory>

// include json for modern c++
#include "control_ui.hpp"

class server_ui : public control_ui
{
    public:

    explicit server_ui(const std::shared_ptr<cryo_control> &);
    virtual ~server_ui() = default;

    void task_loop();                                                               // runs the ui
    zmq::socket_t initialize_socket() const;                                        // Initializes the ipc bound socket
    void parse_message(zmq::socket_t, std::string, std::string, int*, int*, int*);  // parse an incomming message
    void send_message(zmq::socket_t, std::string);                                  // send message to socket

    protected:

    server_ui() = default;
};