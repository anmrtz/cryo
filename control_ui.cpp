#include "control_ui.hpp"

explicit control_ui::control_ui(const std::shared_ptr<cryo> & cryo_controller) :
    m_cryo_controller(cryo_controller)
{}

// add common functions as needed..
virtual void control_ui::send_temp_setting_to_control(const temp_t & temp) const
{
    auto cryo_ptr = m_cryo_controller.lock();
    if (!cryo_ptr)
        throw std::runtime_error("control_ui error: null pointer to cryo module\n");
    cryo_ptr->update_temp_setting(temp);
}

virtual void control_ui::send_duty_cycle_to_control(const duty_t & duty) const
{
    auto cryo_ptr = m_cryo_controller.lock();
    if (!cryo_ptr)
        throw std::runtime_error("control_ui error: null pointer to cryo module\n");
    cryo_ptr->update_duty_cycle(duty);
}
