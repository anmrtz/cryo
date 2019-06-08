#pragma once

#include <atomic>
#include <memory>
#include <chrono>
#include <vector>

#include "defs.hpp"
#include "control_ui.hpp"
#include "temp_sensor.hpp"
#include "pwm_control.hpp"

class control_ui;

class cryo_control
{
    public:

    cryo_control(const std::shared_ptr<temp_sensor> &, const std::shared_ptr<pwm_control> &);
    ~cryo_control() = default;

    void register_ui_observer(const std::shared_ptr<control_ui> &);

    temp_t get_stored_temp_reading() const;
    temp_t get_stored_temp_setting() const;
    duty_t get_stored_duty() const;

    void update_temp_setting(const temp_t & temp);
    void update_duty_setting(const duty_t & duty);

    void control_loop(); // main temperature control loop

    private:

    cryo_control() = default;

    temp_t m_stored_temp_reading{0};

    std::vector<std::weak_ptr<control_ui>> m_active_ifaces; // observers
    std::shared_ptr<pwm_control> m_pwm_controller;
    std::shared_ptr<temp_sensor> m_temp_sensor;

    void update_temp_reading(const temp_reading_t &);

    std::atomic<temp_t> m_set_temp;
    std::atomic<duty_t> m_set_duty;
};
