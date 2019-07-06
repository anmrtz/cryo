#include <signal.h>
#include <thread>
#include <future>
#include <iostream>

#include "defs.hpp"
#include "cryo_control.hpp"
#include "console_ui.hpp"
#include "adc_temp.hpp"

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

    std::shared_ptr<cryo_control> cryo = std::make_shared<cryo_control>(
            std::make_shared<adc_temp>(), std::make_shared<pwm_control>(PWM_GPIO_PIN,PWM_FREQ_HZ));
    std::shared_ptr<console_ui> console = std::make_shared<console_ui>(cryo);
    cryo->register_ui_observer(console);

    auto console_thread = std::async(std::launch::async, &console_ui::console_task, std::ref(*console));

    cryo->control_loop();

    console_thread.get();

    std::cout << "cryo process terminated\n" << std::flush;


    return 0;
}
