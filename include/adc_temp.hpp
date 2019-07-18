#pragma once

#include "temp_sensor.hpp"

#include <iostream>
#include <chrono>
#include <exception>
#include <array>

// read thermistor sensor via the MCP3004 ADC SPI channel
class adc_temp : public temp_sensor
{
    public:

    adc_temp();
    virtual ~adc_temp();

    unsigned get_adc_reading() const;
    virtual temp_t read_temp() override;

    private:

    int m_spi_handle{-1};
    int m_cs_gpio{-1};

    void set_cs_pin(bool) const;
};
