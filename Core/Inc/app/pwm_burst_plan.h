#ifndef PWM_BURST_PLAN_H
#define PWM_BURST_PLAN_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint32_t full_segments;
    uint16_t final_segment_pulses;
} PWM_BurstPlan_t;

bool PWM_BurstPlanCreate(uint32_t pulse_count, PWM_BurstPlan_t *plan);

#endif
