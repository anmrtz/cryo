#include "ncurses_ui.hpp"

#include <atomic>
#include <sstream>

#include <curses.h>
#include <menu.h>

extern std::atomic_bool terminate_flag;

void ncurses_ui::console_loop()
{
    WINDOW * console{initscr()};
    if (!console)
        throw std::runtime_error("ncurses_ui error: initscr failed\n");
    noecho();

    unsigned X_OFFSET{10};

    mvaddstr(5, X_OFFSET, "Temperature readings:");
    refresh();

    //sensor_temp_readings["aaa"] = std::make_pair(1111,std::chrono::steady_clock::now());
    //sensor_temp_readings["bbb"] = std::make_pair(2222,std::chrono::steady_clock::now());
    //sensor_temp_readings["ccc"] = std::make_pair(3333,std::chrono::steady_clock::now());

    while (!terminate_flag.load())
    {
        unsigned Y_OFFSET{7};

        if (sensor_temp_readings.empty())
        {
            mvprintw(Y_OFFSET, X_OFFSET, "--- NO TEMPERATURE SENSORS DETECTED ---");
            refresh();
        }
        else
            for (const auto & reading : sensor_temp_readings)
            {
                const auto sensor_id = reading.first;
                const auto sensor_temp = reading.second.first;
                const auto sensor_timestamp = reading.second.second;

                std::stringstream ss;
                ss << sensor_id << " : " << (float)(sensor_temp)/100.0f << " deg.C ; " <<
                    "time since last reading (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::steady_clock::now() - sensor_timestamp).count();

                mvprintw(Y_OFFSET, X_OFFSET, "%s", ss.str().c_str());

                Y_OFFSET += 2;
                refresh();
            }
    
        std::this_thread::sleep_for(CONSOLE_UPDATE_INTERVAL);
    }

    delwin(console);
    endwin();
    refresh();

    std::cout << "Exiting console loop...\n" << std::flush;
}
