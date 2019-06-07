#include "console_ui.hpp"

#include <atomic>

extern std::atomic_bool terminate_flag;

void console_ui::update_sensor_reading(const sensor_state_t & state)
{
    std::lock_guard<std::mutex> lock(m_temp_lock);

    if (!m_last_updates.count(state.first))
        m_last_updates[state.first] = state.second.second;
    else
        m_last_updates[state.first] = m_sensor_states.at(state.first).second;

    m_sensor_states[state.first] = state.second;
}

void console_ui::console_task()
{

}