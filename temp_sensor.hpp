#pragma once

// pure virtual class used for any temperature sensors
class temp_sensor
{
    public:

    virtual ~temp_sensor() = default;

    // add common functions as needed..

    protected:

    temp_sensor() = default;

};
