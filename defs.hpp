#pragma once

#include <chrono>
#include <utility>
#include <string>

using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;
using temp_t = uint16_t; // temperature in degrees celsius * 1000
using temp_reading_t = std::pair<temp_t,timepoint_t>; // temp, timestamp
using sensor_state_t = std::pair<std::string,temp_reading_t>; // sensor_id, temp_reading

using gpio_t = uint32_t;
