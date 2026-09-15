#ifndef PWM_TIMER_CONFIG_H
#define PWM_TIMER_CONFIG_H

#include <stdint.h>

typedef enum
{
    PWM_TIMER_CONFIG_OK = 0,
    PWM_TIMER_CONFIG_RANGE_ERROR
} PWM_TimerConfigStatus_t;

typedef struct
{
    uint16_t prescaler;
    uint16_t auto_reload;
    uint32_t actual_frequency_hz;
} PWM_TimerConfig_t;

PWM_TimerConfigStatus_t PWM_TimerConfigGet(uint32_t requested_frequency_hz,
                                            PWM_TimerConfig_t *config);

#endif /* PWM_TIMER_CONFIG_H */
