#include "pwm_timer_config.h"

#include <stddef.h>

#define TIM4_INPUT_CLOCK_HZ       84000000U
#define TIM4_PRESCALER            1U
#define TIM4_COUNTER_CLOCK_HZ     (TIM4_INPUT_CLOCK_HZ / (TIM4_PRESCALER + 1U))
#define PWM_MIN_FREQUENCY_HZ       1000U
#define PWM_MAX_FREQUENCY_HZ       100000U

PWM_TimerConfigStatus_t PWM_TimerConfigGet(uint32_t requested_frequency_hz,
                                            PWM_TimerConfig_t *config)
{
    uint32_t period_counts;

    if ((config == NULL) || (requested_frequency_hz < PWM_MIN_FREQUENCY_HZ) ||
        (requested_frequency_hz > PWM_MAX_FREQUENCY_HZ))
    {
        return PWM_TIMER_CONFIG_RANGE_ERROR;
    }

    period_counts = (TIM4_COUNTER_CLOCK_HZ + (requested_frequency_hz / 2U)) /
                    requested_frequency_hz;

    if ((period_counts == 0U) || (period_counts > 65536U))
    {
        return PWM_TIMER_CONFIG_RANGE_ERROR;
    }

    config->prescaler = TIM4_PRESCALER;
    config->auto_reload = (uint16_t)(period_counts - 1U);
    config->actual_frequency_hz = TIM4_COUNTER_CLOCK_HZ / period_counts;

    return PWM_TIMER_CONFIG_OK;
}
