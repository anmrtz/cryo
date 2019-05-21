#include <memory>
#include <iostream>
#include <thread>
#include <future>

#include "ncurses_ui.hpp"

// need to set SIGNAL handlers..

// create and detach control interface thread(s) and start main control loop
int main(int argc, char ** argv)
{
    ncurses_ui ui;



    std::cout << "cryo process terminated\n";
    return 0;
}
