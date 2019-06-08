#pragma once


#include <memory>
#include <chrono>
#include <vector>

#include "defs.hpp"
#include "console_ui.hpp"
#include "DS18B20.hpp"
#include "rpiPWM1.h"


class control_ui;

using pwm_control = rpiPWM1;

class cryo
{
    public:

    cryo(const std::shared_ptr<temp_sensor> &, const std::shared_ptr<pwm_control>);
    ~cryo() = default;

    temp_reading_t get_stored_temp() const;
    void update_stored_temp(const temp_reading_t &);
    void register_ui_observer(const std::shared_ptr<control_ui> &);

    private:

    cryo() = default;

    temp_reading_t m_most_recent_temp{0,std::chrono::steady_clock::now()};

    std::vector<std::shared_ptr<control_ui>> m_active_ifaces; // observers
    std::shared_ptr<pwm_control> m_pwm_controller;
    std::shared_ptr<temp_sensor> m_temp_sensor;
};
