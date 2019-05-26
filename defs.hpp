#pragma once

#include <chrono>

using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;
using temp_t = uint16_t; // temperature in degrees celsius * 100
using gpio_t = uint32_t;
