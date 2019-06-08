#pragma once

#include "defs.hpp"
#include "cryo.hpp"

class cryo;

// pure virtual class used for any cryo control user interfaces (ncurses, webserver..)
class control_ui
{
    public:

    explicit control_ui(const std::shared_ptr<cryo> & cryo_controller) :
        m_cryo_controller(cryo_controller) {}
    virtual ~control_ui() = default;

    // add common functions as needed..

    protected:

    control_ui() = default;

    std::weak_ptr<cryo> m_cryo_controller;
};
