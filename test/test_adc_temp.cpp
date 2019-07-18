#include <signal.h>
#include <atomic>
#include <chrono>
#include <thread>

#include <pigpio.h>

#include "defs.hpp"
#include "adc_temp.hpp"

std::atomic_bool terminate_flag{false};

constexpr std::chrono::milliseconds TEMP_READ_INTERVAL{500};

void sig_handler(int s)
{
    terminate_flag = true;
}

int main()
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
    gpioInitialise();
    if (gpioSetSignalFunc(2,sig_handler))
    {
        std::cout << "Could not set gpio signal handler\n";
        return 1;
    }

    std::cout << "ADC temperature sensor test:\n\n";

    adc_temp temp_sensor;
    while (!terminate_flag)
    {
        const auto reading = temp_sensor.get_adc_reading();

        std::cout << "ADC reading: " << reading << "; temp value: " << temp_sensor.read_temp() << '\n';

        std::this_thread::sleep_for(TEMP_READ_INTERVAL);
    }

    std::cout << "\nExiting test program...\n";
    
    return 0;
}
