#include <atomic>
#include <chrono>

#include "defs.hpp"
#include "adc_temp.hpp"

std::atomic_bool terminate_flag{false};

void sig_handler(int s)
{
    terminate_flag = true;
}

int main()
{
    adc_temp temp_sensor;

    while (!terminate_flag)
    {

    }

    return 0;
}
