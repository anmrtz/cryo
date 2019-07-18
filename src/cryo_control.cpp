#include <vector>
#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <atomic>

#include "cryo_control.hpp"
#include "adc_temp.hpp"

extern std::atomic_bool terminate_flag;

static constexpr std::chrono::seconds TONE_DURATION{2};

static constexpr gpio_t TONE_GPIO_PIN{13};
static constexpr freq_t TONE_FREQ_HZ{420};

static constexpr gpio_t PWM_GPIO_PIN{18};
static constexpr freq_t PWM_FREQ_HZ{6000};

cryo_control::cryo_control() :
    m_pwm_control(std::make_shared<pwm_control>(PWM_GPIO_PIN,PWM_FREQ_HZ)),
    m_tone_control(std::make_shared<pwm_control>(TONE_GPIO_PIN,TONE_FREQ_HZ)),
    m_temp_sensor(std::make_shared<adc_temp>())
{}

cryo_control::~cryo_control()
{
    m_pwm_control->pwm_off();
}

void cryo_control::control_loop()
{
    m_pwm_control->pwm_off();
    m_pwm_control->set_duty(DUTY_CYCLE_ACTIVE);

    m_tone_control->pwm_off();
    m_tone_control->set_duty(50);

    while(!terminate_flag)
    {
        temp_reading_t temp_reading;
        temp_reading.temp = m_temp_sensor->read_temp();
        temp_reading.time = std::chrono::steady_clock::now().time_since_epoch();
        m_last_temp_reading = temp_reading;

        if (temp_reading.temp <= m_temp_setting)
        {
            m_pwm_control->pwm_off();
            if (is_cooling_active())
                start_tone();
            set_cooling_active(false);
        }
        else if (m_power_enabled)
            m_pwm_control->pwm_on();
        else
            m_pwm_control->pwm_off();

        if (m_tone_control->get_duty() && std::chrono::steady_clock::now() - m_tone_start > TONE_DURATION)
            stop_tone();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void cryo_control::register_ui_observer(const std::shared_ptr<control_ui> & ui_ptr)
{
    m_active_ifaces.push_back(ui_ptr);
}

bool cryo_control::update_temp_setting(const temp_t & temp)
{
    if (temp < TEMP_SETTING_MIN || temp > TEMP_SETTING_MAX)
    {
        std::cout << "cryo_control::update_temp_setting error: Specified temperature must be integer from " <<
            TEMP_SETTING_MIN << " to " << TEMP_SETTING_MAX << '\n';
        return false;
    }

    m_temp_setting = temp;
    return true;
}

temp_reading_t cryo_control::get_last_temp_reading() const
{
    return m_last_temp_reading;
}

temp_t cryo_control::get_temp_setting() const
{
    return m_temp_setting;
}

duty_t cryo_control::get_current_duty() const
{
    return m_pwm_control->get_duty();
}

temp_t cryo_control::read_temp_sensor()
{
    return m_temp_sensor->read_temp();
}

void cryo_control::set_cooling_active(bool is_enabled)
{
    m_power_enabled = is_enabled;
    if (m_power_enabled)
        m_start_time = std::chrono::steady_clock::now();
    else
    {
        m_stop_time = std::chrono::steady_clock::now();
    }
}

bool cryo_control::is_cooling_active() const
{
    return m_power_enabled;
}

duration_t cryo_control::get_active_duration() const
{
    if (m_power_enabled)
        return std::chrono::steady_clock::now() - m_start_time.load();
    else
        return m_stop_time.load() - m_start_time.load();
}

void cryo_control::start_tone()
{
    m_tone_start = std::chrono::steady_clock::now();
    m_tone_control->pwm_on();
}

void cryo_control::stop_tone()
{
    m_tone_control->pwm_off();
}
