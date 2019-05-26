#pragma once

#include "temp_sensor.hpp"

#include <iostream>
#include <chrono>
#include <exception>
#include <thread>
#include <mutex>

static constexpr char ONE_WIRE_DIRECTORY[]{"/sys/bus/w1/devices/"};

class DS18B20 : public temp_sensor
{
    public:

    DS18B20(const gpio_t & pin_id, const std::string & device_path = ONE_WIRE_DIRECTORY);
    virtual ~DS18B20() = default;

    virtual temp_t read_temp() override;

    private:

    DS18B20() = default;

    std::string sensor_id;
    uint16_t last_temp_reading;
    timepoint_t last_read_time;

    std::string device_path;
};
