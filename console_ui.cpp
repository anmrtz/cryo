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
        "\tpwm on\t\t-> Turns GPIO PWM on\n"
        "\tpwm off\t\t-> Turns GPIO PWM off\n"
        "\tpower on\t-> Turns GPIO power pin on\n"
        "\tpower off\t-> Turns GPIO power pin off\n"
        "\tduty %\t\t-> Sets GPIO PWM duty cycle to % (e.x. duty 50)\n"
        "\ttemp #\t\t-> Sets target temperature to # degrees Celsius (e.x. temp 6)\n"
        "\tstatus\t\t-> Prints status\n"
        "\thelp\t\t-> Prints help menu\n"
        "\tquit\t\t-> Turns off all PWM and kills the program\n"
        "\nNotes:\n"
        "- All command parameters must be entered as integers (no decimals)\n"
        "- Duty percentages are numbered " << DUTY_CYCLE_MIN << " to " << DUTY_CYCLE_MAX << "\n"
        "- Temperature settings are numbered " << TEMP_SETTING_MIN << " to " << TEMP_SETTING_MAX << "\n"
        "- Command lines with extra words/characters are invalid\n\n";
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

    const std::chrono::nanoseconds time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()- temp_reading.time);

    std::cout << "\nCryo status:\n\n" <<
        "\tCurrent temperature:\t\t" << temp_reading.temp << '\n' <<
        "\tTime since last temp reading:\t" << time_to_str(time_ns) << '\n' <<
        "\tTarget temperature:\t\t" << cryo->get_temp_setting() << '\n' <<
        "\tDuty setting:\t\t\t" << cryo->get_duty_setting() << '\n' <<
        "\tCurrent duty cycle:\t\t" << cryo->get_current_duty() << '\n' <<
        "\tPWM enabled:\t\t\t" << cryo->is_pwm_enabled() << '\n' <<
        "\tPower enabled:\t\t\t" << cryo->is_power_enabled() << '\n' <<
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
    duty_t set_duty;
    temp_t set_temp;
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

        if (tokens.at(0) == "duty" && tokens.size() == 2)
        {
            try
            {
                set_duty = std::stoul(tokens.at(1));
            }
            catch (std::logic_error & e)
            {
                std::cout << "String to unsigned conversion error: " << e.what() << '\n';
            }

            if (!valid_duty(set_duty))
                continue;
            std::cout << "Setting all cryo duty cycle to " << set_duty << "%\n";

            get_cryo_ptr()->update_duty_setting(set_duty);
        }

        else if (tokens.at(0) == "temp" && tokens.size() == 2)
        {
            try
            {
                set_temp = std::stoul(tokens.at(1));
            }
            catch (std::logic_error & e)
            {
                std::cout << "String to unsigned conversion error: " << e.what() << '\n';
            }

            if (!valid_temp(set_temp))
                continue;
            std::cout << "Setting cryo target temperature to " << set_temp << " degrees Celsius\n";

            get_cryo_ptr()->update_temp_setting(set_temp);
        }

        else if (tokens.at(0) == "pwm" && tokens.at(1) == "on" && tokens.size() == 2)
        {
            std::cout << "Turning on PWM...\n";

            get_cryo_ptr()->set_pwm_enable(true);
        }

        else if (tokens.at(0) == "pwm" && tokens.at(1) == "off" && tokens.size() == 2)
        {
            std::cout << "Turning off PWM...\n";

            get_cryo_ptr()->set_pwm_enable(false);
        }

        else if (tokens.at(0) == "power" && tokens.at(1) == "on" && tokens.size() == 2)
        {
            std::cout << "Turning on power pin...\n";

            get_cryo_ptr()->set_power_enable(true);
        }

        else if (tokens.at(0) == "power" && tokens.at(1) == "off" && tokens.size() == 2)
        {
            std::cout << "Turning off power pin...\n";

            get_cryo_ptr()->set_power_enable(false);
        }

        else if ((tokens.at(0) == "status" || tokens.at(0) == "s") && tokens.size() == 1)
        {
            print_status();
        }

        else if (tokens.at(0) == "help" && tokens.size() == 1)
        {
            print_help();
        }

        else if (tokens.at(0) == "quit" && tokens.size() == 1)
        {
            std::cout << "Deactivating all GPIO and killing program...\n";
            
            get_cryo_ptr()->set_pwm_enable(false);
            get_cryo_ptr()->set_power_enable(false);

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
