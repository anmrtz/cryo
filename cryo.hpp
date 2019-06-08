#pragma once

#include <atomic>
#include <memory>
#include <chrono>
#include <vector>

#include "defs.hpp"
#include "DS18B20.hpp"
#include "rpiPWM1.h"
#include "console_ui.hpp"

class control_ui;

using pwm_control = rpiPWM1;

class cryo
{
    public:

    cryo(const std::shared_ptr<temp_sensor> &, const std::shared_ptr<pwm_control>);
    ~cryo() = default;

    void register_ui_observer(const std::shared_ptr<control_ui> &);

    temp_reading_t get_stored_temp() const;
    duty_t get_stored_duty() const;

    void update_temp_setting(const temp_t & temp);
    void update_duty_cycle(const duty_t & duty);

    private:

    cryo() = default;

    temp_reading_t m_most_recent_temp{0,std::chrono::steady_clock::now()};

    std::vector<std::shared_ptr<control_ui>> m_active_ifaces; // observers
    std::shared_ptr<pwm_control> m_pwm_controller;
    std::shared_ptr<temp_sensor> m_temp_sensor;

    void update_stored_temp(const temp_reading_t &);

    std::atomic<temp_t> m_set_temp;
    std::atomic<duty_t> m_set_duty;
};
