#include <vector>
#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <atomic>

#include "cryo_control.hpp"

extern std::atomic_bool terminate_flag;

cryo_control::cryo_control(const std::shared_ptr<temp_sensor> & temp_sensor_ptr, const std::shared_ptr<pwm_control> & pwm_controller_ptr) :
    m_pwm_controller(pwm_controller_ptr),
    m_temp_sensor(temp_sensor_ptr)
{}

void cryo_control::register_ui_observer(const std::shared_ptr<control_ui> & ui_ptr)
{
    m_active_ifaces.push_back(ui_ptr);
}

void cryo_control::update_temp_setting(const temp_t & temp)
{
    m_set_temp = temp;
}

void cryo_control::update_duty_setting(const duty_t & duty)
{
    m_set_duty = duty;
}
