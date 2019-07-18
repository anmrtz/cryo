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
    duty_t get_current_duty() const;

    bool update_temp_setting(const temp_t & temp);

    void set_cooling_active(bool);
    bool is_cooling_active() const;

    duration_t get_active_duration() const;

    void control_loop(); // main temperature control loop

    private:

    cryo_control() = default;

    temp_t read_temp_sensor();

    void send_temp_reading_to_uis() const;
    void send_temp_setting_to_uis() const;

    std::vector<std::weak_ptr<control_ui>> m_active_ifaces; // observers
    std::shared_ptr<pwm_control> m_pwm_control;
    std::shared_ptr<temp_sensor> m_temp_sensor;

    std::atomic<temp_reading_t> m_last_temp_reading;
    std::atomic<temp_t> m_temp_setting;

    std::atomic_bool m_power_enabled{false};

    std::atomic<std::chrono::steady_clock::time_point> m_start_time{std::chrono::steady_clock::now()};
    std::atomic<std::chrono::steady_clock::time_point> m_stop_time{std::chrono::steady_clock::now()};
};
