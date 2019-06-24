#pragma once

#include <map>
#include <string>
#include <mutex>
#include <memory>

// include json for modern c++
#include "control_ui.hpp"

class server_ui : public control_ui
{
    public:

    explicit server_ui(const std::shared_ptr<cryo_control> &);
    virtual ~server_ui() = default;

    void task_loop(); // runs the ui

    protected:

    server_ui() = default;
};
