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

    // updates the values displayed by the console
    void update_sensor_reading(const sensor_state_t &);
    void update_temp_setting(const temp_reading_t &);

    protected:

    console_ui() = default;

    temp_t m_temp_setting{20};
    std::map<std::string,temp_reading_t> m_sensor_states; // sensor_id, reading
    std::map<std::string,timepoint_t> m_last_updates; // sensor_id, last update time

    std::mutex m_temp_lock;
};
