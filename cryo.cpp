#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <atomic>

#include <signal.h>

#include "defs.hpp"
#include "ncurses_ui.hpp"

std::atomic_bool terminate_flag{false}; // TODO: remove this

// need to set SIGNAL handlers..
void sig_handler(int s)
{
    terminate_flag = true;
}

// create and detach control interface thread(s) and start main control loop
int main(int argc, char ** argv)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
    
    ncurses_ui ui;

    auto console_thread = std::async(std::launch::async, &ncurses_ui::console_loop, std::ref(ui));

    console_thread.get();
    std::cout << "cryo process terminated\n" << std::flush;

    return 0;
}
