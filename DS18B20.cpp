#include "DS18B20.hpp"

#include <sstream>
#include <exception>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

temp_t DS18B20::parse_temp(const std::string & w1_output)
{    
    const auto temp_pos = w1_output.find("t=");
    if (temp_pos == std::string::npos)
        throw std::runtime_error("DS18B20::parse_temp error: \"t=\" substring not found\n");

    const std::string temp_str = w1_output.substr(temp_pos);
    temp_t parsed_temp;
    try
    {
        parsed_temp = std::stoul(temp_str);
    }
    catch (std::logic_error & e)
    {
        throw std::runtime_error("DS18B20::parse_temp error: could not parse temperature from string - \"" + 
            temp_str + "\" - " + e.what() + '\n');
    }

    return parsed_temp;
}

DS18B20::DS18B20(const std::string & w1_devices_path):
    m_device_path(w1_devices_path)
{}

temp_t DS18B20::read_temp()
{
    // find the directory entry for the device
    std::string ds18b20_path;
    try
    {
        const auto & w1_entries = fs::directory_iterator(m_device_path);
        for (const auto & entry : w1_entries)
        {
            const std::string entry_path{entry.path()};
            if (entry_path.find("28-",0) == 0)
            {
                ds18b20_path = entry_path;
                break;
            }
        }
    }
    catch (std::exception & e)
    {
        std::cerr << "One-wire device path iterator error: " << e.what() << '\n';
        return temp_t(0);
    }

    if (ds18b20_path.empty())
    {
        std::cerr << "No DS18B20 devices found on one-wire bus\n";
        return temp_t(0);
    }

    std::fstream w1_file(ds18b20_path);
    if (!w1_file.is_open())
        throw std::runtime_error("Could not open DS18B20 device: " + ds18b20_path + '\n');

    return parse_temp(std::string(static_cast<std::stringstream const&>(
        std::stringstream() << w1_file.rdbuf()).str()));
}
