#pragma once

#include <map>
#include <string>
#include <mutex>

#include "control_ui.hpp"

class console_ui : public control_ui
{
    public:

    console_ui() = default;
    virtual ~console_ui() = default;

    void console_task(); // runs the console

    // updates the values displayed by the console
    void update_sensor_reading(const sensor_state_t &);
    void update_temp_setting(const temp_reading_t &);

    // send user-specified temperature settings to the cryo controller
    void send_temp_setting_to_contoller();

    private:

    temp_t m_temp_setting{30};
    std::map<std::string,temp_reading_t> m_sensor_states; // sensor_id, reading
    std::map<std::string,timepoint_t> m_last_updates; // sensor_id, last update time

    std::mutex m_temp_lock;

    //std::weak_ptr<cryo_controller> cryo;
};
