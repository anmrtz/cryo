#include "console_ui.hpp"

#include <atomic>
#include <string>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <thread>

extern std::atomic_bool terminate_flag;

console_ui::console_ui(const std::shared_ptr<cryo_control> & cryo_ptr) :
    control_ui(cryo_ptr)
{}

void console_ui::update_sensor_reading(const sensor_state_t & state)
{
    std::lock_guard<std::mutex> lock(m_temp_lock);

    if (!m_last_updates.count(state.first))
        m_last_updates[state.first] = state.second.second;
    else
        m_last_updates[state.first] = m_sensor_states.at(state.first).second;

    m_sensor_states[state.first] = state.second;
}

static inline void print_help()
{
    std::cout << "\nList of commands:\n\n"
        "on\t\t-> Turns GPIO PWM on\n"
        "off\t\t-> Turns GPIO PWM off\n"
        "duty %%\t\t-> Sets GPIO PWM duty cycle to %% (e.x. duty 50)\n"
        "temp #\t\t-> Sets target temperature in degrees Celsius (e.x. temp 6)\n"
        "status\t\t-> Prints status\n"
        "help\t\t-> Prints help menu\n\n"
        "kill\t\t-> Turns off all PWM and kills the program\n"
        "\nNotes:\n"
        "- All command parameters must be entered as integers (no decimals)\n"
        "- Duty percentages are numbered " << DUTY_CYCLE_MIN << " to " << DUTY_CYCLE_MAX << "\n"
        "- Temperature settings are numbered " << TEMP_SETTING_MIN << " to " << TEMP_SETTING_MAX << "\n"
        "- Command lines with extra words/characters are invalid\n\n";
}

static inline void print_status()
{
    std::cout << "\nCryo status:\n\n";

    // TODO
}

static inline bool valid_temp(temp_t temp)
{
    if (temp < TEMP_SETTING_MIN || temp > TEMP_SETTING_MAX)
    {
        std::cout << "ERROR: Specified LED must be integer from " <<
            TEMP_SETTING_MIN << " to " << TEMP_SETTING_MAX << '\n';
        return false;
    }
    return true;
}

static inline bool valid_duty(duty_t percent)
{
    if (percent < 0 || percent > 100)
    {
        std::cout << "ERROR: Duty percentage must be integer within [0,100]\n";
        return false;
    }
    return true;
}

static inline void turn_off()
{
    // TODO
}

static inline void turn_on()
{
    // TODO
}

void console_ui::console_task()
{
    std::cout << "\n*****************************"
              "\nCryo interface initialized!"
              "\n*****************************\n\n";
    print_status();
    print_help();

    char line[128];
    char token[128];
    duty_t set_duty;
    temp_t set_temp;
    while(true)
    {
        if (terminate_flag)
            return;

        std::cout << "\n**************************************"
                  "\nEnter command ('help' to view manual):\n";
        auto err = scanf("%127[^\n]",line);
        if (err != 1)
        {
            std::cout << "ERROR: Empty line recieved!\n";
            continue;
        }
        line[127] = 0;
        token[0] = 0;
        if (sscanf(line,"%s",token) == EOF)
        {
            std::cout << "ERROR: line is only whitespace!\n";
            continue;
        }

        int word_count = 0;
        bool letter_found = false;
        for (int i = 0; i < (int)strlen(line); i++)
        {
            if (line[i] == ' ')
                letter_found = false;
            else
                if (!letter_found)
                {
                    letter_found = true;
                    word_count++;
                }
        }

        std::cout << "\nLine recieved: " << line << "\n\n";

        if (strcmp(token,"duty") == 0 && word_count == 2 && sscanf(line,"%*s%u",&set_duty) == 1)
        {
            if (!valid_duty(set_duty))
                continue;
            std::cout << "Setting all cryo duty cycle to " << set_duty << "%%\n";

            // TODO
        }

        else if (strcmp(token,"temp") == 0 && word_count == 2 && sscanf(line,"%*s%u",&set_temp) == 1)
        {
            if (!valid_temp(set_temp))
                continue;
            std::cout << "Setting cryo target temperature to " << set_temp << " degrees Celsius\n";

            // TODO
        }

        else if (strcmp(token,"on") == 0 && word_count == 1)
        {
            std::cout << "Turning on temperature control...\n";

            turn_on();
        }

        else if (strcmp(token,"off") == 0 && word_count == 1)
        {
            std::cout << "Turning off temperature control...\n";

            turn_off();
        }

        else if (strcmp(token,"status") == 0 && word_count == 1)
        {
            print_status();
        }

        else if (strcmp(token, "help") == 0 && word_count == 1)
        {
            print_help();
        }

        else if (strcmp(token, "kill") == 0)
        {
            std::cout << "Deactivating all PWM and killing program...\n";
            
            turn_off();

            return;
        }

        else
        {
            std::cout << "ERROR: Invalid line sent. Check your command and parameter values. Make sure there are no extra characters.\n"
                << line << '\n';
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}