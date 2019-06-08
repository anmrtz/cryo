#include <vector>
#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <atomic>

#include <signal.h>

#include "cryo.hpp"

//#define LOG_OUTPUT // output cryo module logs directly to std::out (disables ncurses console)

std::atomic_bool terminate_flag{false}; // TODO: remove this

// need to set SIGNAL handlers..
void sig_handler(int s)
{
    terminate_flag = true;
}

void cryo::update_temp_setting(const temp_t & temp)
{
    m_set_temp = temp;
}

void cryo::update_duty_cycle(const duty_t & duty)
{
    m_set_duty = duty;
}

// create and detach control interface thread(s) and start main control loop
int main(int argc, char ** argv)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
    
    DS18B20 temp_sensor(ONE_WIRE_DIRECTORY);
/*    
    console_ui ui();

#ifdef LOG_OUTPUT
    std::cout << "Starting log output...\n";
    auto last_reading = std::chrono::steady_clock::now();
#else
    auto console_thread = std::async(std::launch::async, &console_ui::console_task, std::ref(ui));
#endif
    while (!terminate_flag.load())
    {
        auto temp_read_thread = std::async(&DS18B20::read_temp, std::ref(temp_sensor));
        const temp_t temp_reading{temp_read_thread.get()};

#ifdef LOG_OUTPUT
        std::cout << "Temp reading:  " << temp_reading << " | Read delay (ms): " << 
        std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::steady_clock::now() - last_reading).count() << '\n';
        last_reading = std::chrono::steady_clock::now();
#else
        ui.update_sensor_reading(std::make_pair("DS18B20",
            std::make_pair(temp_reading,std::chrono::steady_clock::now())));
#endif

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

#ifndef LOG_OUTPUT
    console_thread.get();
#endif
    std::cout << "cryo process terminated\n" << std::flush;
*/
    return 0;
}
