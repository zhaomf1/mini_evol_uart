#include <assert.h>

#include "pwm_burst_plan.h"

int main(void)
{
    PWM_BurstPlan_t plan;

    assert(PWM_BurstPlanCreate(10000U, &plan));
    assert(plan.full_segments == 39U);
    assert(plan.final_segment_pulses == 16U);
    assert(PWM_BurstPlanCreate(256U, &plan));
    assert(plan.full_segments == 1U);
    assert(plan.final_segment_pulses == 0U);
    assert(PWM_BurstPlanCreate(1U, &plan));
    assert(plan.full_segments == 0U);
    assert(plan.final_segment_pulses == 1U);
    return 0;
}
