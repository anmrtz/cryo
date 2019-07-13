#pragma once

#include <chrono>
#include <utility>
#include <string>

using read_time_t = std::chrono::steady_clock::duration; // time since epoch
using temp_t = uint32_t; // temperature in degrees celsius * 1000
struct temp_reading_t
{
    temp_t temp;
    read_time_t time;
};
static_assert(std::is_pod<temp_reading_t>::value); // verify std::atomic compatibility

using duty_t = uint32_t; // duty cycle as percentage from 0 to 100
using freq_t = uint32_t; // frequency expressed in Hz

using gpio_t = uint32_t;

constexpr temp_t TEMP_SETTING_MIN{0};
constexpr temp_t TEMP_SETTING_MAX{20'000};
constexpr temp_t INVALID_TEMP_READING{0};

constexpr duty_t DUTY_CYCLE_MIN{0};
constexpr duty_t DUTY_CYCLE_MAX{100};
constexpr duty_t DUTY_CYCLE_ACTIVE{100};
static_assert(DUTY_CYCLE_ACTIVE > DUTY_CYCLE_MIN && DUTY_CYCLE_ACTIVE <= DUTY_CYCLE_MAX);

constexpr gpio_t PWM_GPIO_PIN{18};
constexpr freq_t PWM_FREQ_HZ{6000};
