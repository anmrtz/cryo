#pragma once


#include "defs.hpp"


// pure virtual class used for any temperature sensors
class temp_sensor
{
    public:

    virtual ~temp_sensor() = default;

    // add common functions as needed..
    virtual bool init_sensor() = 0;
    virtual temp_t read_temp() = 0;

    protected:

    temp_sensor() = default;

};
