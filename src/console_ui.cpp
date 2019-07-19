#include "console_ui.hpp"

#include <atomic>
#include <string>
#include <iterator>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

extern std::atomic_bool terminate_flag;

console_ui::console_ui(const std::shared_ptr<cryo_control> & cryo_ptr) :
    control_ui(cryo_ptr)
{}

static inline void print_help()
{
    std::cout << "\nList of commands:\n\n"
        "\tstart\t\t-> Turns cooling system on\n"
        "\tstop\t\t-> Turns cooling system off\n"
        "\tpidon\t\t-> Turns PID temperature holding on\n"
        "\tpidoff\t\t-> Turns PID temperature holding off\n"
        "\ttemp #\t\t-> Sets target temperature to # degrees Celsius (e.x. temp 6)\n"
        "\tstatus\t\t-> Prints status\n"
        "\thelp\t\t-> Prints help menu\n"
        "\tquit\t\t-> Turns off all PWM and kills the program\n"
        "\nNotes:\n"
        "- All command parameters must be entered as integers (no decimals)\n"
        "- Duty percentages are numbered " << DUTY_CYCLE_MIN << " to " << DUTY_CYCLE_MAX << "\n"
        "- Temperature settings are numbered " << TEMP_SETTING_MIN/1000 << " to " << TEMP_SETTING_MAX/1000 << "\n"
        "- Command lines with extra words/characters are invalid\n\n";
}

static std::string time_to_str(std::chrono::nanoseconds ns)
{
    std::stringstream os;
    char fill = os.fill();
    os.fill('0');

    const auto m = std::chrono::duration_cast<std::chrono::minutes>(ns);
    ns -= m;
    const auto s = std::chrono::duration_cast<std::chrono::seconds>(ns);
    ns -= s;
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(ns);
    ns -= ms;

    os << std::setw(2) << m.count() << "m:"
       << std::setw(2) << s.count() << "."
       << std::setw(3) << ms.count() << "s";
    os.fill(fill);
    return os.str();
}

void console_ui::print_status() const
{
    auto cryo = get_cryo_ptr();
    const auto temp_reading = cryo->get_last_temp_reading();

    const std::chrono::nanoseconds last_read_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()- temp_reading.time);

    const std::chrono::nanoseconds active_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            cryo->get_active_duration());

    std::cout << std::boolalpha << "\nCryo status:\n\n" <<
        "\tCurrent temperature:\t\t" << (float)temp_reading.temp/1000.0 << '\n' <<
        "\tTime since last temp reading:\t" << time_to_str(last_read_time_ns) << '\n' <<
        "\tTarget temperature:\t\t" << (float)cryo->get_temp_setting()/1000.0 << '\n' <<
        "\tCurrent duty cycle:\t\t" << cryo->get_current_duty() << '\n' <<
        "\tCooling enabled:\t\t" << cryo->is_cooling_active() << '\n' <<
        "\tPID enabled:\t\t\t" << cryo->is_pid_mode() << '\n' <<
        "\tActive duration:\t\t" << time_to_str(active_time_ns) << '\n' <<
        "\n";
}

void console_ui::console_task()
{
    std::cout << "\n*****************************"
              "\nCryo interface initialized!"
              "\n*****************************\n\n";
    //print_status();
    print_help();

    std::string line;
    std::string token;
    float set_temp;
    while(true)
    {
        if (terminate_flag)
            break;

        std::cout << "\n**************************************"
                  "\nEnter command ('help' to view manual):\n";
        std::getline(std::cin,line);
        if (line.empty())
            continue;

        std::istringstream iss(line);
        std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)),std::istream_iterator<std::string>());
        if (tokens.empty())
            continue;

        if (tokens.at(0) == "temp" && tokens.size() == 2)
        {
            try
            {
                set_temp = std::stof(tokens.at(1));
            }
            catch (std::logic_error & e)
            {
                std::cout << "String to unsigned conversion error: " << e.what() << '\n';
            }

            if (get_cryo_ptr()->update_temp_setting((temp_t)(set_temp*1000)))
                std::cout << "Setting cryo target temperature to " << set_temp << " degrees C\n";
            else
                std::cout << "Failed to set cryo target temperature to " << set_temp << " degrees C\n";
        }

        else if (tokens.at(0) == "start" && tokens.size() == 1)
        {
            std::cout << "Turning on cooling system...\n";

            get_cryo_ptr()->set_cooling_active(true);
        }

        else if (tokens.at(0) == "stop" && tokens.size() == 1)
        {
            std::cout << "Turning off cooling system...\n";

            get_cryo_ptr()->set_cooling_active(false);
        }

        else if (tokens.at(0) == "pidon" && tokens.size() == 1)
        {
            std::cout << "Turning on PID control...\n";

            get_cryo_ptr()->set_pid_mode(true);
        }

        else if (tokens.at(0) == "pidoff" && tokens.size() == 1)
        {
            std::cout << "Turning off PID control...\n";

            get_cryo_ptr()->set_pid_mode(false);
        }

        else if ((tokens.at(0) == "status" || tokens.at(0) == "s") && tokens.size() == 1)
        {
            print_status();
        }

        else if (tokens.at(0) == "help" && tokens.size() == 1)
        {
            print_help();
        }

        else if ((tokens.at(0) == "quit" || tokens.at(0) == "exit") && tokens.size() == 1)
        {
            std::cout << "Deactivating all GPIO and killing program...\n";
            
            get_cryo_ptr()->set_cooling_active(false);

            terminate_flag = true;

            break;
        }

        else
        {
            std::cout << "ERROR: Invalid line sent. Check your command and parameter values. Make sure there are no extra characters.\n"
                << line << '\n';
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Console terminated\n";
}
