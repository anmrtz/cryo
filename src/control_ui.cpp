#include "control_ui.hpp"

control_ui::control_ui(const std::shared_ptr<cryo_control> & cryo_controller) :
    m_cryo_controller(cryo_controller)
{}

std::shared_ptr<cryo_control> control_ui::get_cryo_ptr() const
{
    auto cryo_ptr = m_cryo_controller.lock();
    if (!cryo_ptr)
        throw std::runtime_error("control_ui error: null pointer to cryo module\n");
    return cryo_ptr;
}
