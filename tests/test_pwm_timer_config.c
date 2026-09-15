#include <assert.h>

#include "pwm_timer_config.h"

static void test_timer_config_covers_the_full_requested_range(void)
{
    PWM_TimerConfig_t config;

    assert(PWM_TimerConfigGet(1000U, &config) == PWM_TIMER_CONFIG_OK);
    assert(config.prescaler == 1U);
    assert(config.auto_reload == 41999U);
    assert(config.actual_frequency_hz == 1000U);

    assert(PWM_TimerConfigGet(10000U, &config) == PWM_TIMER_CONFIG_OK);
    assert(config.prescaler == 1U);
    assert(config.auto_reload == 4199U);
    assert(config.actual_frequency_hz == 10000U);

    assert(PWM_TimerConfigGet(100000U, &config) == PWM_TIMER_CONFIG_OK);
    assert(config.prescaler == 1U);
    assert(config.auto_reload == 419U);
    assert(config.actual_frequency_hz == 100000U);
    assert(PWM_TimerConfigGet(999U, &config) == PWM_TIMER_CONFIG_RANGE_ERROR);
    assert(PWM_TimerConfigGet(100001U, &config) == PWM_TIMER_CONFIG_RANGE_ERROR);
}

int main(void)
{
    test_timer_config_covers_the_full_requested_range();
    return 0;
}
