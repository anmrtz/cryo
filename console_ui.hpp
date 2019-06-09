#pragma once

#include <map>
#include <string>
#include <mutex>
#include <memory>

#include "control_ui.hpp"

class console_ui : public control_ui
{
    public:

    explicit console_ui(const std::shared_ptr<cryo_control> &);
    virtual ~console_ui() = default;

    void console_task(); // runs the console

    protected:

    console_ui() = default;

    void print_status() const;
};
