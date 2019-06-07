#pragma once


#include <memory>
#include <chrono>

#include "defs.hpp"
#include "console_ui.hpp"
#include "DS18B20.hpp"


class cryo
{
    public:

    cryo() = default;
    ~cryo() = default;

    temp_reading_t get_stored_temp() const;

    private:

    temp_reading_t most_recent_temp{0,std::chrono::steady_clock::now()};

};
