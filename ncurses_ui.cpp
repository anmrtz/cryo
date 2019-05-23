#include "ncurses_ui.hpp"

#include <atomic>

#include <curses.h>
#include <menu.h>

extern std::atomic<bool> terminate_flag;

void ncurses_ui::console_loop()
{
    WINDOW * console{initscr()};
    if (!console)
        throw std::runtime_error("ncurses_ui error: initscr failed\n");
    noecho();

    mvaddstr(5, 10, "Debug values:");
    refresh();
    
    while (!terminate_flag.load())
    {
        deleteln();
        mvprintw(7, 10, "Temperature reading: %f",temp_reading++);
        mvprintw(8, 10, "Temperature setting: %f",temp_setting++);
        refresh();

        std::this_thread::sleep_for(CONSOLE_UPDATE_INTERVAL);
    }

    delwin(console);
    endwin();
    refresh();

    std::cout << "Exiting console loop...\n" << std::flush;
}
