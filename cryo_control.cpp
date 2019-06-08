#include <vector>
#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <atomic>

#include "cryo.hpp"

extern std::atomic_bool terminate_flag;


void cryo::register_ui_observer(const std::shared_ptr<control_ui> & ui_ptr)
{
    m_active_ifaces.push_back(ui_ptr);
}

void cryo::update_temp_setting(const temp_t & temp)
{
    m_set_temp = temp;
}

void cryo::update_duty_setting(const duty_t & duty)
{
    m_set_duty = duty;
}
