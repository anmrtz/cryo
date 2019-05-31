#include "ncurses_ui.hpp"

#include <atomic>
#include <sstream>
#include <iomanip>

#include <curses.h>
#include <menu.h>

extern std::atomic_bool terminate_flag;

void ncurses_ui::update_sensor_reading(const sensor_state_t & state)
{
    std::lock_guard<std::mutex> lock(m_temp_lock);

    if (!m_last_updates.count(state.first))
        m_last_updates[state.first] = state.second.second;
    else
        m_last_updates[state.first] = m_sensor_states.at(state.first).second;

    m_sensor_states[state.first] = state.second;
}

void ncurses_ui::console_loop()
{
    WINDOW * console{initscr()};
    if (!console)
        throw std::runtime_error("ncurses_ui error: initscr failed\n");
    noecho();

    unsigned X_OFFSET{10};


    while (!terminate_flag.load())
    {
        werase(console);
        unsigned Y_OFFSET{5};
        mvwaddstr(console,Y_OFFSET++, X_OFFSET, "Temperature readings:");

        {
            std::lock_guard<std::mutex> lock(m_temp_lock);
            if (m_sensor_states.empty())
            {
                mvwprintw(console,Y_OFFSET, X_OFFSET, "--- NO TEMPERATURE SENSORS DETECTED ---");
                wrefresh(console);
            }
            else
            {
                for (const auto & reading : m_sensor_states)
                {
                    const auto sensor_id{reading.first};
                    const float temp_deg_c{reading.second.first/1000.0f};
                    const auto sensor_timestamp{reading.second.second};

                    std::stringstream ss;
                    ss.precision(3);
                    ss << sensor_id << " : "  << temp_deg_c << " °C " << 
                        "| Read delay: " << std::chrono::duration_cast<std::chrono::milliseconds>
                        (sensor_timestamp - m_last_updates.at(sensor_id)).count() << " ms";

                    mvwprintw(console,Y_OFFSET++, X_OFFSET, "%s", ss.str().c_str());

                    wrefresh(console);
                }
            }
        }

        std::this_thread::sleep_for(CONSOLE_UPDATE_INTERVAL);
    }

    delwin(console);
    endwin();
    refresh();

    std::cout << "Exiting console loop...\n" << std::flush;
}
