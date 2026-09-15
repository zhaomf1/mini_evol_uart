#include "pwm_burst_plan.h"

#include <stddef.h>

#define PWM_BURST_SEGMENT_PULSES 256U

bool PWM_BurstPlanCreate(uint32_t pulse_count, PWM_BurstPlan_t *plan)
{
    if ((pulse_count == 0U) || (plan == NULL))
    {
        return false;
    }

    plan->full_segments = pulse_count / PWM_BURST_SEGMENT_PULSES;
    plan->final_segment_pulses =
        (uint16_t)(pulse_count % PWM_BURST_SEGMENT_PULSES);
    return true;
}
