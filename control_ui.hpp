#pragma once

#include "defs.hpp"
#include "cryo_control.hpp"

class cryo_control;

// virtual class used for any cryo control user interfaces (ncurses, webserver..)
class control_ui
{
    public:

    explicit control_ui(const std::shared_ptr<cryo_control> & cryo_controller);
    virtual ~control_ui() = default;

    // add common functions as needed..
    virtual void send_temp_setting_to_control(const temp_t & temp) const;
    virtual void send_duty_setting_to_control(const duty_t & duty) const;

    protected:

    control_ui() = default;

    std::weak_ptr<cryo_control> m_cryo_controller;
};
