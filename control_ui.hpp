#pragma once

#include "defs.hpp"
#include "cryo.hpp"

class cryo;

// virtual class used for any cryo control user interfaces (ncurses, webserver..)
class control_ui
{
    public:

    explicit control_ui(const std::shared_ptr<cryo> & cryo_controller);
    virtual ~control_ui() = default;

    // add common functions as needed..
    virtual void send_temp_setting(const temp_t & temp) const;
    virtual void send_duty_cycle(const duty_t & duty) const;

    protected:

    control_ui() = default;

    std::weak_ptr<cryo> m_cryo_controller;
};
