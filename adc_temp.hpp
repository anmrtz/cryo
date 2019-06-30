#pragma once

#include "temp_sensor.hpp"

#include <iostream>
#include <chrono>
#include <exception>

class adc_temp : public temp_sensor
{
    public:

    adc_temp() = default;
    virtual ~adc_temp() = default;

    virtual temp_t read_temp() override;

    private:
};
