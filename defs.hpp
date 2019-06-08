#pragma once

#include <chrono>
#include <utility>
#include <string>

using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;
using temp_t = uint32_t; // temperature in degrees celsius * 1000
using temp_reading_t = std::pair<temp_t,timepoint_t>; // temp, timestamp
using sensor_state_t = std::pair<std::string,temp_reading_t>; // sensor_id, temp_reading

using duty_t = uint32_t; // duty cycle as percentage from 0 to 100

using gpio_t = uint32_t;

constexpr temp_t TEMP_SETTING_MIN{0};
constexpr temp_t TEMP_SETTING_MAX{20};
constexpr duty_t DUTY_CYCLE_MIN{0};
constexpr duty_t DUTY_CYCLE_MAX{100};
