#include "pwm_timer_config.h"

#include <stddef.h>

/* TIM4 运行在 APB1 定时器时钟上（84MHz = APB1 42MHz × 2，因为 APB1 分频不是 1）。
 * 预分频设为 1 再除以 2，得到 42MHz 计数时钟——低到可在 16 位 ARR 内实现 1kHz。 */
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

    /* 每个周期所需的计数脉冲数（四舍五入）；ARR 存的是减一后的值。 */
    period_counts = (TIM4_COUNTER_CLOCK_HZ + (requested_frequency_hz / 2U)) /
                    requested_frequency_hz;

    if ((period_counts == 0U) || (period_counts > 65536U))
    {
        return PWM_TIMER_CONFIG_RANGE_ERROR;
    }

    config->prescaler = TIM4_PRESCALER;
    config->auto_reload = (uint16_t)(period_counts - 1U);
    /* 整数 ARR 只能逼近部分频率；回填实际产生的频率，让上位机看到真实值而非请求值。 */
    config->actual_frequency_hz = TIM4_COUNTER_CLOCK_HZ / period_counts;

    return PWM_TIMER_CONFIG_OK;
}
