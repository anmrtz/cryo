#pragma once

#include "temp_sensor.hpp"


class mock_temp_sensor : public temp_sensor
{
    public:

    mock_temp_sensor() = default;
    virtual ~mock_temp_sensor() = default;

    virtual temp_t read_temp() override
    {
        return TEMP_SETTING_MAX;
    }

    private:

};
