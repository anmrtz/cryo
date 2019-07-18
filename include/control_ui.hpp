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

    protected:

    std::weak_ptr<cryo_control> m_cryo_controller;

    control_ui() = default;

    virtual std::shared_ptr<cryo_control> get_cryo_ptr() const;
};
