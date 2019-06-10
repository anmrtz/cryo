#include "pwm_control.hpp"

#include <stdexcept>
#include <thread>

#include <pigpio.h>

pwm_control::pwm_control(gpio_t gpio_pin, freq_t frequency) :
    m_pwm_gpio_pin(gpio_pin),
    m_stored_freq_setting(frequency)
{
    if (gpioInitialise() < 0)
        throw std::runtime_error("pwm_control error: gpioInitialise() failed\n");

    restart_pwm(0,true);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

pwm_control::~pwm_control()
{
    pwm_off();
    gpioTerminate();
}

void pwm_control::set_duty(duty_t duty_cycle)
{
    m_stored_duty_setting = duty_cycle;
    if (m_is_pwm_on)
        restart_pwm(m_stored_duty_setting);
}

duty_t pwm_control::get_duty() const
{
    return gpioGetPWMdutycycle(m_pwm_gpio_pin) / 10'000;
}

void pwm_control::pwm_off()
{
    restart_pwm(0);
    m_is_pwm_on = false;
}

void pwm_control::pwm_on()
{
    restart_pwm(m_stored_duty_setting);
    m_is_pwm_on = true;
}

inline void pwm_control::restart_pwm(duty_t duty_cycle, bool force)
{
    if (duty_cycle < 0 || duty_cycle > 100) // verify duty cycle
        throw std::runtime_error("pwm_control error: invalid duty cycle - must be in range 0 <= duty_cycle <= 100\n");

    if (get_duty() == duty_cycle && !force) // if already running at that duty cycle, then don't do anything
        return;

    if (gpioHardwarePWM(m_pwm_gpio_pin, m_stored_freq_setting, duty_cycle * 10'000) < 0)
        throw std::runtime_error("pwm_control error: gpioHardwarePWM() failed\n");
}
