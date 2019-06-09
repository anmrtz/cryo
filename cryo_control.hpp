#pragma once

#include <atomic>
#include <memory>
#include <chrono>
#include <vector>
#include <mutex>

#include "defs.hpp"
#include "control_ui.hpp"
#include "temp_sensor.hpp"
#include "pwm_control.hpp"

class control_ui;

class cryo_control
{
    public:

    cryo_control(const std::shared_ptr<temp_sensor> &, const std::shared_ptr<pwm_control> &);
    ~cryo_control();

    void register_ui_observer(const std::shared_ptr<control_ui> &);

    temp_reading_t get_last_temp_reading() const;
    temp_t get_temp_setting() const;
    duty_t get_duty_setting() const;

    void update_temp_setting(const temp_t & temp);
    void update_duty_setting(const duty_t & duty);

    void set_pwm_enable(bool);
    bool is_pwm_enabled() const;

    void set_power_enable(bool);
    bool is_power_enabled() const;

    void control_loop(); // main temperature control loop

    private:

    cryo_control() = default;

    temp_t read_temp_sensor();

    void send_temp_reading_to_uis() const;
    void send_temp_setting_to_uis() const;

    std::vector<std::weak_ptr<control_ui>> m_active_ifaces; // observers
    std::shared_ptr<pwm_control> m_pwm_controller;
    std::shared_ptr<temp_sensor> m_temp_sensor;

    std::atomic<temp_reading_t> m_last_temp_reading;
    std::atomic<temp_t> m_temp_setting;
    std::atomic<duty_t> m_duty_setting;

    std::atomic_bool m_pwm_enabled{false};
    std::atomic_bool m_power_enabled{false};
};
