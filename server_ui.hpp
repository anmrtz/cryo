#pragma once

#include <map>
#include <string>
#include <memory>

#include <zmq.hpp>

#include "defs.hpp"
#include "control_ui.hpp"

class server_ui : public control_ui
{
    public:

    explicit server_ui(const std::shared_ptr<cryo_control> &);
    virtual ~server_ui() = default;

    void task_loop();                                                                       // runs the ui
    void send_message(zmq::socket_t &, const std::string &);                                          // send message to socket

    protected:

    server_ui() = default;

    void process_gui_command(zmq::message_t &);
    void send_cryo_status();

    zmq::context_t m_zmq_context;

    zmq::socket_t m_zmq_from_gui; // commands from the GUI process(es)
    zmq::socket_t m_zmq_to_gui; // status updates back to the GUI process(es)
};
