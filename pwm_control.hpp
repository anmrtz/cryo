#pragma once

#include "defs.hpp"

class pwm_control
{
    public:

    pwm_control(gpio_t gpio_pin, freq_t frequency);
    virtual ~pwm_control();

    void set_duty(duty_t duty_cycle);
    duty_t get_duty() const;

    void pwm_off();
    void pwm_on();
    bool is_pwm_on();

    protected:

    pwm_control() = default;

    inline void restart_pwm(duty_t duty_cycle, bool force = false);

    const gpio_t m_pwm_gpio_pin{0};
    const freq_t m_stored_freq_setting{0};
    duty_t m_stored_duty_setting{0};
    bool m_is_pwm_on{false};
};
