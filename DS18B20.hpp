#pragma once

#include "temp_sensor.hpp"

#include <iostream>
#include <chrono>
#include <exception>
#include <thread>
#include <mutex>
#include <fstream>

static constexpr char ONE_WIRE_DIRECTORY[]{"/sys/bus/w1/devices/"};

class DS18B20 : public temp_sensor
{
    public:

    explicit DS18B20(const std::string & w1_devices_path = ONE_WIRE_DIRECTORY);
    virtual ~DS18B20() = default;

    virtual temp_t read_temp() override;

    private:

    DS18B20() = default;
    
    static uint16_t parse_temp(const std::string &);

    const std::string m_device_path;
};
