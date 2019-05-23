#pragma once

#include "temp_sensor.hpp"

#include <iostream>
#include <chrono>
#include <exception>
#include <thread>
#include <mutex>

class DS18B20 : public temp_sensor
{
    public:

    DS18B20() = default;
    virtual ~DS18B20() = default;

    private:

};
