#include "server_ui.hpp"

#include <zmq.hpp>

#include "json.hpp"
using json = nlohmann::json;


// this is used to set or read the process-wide termination flag
extern std::atomic_bool terminate_flag;

server_ui::server_ui(const std::shared_ptr<cryo_control> & cryo_ptr) :
    control_ui(cryo_ptr)
{}

void server_ui::task_loop()
{
    // add server functionality here
    
    zmq_init(1);
}