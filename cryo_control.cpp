#include <vector>
#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <atomic>

#include "cryo_control.hpp"

extern std::atomic_bool terminate_flag;

cryo_control::cryo_control(const std::shared_ptr<temp_sensor> & temp_sensor_ptr, const std::shared_ptr<pwm_control> & pwm_controller_ptr) :
    m_pwm_control(pwm_controller_ptr),
    m_temp_sensor(temp_sensor_ptr)
{}

cryo_control::~cryo_control()
{}

void cryo_control::control_loop()
{
    duty_t last_duty_setting{m_duty_setting};
    temp_t last_temp_setting{m_temp_setting};

    bool last_pwm_state{m_pwm_enabled};

    while(!terminate_flag)
    {
        temp_reading_t temp_reading;
        temp_reading.temp = m_temp_sensor->read_temp();
        temp_reading.time = std::chrono::steady_clock::now().time_since_epoch();
        m_last_temp_reading = temp_reading;

        if (m_temp_setting != last_temp_setting)
        {
            last_temp_setting = m_temp_setting;
        }

        if (m_duty_setting != last_duty_setting)
        {
            last_duty_setting = m_duty_setting;
            m_pwm_control->set_duty(m_duty_setting);
        }

        if (m_pwm_enabled != last_pwm_state)
        {            
            last_pwm_state = m_pwm_enabled;
            if (m_pwm_enabled)
                m_pwm_control->pwm_on();
            else
                m_pwm_control->pwm_off();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void cryo_control::register_ui_observer(const std::shared_ptr<control_ui> & ui_ptr)
{
    m_active_ifaces.push_back(ui_ptr);
}

void cryo_control::update_temp_setting(const temp_t & temp)
{
    m_temp_setting = temp;
}

void cryo_control::update_duty_setting(const duty_t & duty)
{
    m_duty_setting = duty;
}

temp_reading_t cryo_control::get_last_temp_reading() const
{
    return m_last_temp_reading;
}

temp_t cryo_control::get_temp_setting() const
{
    return m_temp_setting;
}

duty_t cryo_control::get_duty_setting() const
{
    return m_duty_setting;
}

duty_t cryo_control::get_current_duty() const
{
    return m_pwm_control->get_duty();
}

temp_t cryo_control::read_temp_sensor()
{
    return m_temp_sensor->read_temp();
}

void cryo_control::set_pwm_enable(bool is_enabled)
{
    m_pwm_enabled = is_enabled;
}

bool cryo_control::is_pwm_enabled() const
{
    return m_pwm_enabled;
}

void cryo_control::set_power_enable(bool is_enabled)
{
    m_power_enabled = is_enabled;
}

bool cryo_control::is_power_enabled() const
{
    return m_power_enabled;
}
